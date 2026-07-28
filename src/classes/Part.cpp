#include "gargantuan/classes/Part.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/MeshProvider.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <SDL3/SDL_log.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>

namespace gargantuan {
	const Instance::ClassDefinition Part::DEFINITION = {
		.Name = "Part",
		.Superclass = "BasePart",
		.Constructor = ClassDefinition::WrapConstructor<Part>(),
		.Properties = {
			{"Shape", Property::fromSimple<&Part::Shape>(true, true).SetSerializable()},
		}
	};

	std::unique_ptr<GpuMesh> &Part::GetMesh() const {
		std::string key = "gargantuan://meshes/" + std::string(magic_enum::enum_name(Shape));
		return MeshProvider::GetGpuMesh(key);
	};
}
