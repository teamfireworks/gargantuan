#include "gargantuan.core/render/GpuMesh.hpp"
#include "gargantuan.core/render/Mesh.hpp"
#include <cstring>

namespace gargantuan {
	GpuMesh::GpuMesh(Mesh mesh) {
		this->Vertices = mesh.Vertices;
		this->VertexCount = Vertices.size();
		this->VertexBufferSize = VertexCount * sizeof(Vertex);

		this->Indices = mesh.Indices;
		this->IndexCount = Indices.size();
		this->IndexBufferSize = IndexCount * sizeof(uint32_t);
	}

	SDL_GPUBuffer *GpuMesh::CreateVertexBuffer(SDL_GPUDevice *gpu) {
		if (VertexBuffer) {
			return VertexBuffer;
		}

		SDL_GPUBufferCreateInfo info = {.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = VertexBufferSize};
		VertexBuffer = SDL_CreateGPUBuffer(gpu, &info);

		return VertexBuffer;
	}

	SDL_GPUBuffer *GpuMesh::CreateIndexBuffer(SDL_GPUDevice *gpu) {
		if (IndexBuffer) {
			return IndexBuffer;
		}

		SDL_GPUBufferCreateInfo info = {.usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = IndexBufferSize};
		IndexBuffer = SDL_CreateGPUBuffer(gpu, &info);

		return IndexBuffer;
	}

	SDL_GPUTransferBuffer *GpuMesh::CreateTransferBuffer(SDL_GPUDevice *gpu) {
		if (TransferBuffer) {
			return TransferBuffer;
		}

		SDL_GPUTransferBufferCreateInfo info = {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = VertexBufferSize + IndexBufferSize,
		};

		TransferBuffer = SDL_CreateGPUTransferBuffer(gpu, &info);

		void *pointer = SDL_MapGPUTransferBuffer(gpu, TransferBuffer, false);
		std::memcpy(pointer, Vertices.data(), VertexBufferSize);
		std::memcpy((uint8_t *)pointer + VertexBufferSize, Indices.data(), IndexBufferSize);
		SDL_UnmapGPUTransferBuffer(gpu, TransferBuffer);

		return TransferBuffer;
	}

	void GpuMesh::DestroyTransferBuffer(SDL_GPUDevice *gpu) {
		if (TransferBuffer) {
			SDL_ReleaseGPUTransferBuffer(gpu, TransferBuffer);
			TransferBuffer = nullptr;
		}
	}

	void GpuMesh::Upload(SDL_GPUDevice *gpu, SDL_GPUCopyPass *copyPass) {
		auto transferBuffer = CreateTransferBuffer(gpu);

		SDL_GPUTransferBufferLocation vertexSource{.transfer_buffer = transferBuffer, .offset = 0};
		SDL_GPUBufferRegion vertexDestination{.buffer = CreateVertexBuffer(gpu), .offset = 0, .size = VertexBufferSize};
		SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);

		SDL_GPUTransferBufferLocation indexSource{.transfer_buffer = transferBuffer, .offset = VertexBufferSize};
		SDL_GPUBufferRegion indexDestination{.buffer = CreateIndexBuffer(gpu), .offset = 0, .size = IndexBufferSize};
		SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);

		DestroyTransferBuffer(gpu);
	}

	void GpuMesh::Destroy(SDL_GPUDevice *gpu) {
		DestroyTransferBuffer(gpu);

		if (VertexBuffer) {
			SDL_ReleaseGPUBuffer(gpu, VertexBuffer);
			VertexBuffer = nullptr;
		}

		if (IndexBuffer) {
			SDL_ReleaseGPUBuffer(gpu, IndexBuffer);
			IndexBuffer = nullptr;
		}
	}
} // namespace gargantuan
