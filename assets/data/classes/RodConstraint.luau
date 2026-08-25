const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "RodConstraint",
	extends = "Constraint",
	description = "	Constrains its attachments to maintain a fixed distance. ",
	since = "0.1.0",

	properties = {
		CurrentDistance = {
			description = "Read only property that contains the current distance of the constriant.",
			type = t.Float,
			unmodified = "0",
			serializable = false,
			writeLevel = "Never",
		},
		Length = {
			description = "The target distance. Measured in units.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		LimitsEnabled = {
			description = "Sets wheter the RodConstraint will limit the angles of rod and respective attachments.",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		LimitAngle0 = {
			description = "Maximum angle the RodConstraint will allow between the Rod and Attachment0.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		LimitAngle1 = {
			description = "Maximum angle the RodConstraint will allow between the Rod and Attachment1.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
	},
})
