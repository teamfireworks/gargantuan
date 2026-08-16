#include "gargantuan/classes/HingeConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	b3JointId HingeConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3RevoluteJointDef jointDefinition = b3DefaultRevoluteJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		if (this->LimitsEnabled) {
			jointDefinition.lowerAngle = this->LowerAngle;
			jointDefinition.upperAngle = this->UpperAngle;
		}
		jointDefinition.base.collideConnected = true;
		return b3CreateRevoluteJoint(*world, &jointDefinition);
	}
}
