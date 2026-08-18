const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "SpringConstraint",
	extends = "Constraint",
	description = "	Constrains its attachments to maintain a fixed distance using a spring. ",
	since = "0.1.0",

	properties = {
		CurrentLength = {
			description = "Read only property that contains the current distance of the constriant.",
			type = t.Float,
			unmodified = "0",
			serializable = false,
			writeLevel = "Never",
		},
		FreeLength = {
			description = "Natural resting length of the spring.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		MaxForce = {
			description = "The maximum force the SpringConstraint can apply on its Attachments.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
	 	Stiffness = {
			description = "The strength of the spring. The higher this value the more force will be applied when the attachments are separated a different length than the FreeLength.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		Damping = {
			description = "Damping constant for the SpringConstraint. Multiplied to the velocity of the constraint's Attachments to reduce the spring force applied.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		--limits stuff
		LimitsEnabled = {
			description = "Sets wheter the RodConstraint will limit the angles of rod and respective attachments.",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		MinLength = {
			description = "Maximum angle the RodConstraint will allow between the Rod and Attachment0.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
		MaxLength = {
			description = "Maximum angle the RodConstraint will allow between the Rod and Attachment1.",
			type = t.Float,
			unmodified = "0",
			serializable = true,
		},
	},
})
