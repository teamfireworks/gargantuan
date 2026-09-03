const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "SlidingBallConstraint",
	extends = "Constraint",
	description = "	The base class for constraints that allow their attachments to slide along an axis but not rotate. ",
	since = "0.1.0",

	properties = {
		ActuatorType = {
			description = "Sets whether the rotation of the PrismaticConstraint is actuated and, if so, what kind of actuation it uses.",
			type = t.EnumItem("ActuatorType"),
			unmodified = "Enums::ActuatorType::None",
		},
		CurrentPosition = {
			description = "Read only property that contains the current position of the constriant.",
			type = t.Float,
			unmodified = "0",
			serializable = false,
			writeLevel = "Never",
		},
		LimitsEnabled = {
			description = "Sets wheter the PrismaticConstraint will limit the range of rotation.",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		UpperLimit = {
			description = "The higher positional limit the PrismaticConstraint will allow if LimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		LowerLimit = {
			description = "The lower positional limit the PrismaticConstraint will allow if LimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		TargetPosition = {
			description = "The PrismaticConstraint's attempted target position when ActuatorType is set to Servo.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		Speed = {
			description = "The desired speed a PrismaticConstraint with ActuatorType set to Servo will attempt to maintain while moving towards its TargetPosition. Measured in units per second.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		ServoMaxForce = {
			description = "The PrismaticConstraint's maximum force when ActuatorType is set to Servo, as the constraint attempts to reach its desired Speed.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		LinearResponsiveness = {
			description = 'Specifies the "sharpness" of the linear servo motor in reaching the TargetPosition.',
			type = t.Float,
			unmodified = "1",
			serializable = true,
		},
		Velocity = {
			description = "The PrismaticConstraint's attempted velocity when ActuatorType is set to Motor. Measured in units per second.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		MotorMaxAcceleration = {
			description = "The PrismaticConstraint's maximum acceleration when ActuatorType is set to Motor as the constraint attempts to reach its desired Velocity.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		MotorMaxForce = {
			description = "The maximum force a PrismaticConstraint with ActuatorType set to Motor can apply when trying to reach its desired Velocity.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
	},
})
