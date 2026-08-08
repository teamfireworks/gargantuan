#include "gargantuan/classes/Part.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/render/MeshProvider.hpp"
#include "gargantuan/scripting/Userdata.hpp"

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
