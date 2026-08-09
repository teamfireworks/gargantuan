const c, t, p = require("@tools/classes").unpack()

return c({
	name = "GuiBase2d",
	extends = "GuiBase",
	description = "Intermediate abstract class for instances that compose 2D graphical user interface",
	abstract = true,
	since = "0.1.0",

	properties = {
		AbsolutePosition = {
			description = "Screen position of this instance in pixels",
			type = t.Vector2,
			unmodified = "gargantuan::Vector2(0.0f, 0.0f)",
		},
		AbsoluteRotation = {
			description = "Screen rotation of this instance in degrees",
			type = t.Float,
			unmodified = "0.0f",
		},
		AbsoluteSize = {
			description = "Screen size of this instance in pixels",
			type = t.Vector2,
			unmodified = "gargantuan::Vector2(0.0f, 0.0f)",
		},
	},
})
