#pragma once

#include "gargantuan/classes/generated/RodConstraint.hpp"

namespace gargantuan {
	class RodConstraint : public Constraint {
		I_RodConstraint;

		RodConstraint();

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
		void UpdateJoint() override;
	};
}
