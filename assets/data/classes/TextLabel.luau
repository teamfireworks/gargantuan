const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "TextLabel",
	extends = "GuiObject",
	description = "A 2D user interface element that displays non-interactive text.",
	since = "0.1.0",

	properties = {
		FontFace = {
			description = "Determines the font used to render text.",
			type = t.Font,
			unmodified = 'gargantuan::Font::fromName("TitiliumWeb")',
		},
		LineHeight = {
			description = "TBA.",
			type = t.Float,
			unmodified = "1.0f",
		},
		Text = {
			description = "Determines the string rendered by this instance.",
			type = t.String,
			unmodified = '""',
		},
		TextColor3 = {
			description = "Determines the color of rendered text.",
			type = t.Color3,
			unmodified = "gargantuan::Color3()",
		},
		TextSize = {
			description = "Determines the line height in pixel offset.",
			type = t.Float,
			unmodified = "12.0f",
		},
		TextTransparency = {
			description = "Determines the transparency of rendered text.",
			type = t.Float,
			unmodified = "0.0f",
		},
	},
})
