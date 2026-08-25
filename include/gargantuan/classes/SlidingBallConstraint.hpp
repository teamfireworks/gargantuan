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
	};
}
