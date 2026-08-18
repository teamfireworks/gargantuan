#include "gargantuan/classes/BallSocketConstraint.hpp"
#include "gargantuan/math/Conversions.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	b3JointId BallSocketConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3SphericalJointDef jointDefinition = b3DefaultSphericalJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.localFrameA.p = AsB3Vec3(this->Attachment0->get()->GetCFrame().Position);
		jointDefinition.base.localFrameA.q = AsB3Quat(this->Attachment0->get()->GetCFrame().ToQuaternion());
		jointDefinition.base.localFrameB.p = AsB3Vec3(this->Attachment1->get()->GetCFrame().Position);
		jointDefinition.base.localFrameB.q = AsB3Quat(this->Attachment1->get()->GetCFrame().ToQuaternion());
		jointDefinition.enableConeLimit = this->LimitsEnabled;
		jointDefinition.coneAngle = this->UpperAngle;
		jointDefinition.enableTwistLimit = this->TwistLimitsEnabled;
		jointDefinition.upperTwistAngle = this->TwistUpperAngle;
		jointDefinition.lowerTwistAngle = this->TwistLowerAngle;
		jointDefinition.base.collideConnected = true;
		// for restitution values experiment with the spring cuz idk how it works yet
		return b3CreateSphericalJoint(*world, &jointDefinition);
	}
}
