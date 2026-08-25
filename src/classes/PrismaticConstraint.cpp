#include "gargantuan/classes/PrismaticConstraint.hpp"
#include <box3d/box3d.h>
#include <box3d/types.h>

namespace gargantuan {
	PrismaticConstraint::PrismaticConstraint() {
		BindJointProperties({
			"ActuatorType",
			"LimitsEnabled",
			"LowerLimit",
			"MotorMaxForce",
			"TargetPosition",
			"UpperLimit",
			"Velocity",
		});
	}

	b3JointId PrismaticConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3PrismaticJointDef jointDefinition = b3DefaultPrismaticJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = true;
		return b3CreatePrismaticJoint(*world, &jointDefinition);
	}

	void PrismaticConstraint::UpdateJoint() {
		UpdateJointFrames();

		b3PrismaticJoint_EnableLimit(LeJoint, this->LimitsEnabled);
		b3PrismaticJoint_SetLimits(LeJoint, this->LowerLimit, this->UpperLimit);

		bool isMotor = this->ActuatorType == Enums::ActuatorType::Motor;
		b3PrismaticJoint_EnableMotor(LeJoint, isMotor);
		if (isMotor) {
			b3PrismaticJoint_SetMaxMotorForce(LeJoint, this->MotorMaxForce);
			b3PrismaticJoint_SetMotorSpeed(LeJoint, this->Velocity);
			// couldn't find a way to put in MotorMaxSpeed easily so thats a TODO
			// same with the Speed
		}

		bool isServo = this->ActuatorType == Enums::ActuatorType::Servo;
		b3PrismaticJoint_EnableSpring(LeJoint, isServo);
		if (isServo) {
			b3PrismaticJoint_SetTargetTranslation(LeJoint, this->TargetPosition);
		}
	}
}
