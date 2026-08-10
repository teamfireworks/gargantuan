const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Constraint",
	description = "Abstract class for constraint-based instances.",
	since = "0.1.0",
	abstract = true,

	properties = {
		Active = {
			description = "Whether if the constraint and both of its parts are in a WorldRoot and the Constraint.Enabled property is true.",
			type = t.Bool,
			unmodified = "false",
			overrideRead = "GetActive",
		},
		Attachment0 = {
			description = "First attachment of this constraint.",
			type = t.Optional(t.Instance("Attachment")),
			unmodified = "std::nullopt",
			serializable = true,
		},
		Attachment1 = {
			description = "Second attachment of this constraint.",
			type = t.Optional(t.Instance("Attachment")),
			unmodified = "std::nullopt",
			serializable = true,
		},
		Enabled = {
			description = "Whether this constraint is enabled.",
			type = t.Bool,
			unmodified = "true",
			serializable = true,
		},
	},
})
