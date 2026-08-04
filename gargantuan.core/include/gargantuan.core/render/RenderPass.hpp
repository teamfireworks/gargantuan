#pragma once

#include "gargantuan/classes/Camera.hpp"
#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/render/Shader.hpp"

#include <SDL3/SDL.h>
#include <memory>

namespace gargantuan {
	struct DrawContext {
		std::shared_ptr<WorldRoot> WorldRoot;
		std::shared_ptr<Camera> Camera;

		// Direction TOWARDS the light
		glm::vec3 LightDirection;
	};

	struct FrameContext : DrawContext {
		SDL_GPUCommandBuffer *Commands;

		SDL_GPUTexture *SwapchainTexture;
		SDL_GPUTexture *DepthTexture;

		SDL_GPUTexture *ShadowMapTexture;
		SDL_GPUSampler *ShadowSampler;
		glm::mat4 ShadowMatrix;

		uint32_t Width;
		uint32_t Height;
	};

	class RenderPass {
	  public:
		Shader Shader;
		SDL_GPUGraphicsPipeline *Pipeline = nullptr;

		virtual ~RenderPass() = default;
		virtual SDL_GPURenderPass *Draw(SDL_GPUDevice *gpu, FrameContext &context) = 0;
		virtual void Resize(SDL_GPUDevice *gpu, uint32_t width, uint32_t height) {};
		virtual void Destroy(SDL_GPUDevice *gpu);
	};
} // namespace gargantuan
