#pragma once

#include "gargantuan/classes/generated/PrismaticConstraint.hpp"

namespace gargantuan {
	class PrismaticConstraint : public SlidingBallConstraint {
		I_PrismaticConstraint;

		PrismaticConstraint();

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
		void UpdateJoint() override;
	};
}
