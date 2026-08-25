#pragma once

#include "gargantuan/classes/generated/BallSocketConstraint.hpp"

namespace gargantuan {
	class BallSocketConstraint : public Constraint {
		I_BallSocketConstraint;

		BallSocketConstraint();

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
		void UpdateJoint() override;
	};
}
