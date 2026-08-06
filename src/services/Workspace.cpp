#include "gargantuan/services/Workspace.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	I_IMPL(
		Workspace,
		.Superclass = "WorldRoot",
		.Properties = {
			{"CurrentCamera", Property::fromMember<&Workspace::CurrentCamera>(true, true)},
		}
	);
}
