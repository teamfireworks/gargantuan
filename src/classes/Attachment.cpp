#include "gargantuan/classes/Attachment.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		Attachment,
		.Description = "Used by the physics solver to determine points for constraints.",
		.Properties = {
			{"CFrame", Property::fromMember<&Attachment::CFrame>(true, true).SetSerializable()},
			// TODO: add the world relative stuff, axis and etc, CFrame is relative to the part its parented to
			// and also also WeldConstraint and HingeConstraint to be added and like everything else and like idk but
		}
	);
}
