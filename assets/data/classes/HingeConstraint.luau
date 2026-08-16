const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "HingeConstraint",
	extends = "Constraint",
	description = "	Constrains its attachments to rotate about a single axis. ",
	since = "0.1.0",

	properties = {
		CurrentAngle = {
			description = "Read only property that contains the current angle of the constriant.",
			type = t.Float, --i think but idk how the types.luau works cuz i'm new to it
			unmodified = "0", --idk what this means
			serializable = false, --how do i make it read only
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
			description = "The target angle lol",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		--TODO: add the others
	},
})
