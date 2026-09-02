#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <glm/glm.hpp>

namespace gargantuan {
	static constexpr SDL_GPUTextureFormat RENDERER_COLOR_FORMAT = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
	static constexpr SDL_GPUTextureFormat RENDERER_DEPTH_FORMAT = SDL_GPU_TEXTUREFORMAT_D16_UNORM;

	struct Vertex {
	  public:
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;

		static std::array<SDL_GPUVertexBufferDescription, 1> BufferDescriptions[];
		static std::array<SDL_GPUVertexAttribute, 3> Attributes[];
	};

	enum GuiTextureType : int { Color = -1, Font = -2 };

	struct GuiVertex {
	  public:
		glm::vec2 AbsolutePosition;
		glm::vec2 AbsoluteSize;
		glm::vec2 UV;
		glm::vec4 Background = {0.0f, 0.0f, 0.0f, 0.0f};
		float Rotation = 0;
		GuiTextureType TextureIndex = GuiTextureType::Color;

		static std::array<SDL_GPUVertexBufferDescription, 1> BufferDescriptions[];
		static std::array<SDL_GPUVertexAttribute, 5> Attributes[];
	};

	struct Mesh {
	  public:
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};
};
