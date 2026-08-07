#include "gargantuan/classes/Constraint.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(
		Constraint,
		.Description = "The base class for constraint-based objects.",
		.Properties = {
			{"Active", Property::fromMember<&Constraint::Active>(true, false).SetSerializable()},
			{"Attachment0", Property::fromMember<&Constraint::Attachment0>(true, true).SetSerializable()},
			{"Attachment1", Property::fromMember<&Constraint::Attachment1>(true, true).SetSerializable()},
			{"Enabled", Property::fromMember<&Constraint::Enabled>(true, true).SetSerializable()},
		}
	)
} // namespace gargantuan
