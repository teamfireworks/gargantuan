#include "gargantuan/classes/HingeConstraint.hpp"
#include "gargantuan/math/Conversions.hpp"
#include <algorithm>
#include <box3d/box3d.h>
#include <box3d/math_functions.h>
#include <box3d/types.h>
#include <cmath>
#include <trigonometric.hpp>

namespace gargantuan {
	HingeConstraint::HingeConstraint() {
		BindJointProperties({
			"ActuatorType",
			"AngularSpeed",
			"AngularVelocity",
			"LimitsEnabled",
			"LowerAngle",
			"MotorMaxTorque",
			"ServoMaxTorque",
			"TargetAngle",
			"UpperAngle",
		});
	}

	b3JointId HingeConstraint::CreateJoint(b3WorldId *world, b3BodyId body0, b3BodyId body1) {
		b3RevoluteJointDef jointDefinition = b3DefaultRevoluteJointDef();
		jointDefinition.base.bodyIdA = body0;
		jointDefinition.base.bodyIdB = body1;
		jointDefinition.base.collideConnected = false; // roblox parity
		return b3CreateRevoluteJoint(*world, &jointDefinition);
	}

	void HingeConstraint::UpdateJoint() {
		UpdateJointFrames();

		constexpr float limit = 0.99f * B3_PI;
		b3RevoluteJoint_EnableLimit(LeJoint, this->LimitsEnabled);
		b3RevoluteJoint_SetLimits(
			LeJoint,
			std::clamp(B3_DEG_TO_RAD * this->LowerAngle, -limit, limit),
			std::clamp(B3_DEG_TO_RAD * this->UpperAngle, -limit, limit)
		);

		bool isMotor = this->ActuatorType == Enums::ActuatorType::Motor;
		bool isServo = this->ActuatorType == Enums::ActuatorType::Servo;

		b3RevoluteJoint_EnableSpring(LeJoint, false);
		b3RevoluteJoint_EnableMotor(LeJoint, isMotor || isServo);

		if (isMotor) {
			b3RevoluteJoint_SetMaxMotorTorque(LeJoint, AsB3Limit(this->MotorMaxTorque));
			b3RevoluteJoint_SetMotorSpeed(LeJoint, this->AngularVelocity);
		} else if (isServo) {
			b3RevoluteJoint_SetMaxMotorTorque(LeJoint, AsB3Limit(this->ServoMaxTorque));
			b3Joint_WakeBodies(LeJoint);
		}
	}

	void HingeConstraint::StepJoint(float deltaTime) {
		if (!b3Joint_IsValid(LeJoint)) return;

		float angle = b3RevoluteJoint_GetAngle(LeJoint);

		float degrees = B3_RAD_TO_DEG * angle;
		if (degrees != this->CurrentAngle) SetCurrentAngle(degrees);

		if (this->ActuatorType != Enums::ActuatorType::Servo) return;

		float error = std::remainder(B3_DEG_TO_RAD * this->TargetAngle - angle, 2.0f * B3_PI);

		float speed = AsB3Limit(this->AngularSpeed);
		b3RevoluteJoint_SetMotorSpeed(LeJoint, std::clamp(error / deltaTime, -speed, speed));
	}

	glm::quat HingeConstraint::GetFrameCorrection() const {
		return glm::angleAxis(glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
