#include "gargantuan/classes/RodConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	RodConstraint::RodConstraint() {
		BindJointProperties({
			"LimitsEnabled",
			"Length",
		});
	}

	b3JointId RodConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3DistanceJointDef jointDefinition = b3DefaultDistanceJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = true;
		// tbh this would probably be better as a spring joint
		// probably just combine with a spherical joint
		return b3CreateDistanceJoint(*world, &jointDefinition);
	}

	void RodConstraint::UpdateJoint() {
		UpdateJointFrames();

		b3DistanceJoint_SetLength(LeJoint, this->Length);
		b3DistanceJoint_EnableLimit(LeJoint, this->LimitsEnabled);
		b3DistanceJoint_SetLengthRange(LeJoint, this->Length, this->Length); // fix limits (angle wise)
	}
}
