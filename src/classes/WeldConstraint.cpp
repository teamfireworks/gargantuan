#include "gargantuan/classes/WeldConstraint.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		WeldConstraint,
		.Superclass = "Constraint",
		.Properties = {
			{"Part0", Property::fromMember<&WeldConstraint::Part0>(true, true).SetSerializable()},
			{"Part1", Property::fromMember<&WeldConstraint::Part1>(true, true).SetSerializable()},
		}
	)
} // namespace gargantuan
