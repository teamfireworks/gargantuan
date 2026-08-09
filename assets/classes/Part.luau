const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Part",
	extends = "BasePart",
	description = "Class that renders a primitive shape",
	since = "0.1.0",

	properties = {
		Shape = {
			description = "Primitive shape of this instance",
			type = t.EnumItem("PartType"),
			unmodified = "Enums::PartType::Block",
		},
	},
})
