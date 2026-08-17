#include "gargantuan/render/RenderPass.hpp"

#include <SDL3/SDL.h>

namespace gargantuan {
	void RenderPass::Destroy(SDL_GPUDevice *gpu) {
		if (Pipeline) {
			SDL_ReleaseGPUGraphicsPipeline(gpu, Pipeline);
			Pipeline = nullptr;
		}

		Shader.Destroy(gpu);
	}
}
