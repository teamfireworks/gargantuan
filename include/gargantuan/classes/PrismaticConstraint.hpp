#pragma once

#include "gargantuan/classes/generated/PrismaticConstraint.hpp"

namespace gargantuan {
	G_ENUM( // hi fire did i do the enum correctly
		ActuatorType,

		None,
		Motor,
		Servo,
	)

	class PrismaticConstraint : public Constraint {
		I_PrismaticConstraint;

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
	};
}
