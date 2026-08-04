#include "gargantuan.core/services/Workspace.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"
#include "gargantuan.runtime/Userdata.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		Workspace,
		.Superclass = "WorldRoot",
		.Properties = {
			{"CurrentCamera", Property::fromMember<&Workspace::CurrentCamera>(true, true)},
		}
	);
}
