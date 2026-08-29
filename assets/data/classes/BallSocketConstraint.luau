const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "BallSocketConstraint",
	extends = "Constraint",
	description = "	Forces its two attachments into the same position and allows them to freely rotate about all three axes, with optional limits to restrict both tilt and twist. ",
	since = "0.1.0",

	properties = {
		MaxFrictionTorque = {
			description = "Sets the maximum frictional torque applied to keep its Attachments aligned.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		Restitution = {
			description = "How elastic Attachments connected by a BallSocketConstraint will be when they reach the end of the range specified by UpperAngle when LimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		LimitsEnabled = {
			description = "Sets whether the BallSocketConstraint sets a limit on rotation based on UpperAngle.",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		UpperAngle = {
			description = "Sets the upper rotation limit of the BallSocketConstraint, as long as LimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		TwistLimitsEnabled = {
			description = "Sets whether the BallSocketConstraint sets a limit on twist rotation based on TwistUpperAngle and TwistLowerAngle.",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		TwistLowerAngle = {
			description = "Sets the lower twist rotation limit of the BallSocketConstraint, as long as TwistLimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		TwistUpperAngle = {
			description = "Sets the upper twist rotation limit of the BallSocketConstraint, as long as TwistLimitsEnabled is true.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
	},
})
