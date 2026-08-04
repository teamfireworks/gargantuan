#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <glm/glm.hpp>

namespace gargantuan {
	struct Vertex {
	  public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;

		static std::array<SDL_GPUVertexBufferDescription, 1> BufferDescriptions[];
		static std::array<SDL_GPUVertexAttribute, 3> Attributes[];
	};

	struct Mesh {
	  public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};
}; // namespace gargantuan
