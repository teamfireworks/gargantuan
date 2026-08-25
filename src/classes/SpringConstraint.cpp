#include "gargantuan/classes/SpringConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	SpringConstraint::SpringConstraint() {
		BindJointProperties({
			"Damping",
			"FreeLength",
			"LimitsEnabled",
			"MaxLength",
			"MinLength",
			"Stiffness",
		});
	}

	b3JointId SpringConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3DistanceJointDef jointDefinition = b3DefaultDistanceJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = true;
		return b3CreateDistanceJoint(*world, &jointDefinition);
	}

	void SpringConstraint::UpdateJoint() {
		UpdateJointFrames();

		b3DistanceJoint_EnableSpring(LeJoint, true);
		b3DistanceJoint_SetLength(LeJoint, this->FreeLength);
		b3DistanceJoint_SetSpringDampingRatio(LeJoint, this->Damping);
		// tweak these two incase they look bad
		b3DistanceJoint_SetSpringForceRange(LeJoint, this->Stiffness, this->Stiffness);

		b3DistanceJoint_EnableLimit(LeJoint, this->LimitsEnabled);
		b3DistanceJoint_SetLengthRange(LeJoint, this->MinLength, this->MaxLength);
	}
}
