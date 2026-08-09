#include "gargantuan/classes/Part.hpp"
#include "gargantuan/render/MeshProvider.hpp"

#include <SDL3/SDL.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>

namespace gargantuan {

	std::unique_ptr<GpuMesh> &Part::GetMesh() const {
		std::string key = "gargantuan://meshes/" + std::string(magic_enum::enum_name(Shape));
		return MeshProvider::GetGpuMesh(key);
	};
}
