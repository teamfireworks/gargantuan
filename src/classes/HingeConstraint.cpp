#include "gargantuan/classes/HingeConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	HingeConstraint::HingeConstraint() {
		BindJointProperties({
			"ActuatorType",
			"AngularVelocity",
			"LimitsEnabled",
			"LowerAngle",
			"MotorMaxTorque",
			"TargetAngle",
			"UpperAngle",
		});
	}

	b3JointId HingeConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3RevoluteJointDef jointDefinition = b3DefaultRevoluteJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = true;
		return b3CreateRevoluteJoint(*world, &jointDefinition);
	}

	void HingeConstraint::UpdateJoint() {
		UpdateJointFrames();

		b3RevoluteJoint_EnableLimit(LeJoint, this->LimitsEnabled);
		b3RevoluteJoint_SetLimits(LeJoint, this->LowerAngle, this->UpperAngle);

		bool isMotor = this->ActuatorType == Enums::ActuatorType::Motor;
		b3RevoluteJoint_EnableMotor(LeJoint, isMotor);
		if (isMotor) {
			b3RevoluteJoint_SetMaxMotorTorque(LeJoint, this->MotorMaxTorque);
			b3RevoluteJoint_SetMotorSpeed(LeJoint, this->AngularVelocity);
			// couldn't find a way to put in MotorMaxSpeed easily so thats a TODO
			// same with the AngularSpeed
		}

		// The revolute target angle is driven by the spring solver, so without
		// this the servo branch does nothing at all.
		bool isServo = this->ActuatorType == Enums::ActuatorType::Servo;
		b3RevoluteJoint_EnableSpring(LeJoint, isServo);
		if (isServo) {
			b3RevoluteJoint_SetTargetAngle(LeJoint, this->TargetAngle);
		}
	}
}
