#pragma once

#include "gargantuan/render/Mesh.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace gargantuan {
	struct GpuMesh : public Mesh {
	  public:
		SDL_GPUBuffer *VertexBuffer = nullptr;
		uint32_t VertexCount;
		uint32_t VertexBufferSize;

		SDL_GPUBuffer *IndexBuffer = nullptr;
		uint32_t IndexCount;
		uint32_t IndexBufferSize;

		GpuMesh(Mesh mesh);

		SDL_GPUTransferBuffer *TransferBuffer = nullptr;

		SDL_GPUBuffer *CreateVertexBuffer(SDL_GPUDevice *gpu);
		SDL_GPUBuffer *CreateIndexBuffer(SDL_GPUDevice *gpu);
		SDL_GPUTransferBuffer *CreateTransferBuffer(SDL_GPUDevice *gpu);
		void DestroyTransferBuffer(SDL_GPUDevice *gpu);

		void Upload(SDL_GPUDevice *gpu, SDL_GPUCopyPass *copyPass);
		void Destroy(SDL_GPUDevice *gpu);

		GpuMesh(const GpuMesh &) = delete;
		GpuMesh &operator=(const GpuMesh &) = delete;
	};
} // namespace gargantuan
