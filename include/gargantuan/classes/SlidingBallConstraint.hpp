#pragma once

#include "gargantuan/classes/generated/SlidingBallConstraint.hpp"

namespace gargantuan {
	G_ENUM(
		ActuatorType,

		None,
		Motor,
		Servo,
	)

	class SlidingBallConstraint : public Constraint {
		I_SlidingBallConstraint;

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
		void UpdateJoint() override;
	};
}
