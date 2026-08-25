const d, t, p = require("@tools/declare").unpack()

return d.datatype({
	name = "Color3",
	description = "A color value comprised of red, green, and blue components.",
	since = "0.1.0",

	members = {
		R = d.property({
			description = "The red component of this color from 0 to 1.",
			type = t.Float,
			unmodified = "0.0f",
			withMember = {},
		}),
		G = d.property({
			description = "The green component of this color from 0 to 1.",
			type = t.Float,
			unmodified = "0.0f",
			withMember = {},
		}),
		B = d.property({
			description = "The blue component of this color from 0 to 1.",
			type = t.Float,
			unmodified = "0.0f",
			withMember = {},
		}),

		__add = d.method({
			description = "Adds two Color3s",
			withMember = {
				constant = true,
				parameters = { p("other", t.Color3:ConstRef()) },
				returns = { t.Color3 },
			},
		}),
	},

	library = {
		new = d.method({
			description = "Constructs a new Color3 using red, green, and blue components from 0 to 1.",
			withMember = {
				parameters = {
					p("red", t.Float:Optional()),
					p("green", t.Float:Optional()),
					p("blue", t.Float:Optional()),
				},
				returns = { t.Color3 },
			},
		}),

		fromRGB = d.method({
			description = "Constructs a new Color3 using red, green, and blue components from 1 to 255.",
			withMember = {
				parameters = { p("red", t.Float), p("green", t.Float), p("blue", t.Float) },
				returns = { t.Color3 },
			},
		}),

		fromHSV = d.method({
			description = "Constructs a new Color3 using hue, saturation, and value components from 0 to 1.",
			withMember = {
				parameters = { p("hue", t.Float), p("saturation", t.Float), p("value", t.Float) },
				returns = { t.Color3 },
			},
		}),
	},
})
