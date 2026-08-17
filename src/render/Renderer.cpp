// #define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/assets/FontProvider.hpp"
#include "gargantuan/filesystem/BaseFilesystem.hpp"
#include "gargantuan/render/MeshProvider.hpp"
#include "gargantuan/render/RenderPass.hpp"
#include "gargantuan/render/RenderPrimitives.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <format>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace gargantuan {
	static constexpr auto WINDOW_FLAGS = SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	static constexpr auto SHADER_FORMATS = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_METALLIB |
										   SDL_GPU_SHADERFORMAT_MSL;

	const std::vector<RenderPassConstructor> RENDER_PASS_CONSTRUCTORS{
		CreateShadowPass,
		CreateOpaquePass,
		CreateGuiPass,
	};

	SDLRenderer::SDLRenderer(Vector2 &viewportSize, BaseFilesystem *filesystem) : BaseRenderer(viewportSize) {
		Gpu = SDL_CreateGPUDevice(SHADER_FORMATS, true, nullptr);
		if (!Gpu) throw std::runtime_error(std::format("Failed to create GPU device: {}", SDL_GetError()));

		Window = SDL_CreateWindow("Gargantuan", viewportSize.GetX(), viewportSize.GetY(), WINDOW_FLAGS);
		if (!Window) throw std::runtime_error(std::format("Failed to create window: {}", SDL_GetError()));

		if (!SDL_ClaimWindowForGPUDevice(Gpu, Window)) {
			throw std::runtime_error(std::format("Failed to claim window for GPU: {}", SDL_GetError()));
		}

		SwapchainFormat = SDL_GetGPUSwapchainTextureFormat(Gpu, Window);

		Font = new FontProvider(Gpu, filesystem);

		SDL_GPUTextureCreateInfo shadowMapInfo{
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
			.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = 2048,
			.height = 2048,
			.layer_count_or_depth = 1,
			.num_levels = 1,
		};
		ShadowMapTexture = SDL_CreateGPUTexture(Gpu, &shadowMapInfo);

		SDL_GPUSamplerCreateInfo samplerInfo{
			.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
			.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
			.enable_compare = true,
		};
		ShadowSampler = SDL_CreateGPUSampler(Gpu, &samplerInfo);

		int width, height;
		SDL_GetWindowSizeInPixels(Window, &width, &height);
		Resize(width, height);

		for (auto &ctor : RENDER_PASS_CONSTRUCTORS) {
			RenderPasses.push_back(ctor(Gpu, SwapchainFormat));
		}
	}

	void SDLRenderer::Destroy() {
		SDL_WaitForGPUIdle(Gpu);

		MeshProvider::Destroy(Gpu);

		if (DepthTexture != nullptr) {
			SDL_ReleaseGPUTexture(Gpu, DepthTexture);
			DepthTexture = nullptr;
		};

		if (ShadowMapTexture) {
			SDL_ReleaseGPUTexture(Gpu, ShadowMapTexture);
			ShadowMapTexture = nullptr;
		}

		if (ShadowSampler) {
			SDL_ReleaseGPUSampler(Gpu, ShadowSampler);
			ShadowSampler = nullptr;
		}

		for (auto &pass : RenderPasses) {
			pass->Destroy(Gpu);
		}
		RenderPasses.clear();

		SDL_ReleaseWindowFromGPUDevice(Gpu, Window);
		SDL_DestroyGPUDevice(Gpu);
		SDL_DestroyWindow(Window);
	}

	void SDLRenderer::Draw(DrawContext drawContext) {
		MeshProvider::UploadToGpu(Gpu);

		if (!DepthTexture || !ShadowMapTexture) return;

		SDL_GPUCommandBuffer *commands = SDL_AcquireGPUCommandBuffer(Gpu);
		if (!commands) {
			SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
			return;
		}

		FrameContext frameContext;
		frameContext.Commands = commands;
		frameContext.WorldRoot = drawContext.WorldRoot;
		frameContext.Camera = drawContext.Camera;
		frameContext.Layers = drawContext.Layers;

		frameContext.Font = Font;
		frameContext.DepthTexture = DepthTexture;
		frameContext.ShadowMapTexture = ShadowMapTexture;
		frameContext.ShadowSampler = ShadowSampler;
		frameContext.LightDirection = glm::normalize(glm::vec3(0.75f, 1.0f, 0.5f));

		auto swapchainResult = SDL_AcquireGPUSwapchainTexture(
			frameContext.Commands, Window, &frameContext.SwapchainTexture, &frameContext.Width, &frameContext.Height
		);
		if (!swapchainResult) {
			LOG_TRACE(App, "Failed to acquire swapchain texture: %s", SDL_GetError());
			if (frameContext.Commands) SDL_CancelGPUCommandBuffer(frameContext.Commands);
			return;
		} else if (!frameContext.SwapchainTexture) {
			// Not worth warning
			// LOG_TRACE(App, "Acquired swapchain texture, but it is null");
			if (frameContext.Commands) SDL_CancelGPUCommandBuffer(frameContext.Commands);
			return;
		}

		SDL_GPUColorTargetInfo colorTarget = {
			.texture = frameContext.SwapchainTexture,
			.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE,
		};

		SDL_GPUDepthStencilTargetInfo depthTarget = {
			.texture = frameContext.DepthTexture,
			.clear_depth = 1.0f,
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_DONT_CARE,
			.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
			.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
		};

		for (auto &pass : RenderPasses) {
			SDL_EndGPURenderPass(pass->Draw(Gpu, frameContext));
		}

		SDL_SubmitGPUCommandBuffer(frameContext.Commands);
	}

	void SDLRenderer::Resize(int width, int height) {
		if (width < 1 || height < 1) return;

		// SDL_SetGPUSwapchainParameters(Gpu, Window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE);
		SDL_SetGPUSwapchainParameters(Gpu, Window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

		if (DepthTexture != nullptr) SDL_ReleaseGPUTexture(Gpu, DepthTexture);
		SDL_GPUTextureCreateInfo depthInfo{
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = RENDERER_DEPTH_FORMAT,
			.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
			.width = (uint32_t)width,
			.height = (uint32_t)height,
			.layer_count_or_depth = 1,
			.num_levels = 1,
		};
		DepthTexture = SDL_CreateGPUTexture(Gpu, &depthInfo);
	}
}
