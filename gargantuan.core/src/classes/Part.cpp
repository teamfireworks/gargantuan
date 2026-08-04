#include "gargantuan.core/classes/Part.hpp"
#include "gargantuan.core/datatypes/Instance.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"
#include "gargantuan.core/render/MeshProvider.hpp"
#include "gargantuan.runtime/Userdata.hpp"

#include <SDL3/SDL_log.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>

namespace gargantuan {
	G_INSTANCE_IMPL(
		Part,
		.Superclass = "BasePart",
		.Properties = {
			{"Shape", Property::fromMember<&Part::Shape>(true, true)},
		},
	);

	std::unique_ptr<GpuMesh> &Part::GetMesh() const {
		std::string key = "gargantuan://meshes/" + std::string(magic_enum::enum_name(Shape));
		return MeshProvider::GetGpuMesh(key);
	};
}
