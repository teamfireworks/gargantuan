#pragma once

#include "gargantuan/render/GpuMesh.hpp"

#include <string>
#include <memory>

namespace gargantuan::MeshProvider {
	std::unique_ptr<GpuMesh> &GetGpuMesh(std::string id);
	void UploadToGpu(SDL_GPUDevice *Gpu);
	void Destroy(SDL_GPUDevice *gpu);
}; // namespace gargantuan::MeshProvider
