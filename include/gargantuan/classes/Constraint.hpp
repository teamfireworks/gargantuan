#pragma once

#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/generated/Constraint.hpp"

#include <box3d/box3d.h>
#include <memory>
#include <tuple>

namespace gargantuan {
	class Constraint : public Instance {
		I_Constraint;

		virtual std::tuple<std::shared_ptr<BasePart>, std::shared_ptr<BasePart>> GetActiveParts() const;
		virtual b3JointId CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) = 0;
	};
}
