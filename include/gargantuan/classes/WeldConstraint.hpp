#pragma once

#include "gargantuan/classes/generated/WeldConstraint.hpp"

namespace gargantuan {
	class WeldConstraint : public Constraint {
		I_WeldConstraint;

		virtual std::tuple<std::shared_ptr<BasePart>, std::shared_ptr<BasePart>> GetActiveParts() const override;
		b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) override;
	};
}
