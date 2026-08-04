#include "gargantuan.core/render/Shader.hpp"
#include "gargantuan.core/filesystem/Paths.hpp"

#include <SDL3/SDL.h>

namespace gargantuan {
	std::filesystem::path GetShaderPath(const std::filesystem::path &relativePath) {
		return Paths::GetExecutableDirectory() / "shaders" / relativePath;
	}

	void
	GetShaderFormat(SDL_GPUDevice *gpu, SDL_GPUShaderFormat &format, std::string &extension, std::string &entrypoint) {
		SDL_GPUShaderFormat supportedFormats = SDL_GetGPUShaderFormats(gpu);
		if (supportedFormats & SDL_GPU_SHADERFORMAT_METALLIB) {
			format = SDL_GPU_SHADERFORMAT_METALLIB;
			extension = ".metallib";
			entrypoint = "main0";
		} else if (supportedFormats & SDL_GPU_SHADERFORMAT_MSL) {
			format = SDL_GPU_SHADERFORMAT_MSL;
			extension = ".metal";
			entrypoint = "main0";
		} else if (supportedFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
			format = SDL_GPU_SHADERFORMAT_SPIRV;
			extension = ".spv";
			entrypoint = "main";
		}
	}

	void Shader::Destroy(SDL_GPUDevice *gpu) {
		if (VertexShader) {
			SDL_ReleaseGPUShader(gpu, VertexShader);
			VertexShader = nullptr;
		}

		if (FragmentShader) {
			SDL_ReleaseGPUShader(gpu, FragmentShader);
			FragmentShader = nullptr;
		}
	}

	SDL_GPUShader *
	FileShader::CompileFile(SDL_GPUDevice *gpu, std::filesystem::path filepath, SDL_GPUShaderCreateInfo info) {
		size_t codeSize;
		void *code = SDL_LoadFile(filepath.string().c_str(), &codeSize);
		if (code == nullptr) {
			SDL_Log("Failed to open shader file %s", filepath.string().c_str());
			return nullptr;
		}

		info.code_size = codeSize;
		info.code = static_cast<const Uint8 *>(code);

		SDL_GPUShader *shader = SDL_CreateGPUShader(gpu, &info);
		SDL_free(code);

		if (shader == nullptr) {
			SDL_Log("Failed to create shader file %s: %s", filepath.c_str(), SDL_GetError());
			return nullptr;
		};

		return shader;
	}

	void FileShader::Init(SDL_GPUDevice *gpu) {
		SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
		std::string extension, entrypoint;
		GetShaderFormat(gpu, format, extension, entrypoint);

		if (!VertexShader) {
			std::string filepath = VertexFilepath.string() + extension;
			VertexShader = CompileFile(
				gpu,
				filepath.c_str(),
				{
					.entrypoint = entrypoint.c_str(),
					.format = format,
					.stage = SDL_GPU_SHADERSTAGE_VERTEX,
					.num_samplers = VertexSamplerCount,
					.num_storage_textures = 0,
					.num_storage_buffers = 0,
					.num_uniform_buffers = VertexUniformBufferCount,
				}
			);
		}

		if (!FragmentShader) {
			std::string filepath = FragmentFilepath.string() + extension;
			FragmentShader = CompileFile(
				gpu,
				filepath,
				{
					.entrypoint = entrypoint.c_str(),
					.format = format,
					.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
					.num_samplers = FragmentSamplerCount,
					.num_storage_textures = 0,
					.num_storage_buffers = 0,
					.num_uniform_buffers = FragmentUniformBufferCount,
				}
			);
		}
	}
} // namespace gargantuan
