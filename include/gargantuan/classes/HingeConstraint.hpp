#pragma once

#include "gargantuan/classes/generated/HingeConstraint.hpp"

namespace gargantuan {
	G_ENUM(
		ActuatorType,

		None,
		Motor,
		Servo,
	)

	class HingeConstraint : public Constraint {
		I_HingeConstraint;

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
	};
}
