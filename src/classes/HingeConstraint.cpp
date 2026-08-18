#include "gargantuan/classes/HingeConstraint.hpp"
#include "gargantuan/math/Conversions.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	b3JointId HingeConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3RevoluteJointDef jointDefinition = b3DefaultRevoluteJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.localFrameA.p = AsB3Vec3(this->Attachment0->get()->GetCFrame().Position);
		jointDefinition.base.localFrameA.q = AsB3Quat(this->Attachment0->get()->GetCFrame().ToQuaternion());
		jointDefinition.base.localFrameB.p = AsB3Vec3(this->Attachment1->get()->GetCFrame().Position);
		jointDefinition.base.localFrameB.q = AsB3Quat(this->Attachment1->get()->GetCFrame().ToQuaternion());
		if (this->ActuatorType == Enums::ActuatorType::Motor) {
			jointDefinition.enableMotor = true;
			jointDefinition.maxMotorTorque = this->MotorMaxTorque;
			jointDefinition.motorSpeed = this->AngularVelocity;
			// couldn't find a way to put in MotorMaxSpeed easily so thats a TODO
			// same with the AngularSpeed
		} else if (this->ActuatorType == Enums::ActuatorType::Servo) {
			jointDefinition.targetAngle = this->TargetAngle;
		}
		jointDefinition.enableLimit = this->LimitsEnabled;
		jointDefinition.lowerAngle = this->LowerAngle;
		jointDefinition.upperAngle = this->UpperAngle;
		jointDefinition.base.collideConnected = true;
		return b3CreateRevoluteJoint(*world, &jointDefinition);
	}
}
