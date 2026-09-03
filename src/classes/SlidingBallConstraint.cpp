#include "gargantuan/classes/SlidingBallConstraint.hpp"
#include <box3d/id.h>

namespace gargantuan {
	b3JointId SlidingBallConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		return b3_nullJointId;
	}

	void SlidingBallConstraint::UpdateJoint() {}
}
