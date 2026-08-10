const c, t, p = require("@tools/classes").unpack()

return c({
	name = "WeldConstraint",
	-- DEVIATION: Inherit from Constraint here unlike Roblox to simplify the
	-- WorldRoot physics
	extends = "Constraint",
	description = "	Connects two BaseParts together such that their relative position and orientation remain the same.",
	since = "0.1.0",

	properties = {
		Part0 = {
			description = "First part of this constraint.",
			type = t.Optional(t.Instance("BasePart")),
			unmodified = "std::nullopt",
			serializable = true,
		},
		Part1 = {
			description = "Second part of this constraint.",
			type = t.Optional(t.Instance("BasePart")),
			unmodified = "std::nullopt",
			serializable = true,
		},
	},
})
