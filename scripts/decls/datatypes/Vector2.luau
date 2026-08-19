const d, t, p = require("@tools/declare").unpack()

return d.datatype({
	name = "Vector2",
	description = "Two dimensional vector with direction and magnitude.",
	since = "0.1.0",

	properties = {
		X = d.property({
			description = "Horizontal dimension of this vector.",
			type = t.Float,
			unmodified = "{0.0f, 0.0f}",
			withManual = { getMethod = "GetX", setMethod = "SetX" },
		}),
		Y = d.property({
			description = "Vertical dimension of this vector.",
			type = t.Float,
			unmodified = "{0.0f, 0.0f}",
			withManual = { getMethod = "GetY", setMethod = "SetY" },
		}),
		Magnitude = d.property({
			description = "The length of the Vector2.",
			type = t.Float,
			withManual = { getMethod = "GetY", setMethod = "SetY" },
		}),
		Unit = d.property({
			description = "A normalized copy of the Vector2, maintaining it's direction but with a magnitude of one.",
			type = t.Vector2,

			overrideRead = "GetLookVector",
			overrideWrite = "SetLookVector",
		}),

		Cross = d.method({
			description = "Returns the cross product of two vectors.",
			withMember = { constant = true, parameters = { p("other", t.Vector2) }, returns = t.Float },
		}),
		Abs = d.method({
			description = "Constructs a new vector using the absolute values of this vector's components.",
			withMember = { constant = true, returns = t.Vector2 },
		}),
		Ceil = d.method({
			description = "Constructs a new vector using the ceiling of it's components.",
			withMember = { constant = true, returns = t.Vector2 },
		}),
		Floor = d.method({
			description = "Constructs a new vector using the floor of it's components.",
			withMember = { constant = true, returns = t.Vector2 },
		}),
		Sign = d.method({
			description = "Constructs a new vector using the sign (-1, 0, or 1) of it's components.",
			withMember = { constant = true, returns = t.Vector2 },
		}),
	},

	library = {
		xAxis = d.static({
			description = "A Vector2 with it's X component set to one.",
			type = t.Vector2,
			value = "{1.0f, 0.0f}",
		}),
		yAxis = d.static({
			description = "A Vector2 with it's Y component set to one.",
			type = t.Vector2,
			value = "{0.0f, 1.0f}",
		}),
		zero = d.static({
			description = "A Vector2 with it's components zeroed.",
			type = t.Vector2,
			value = "{0.0f, 0.0f}",
		}),
		one = d.static({
			description = "A Vector2 with it's components set to one.",
			type = t.Vector2,
			value = "{1.0f, 1.0f}",
		}),

		new = d.method({
			description = "Constructs a new vector with the given components.",
			withMember = {
				parameters = { p("x", t.Optional(t.Float)), p("y", t.Optional(t.Float)) },
				returns = t.Vector2,
			},
		}),
	},
})
