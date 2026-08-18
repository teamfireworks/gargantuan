#include "gargantuan/classes/SpringConstraint.hpp"
#include "gargantuan/math/Conversions.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	b3JointId SpringConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3DistanceJointDef jointDefinition = b3DefaultDistanceJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.localFrameA.p = AsB3Vec3(this->Attachment0->get()->GetCFrame().Position);
		jointDefinition.base.localFrameA.q = AsB3Quat(this->Attachment0->get()->GetCFrame().ToQuaternion());
		jointDefinition.base.localFrameB.p = AsB3Vec3(this->Attachment1->get()->GetCFrame().Position);
		jointDefinition.base.localFrameB.q = AsB3Quat(this->Attachment1->get()->GetCFrame().ToQuaternion());
		jointDefinition.enableSpring = true;
		jointDefinition.length = this->FreeLength;
		jointDefinition.enableLimit = this->LimitsEnabled;
		jointDefinition.minLength = this->MinLength;
		jointDefinition.maxLength = this->MaxLength;
		jointDefinition.dampingRatio = this->Damping;
		jointDefinition.upperSpringForce = this->Stiffness; // tweak these two incase they look bad
		jointDefinition.lowerSpringForce = this->Stiffness;
		jointDefinition.base.collideConnected = true;
		return b3CreateDistanceJoint(*world, &jointDefinition);
	}
}
