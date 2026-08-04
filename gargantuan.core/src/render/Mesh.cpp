#include "gargantuan.core/render/Mesh.hpp"

namespace gargantuan {
	std::array<SDL_GPUVertexBufferDescription, 1> Vertex::BufferDescriptions[]{
		SDL_GPUVertexBufferDescription{
			.slot = 0,
			.pitch = sizeof(Vertex),
			.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
		},
	};

	std::array<SDL_GPUVertexAttribute, 3> Vertex::Attributes[]{
		SDL_GPUVertexAttribute{
			.location = 0,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
			.offset = offsetof(Vertex, Position),
		},
		SDL_GPUVertexAttribute{
			.location = 1,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
			.offset = offsetof(Vertex, Normal),
		},
		SDL_GPUVertexAttribute{
			.location = 2,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = offsetof(Vertex, UV),
		},
	};
} // namespace gargantuan
