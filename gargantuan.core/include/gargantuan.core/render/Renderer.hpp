#pragma once

#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/render/RenderPass.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <memory>

namespace gargantuan {
	class BaseRenderer {
	  public:
		BaseRenderer(Vector2 &viewportSize) {};

		BaseRenderer(const BaseRenderer &) = delete;
		BaseRenderer &operator=(const BaseRenderer &) = delete;

		virtual void Draw(DrawContext context) = 0;
		virtual void Resize(int width, int height) = 0;
		virtual void Destroy() = 0;
	};

	class HeadlessRenderer final : public BaseRenderer {
	  public:
		HeadlessRenderer(Vector2 &viewportSize) : BaseRenderer(viewportSize) {};

		void Draw(DrawContext _context) override {};
		void Resize(int _width, int _height) override {};
		void Destroy() override {};
	};

	std::unique_ptr<RenderPass> CreateOpaquePass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat);
	std::unique_ptr<RenderPass> CreateShadowPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat);

	class SDLRenderer final : public BaseRenderer {
	  public:
		SDLRenderer(Vector2 &viewportSize);

		int Width, Height = 0;
		SDL_Window *Window = nullptr;
		SDL_GPUDevice *Gpu = nullptr;
		SDL_GPUTextureFormat SwapchainFormat;

		SDL_GPUTexture *DepthTexture = nullptr;
		SDL_GPUTexture *ShadowMapTexture = nullptr;
		SDL_GPUSampler *ShadowSampler = nullptr;

		std::unique_ptr<RenderPass> ShadowPass;
		std::unique_ptr<RenderPass> OpaquePass;

		void Draw(DrawContext drawContext) override;
		void Resize(int width, int height) override;
		void Destroy() override;
	};
} // namespace gargantuan
