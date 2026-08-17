#include "gargantuan/render/RenderPrimitives.hpp"

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

	std::array<SDL_GPUVertexBufferDescription, 1> GuiVertex::BufferDescriptions[]{
		SDL_GPUVertexBufferDescription{
			.slot = 0,
			.pitch = sizeof(GuiVertex),
			.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
		},
	};

	std::array<SDL_GPUVertexAttribute, 5> GuiVertex::Attributes[]{
		SDL_GPUVertexAttribute{
			.location = 0,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = offsetof(GuiVertex, AbsolutePosition),
		},
		SDL_GPUVertexAttribute{
			.location = 1,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = offsetof(GuiVertex, AbsoluteSize),
		},
		SDL_GPUVertexAttribute{
			.location = 2,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
			.offset = offsetof(GuiVertex, UV),
		},
		SDL_GPUVertexAttribute{
			.location = 3,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
			.offset = offsetof(GuiVertex, Background),
		},
		SDL_GPUVertexAttribute{
			.location = 4,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_INT,
			.offset = offsetof(GuiVertex, TextureIndex),
		},
	};
} // namespace gargantuan
