#include "gargantuan/classes/BallSocketConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	BallSocketConstraint::BallSocketConstraint() {
		BindJointProperties({
			"LimitsEnabled",
			"TwistLimitsEnabled",
			"TwistLowerAngle",
			"TwistUpperAngle",
			"UpperAngle",
		});
	}

	b3JointId BallSocketConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3SphericalJointDef jointDefinition = b3DefaultSphericalJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = false; // roblox parity again
		return b3CreateSphericalJoint(*world, &jointDefinition);
	}

	void BallSocketConstraint::UpdateJoint() {
		UpdateJointFrames();

		b3SphericalJoint_EnableConeLimit(LeJoint, this->LimitsEnabled);
		b3SphericalJoint_SetConeLimit(LeJoint, this->UpperAngle);

		b3SphericalJoint_EnableTwistLimit(LeJoint, this->TwistLimitsEnabled);
		b3SphericalJoint_SetTwistLimits(LeJoint, this->TwistLowerAngle, this->TwistUpperAngle);
	}

	glm::quat BallSocketConstraint::GetFrameCorrection() const {
		return glm::angleAxis(glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
