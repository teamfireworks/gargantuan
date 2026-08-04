#pragma once

#include <SDL3/SDL.h>
#include <filesystem>

namespace gargantuan {
	std::filesystem::path GetShaderPath(const std::filesystem::path &relativePath);

	void
	GetShaderFormat(SDL_GPUDevice *gpu, SDL_GPUShaderFormat &format, std::string &extension, std::string &entrypoint);

	struct Shader {
	  public:
		SDL_GPUShader *VertexShader = nullptr;
		SDL_GPUShader *FragmentShader = nullptr;

		/// Instantiates the vertex and fragment shaders if it doesn't exist.
		void Init(SDL_GPUDevice *gpu);

		/// Release associated shaders from the GPU.
		void Destroy(SDL_GPUDevice *gpu);
	};

	struct FileShader final : public Shader {
	  public:
		std::filesystem::path VertexFilepath;
		Uint32 VertexUniformBufferCount = 1;
		Uint32 VertexSamplerCount = 0;

		std::filesystem::path FragmentFilepath;
		Uint32 FragmentUniformBufferCount = 0;
		Uint32 FragmentSamplerCount = 0;

		void Init(SDL_GPUDevice *gpu);

	  private:
		SDL_GPUShader *CompileFile(SDL_GPUDevice *gpu, std::filesystem::path path, SDL_GPUShaderCreateInfo info);
	};
} // namespace gargantuan
