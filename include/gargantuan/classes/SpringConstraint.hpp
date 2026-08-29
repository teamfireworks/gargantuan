#pragma once

#include "gargantuan/classes/generated/SpringConstraint.hpp"

namespace gargantuan {
	class SpringConstraint : public Constraint {
		I_SpringConstraint;

		SpringConstraint();

		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
		void UpdateJoint() override;
	};
}
