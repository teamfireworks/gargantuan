const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "HingeConstraint",
	extends = "Constraint",
	description = "	Constrains its attachments to rotate about a single axis. ",
	since = "0.1.0",

	properties = {
		ActuatorType = {
			description = "Sets whether the rotation of the HingeConstraint is actuated and, if so, what kind of actuation it uses.",
			type = t.EnumItem("ActuatorType"),
			unmodified = "Enums::ActuatorType::None",
		},
		CurrentAngle = {
			description = "Read only property that contains the current angle of the constriant.",
			type = t.Float,
			unmodified = "0",
			serializable = false,
			writeLevel = "Never",
		},
		LimitsEnabled = {
			description = "Sets wheter the HingeConstraint will limit the range of rotation.",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		UpperAngle = {
			description = "Minimum rotation angle the HingeConstraint will allow if LimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		LowerAngle = {
			description = "Maximum rotation angle the HingeConstraint will allow if LimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		TargetAngle = {
			description = "The target angle a HingeConstraint will attempt to rotate to if its ActuatorType is set to Servo. Measured in radians.", --TODO: radian -> degree and vice versa conversion (because roblox measures these in degrees)
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		AngularSpeed = {
			description = "The desired angular speed a HingeConstraint with ActuatorType set to Servo will attempt to maintain while rotating towards its TargetAngle. Measured in radians/second.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		ServoMaxTorque = {
			description = "The maximum torque a HingeConstraint with ActuatorType set to Servo can apply when trying to reach its desired TargetAngle.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		AngularVelocity = {
			description = "The angular velocity a HingeConstraint with ActuatorType set to Motor will attempt to achieve. Measured in radians/second.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		MotorMaxAcceleration = {
			description = "The maximum angular acceleration a HingeConstraint with ActuatorType set to Motor can apply to achieve its AngularVelocity. Measured in radians/second^2.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		MotorMaxTorque = {
			description = "The maximum torque a HingeConstraint with ActuatorType set to Motor can apply when trying to reach its desired AngularVelocity.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		--TODO: add the others
	},
})
