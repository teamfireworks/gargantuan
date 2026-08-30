-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const l, c, t = require("@gluten/codegen").Unpack()

return l.Userdata({
	Name = "Color3",
	Description = "Represents a color made of red, green, and blue components.",
	Since = vector.create(0, 1, 0),
}, {
	R = c.Property({
		Type = t.Float,
		UnmodifiedValue = "0.0f",
		ImplementMember = true,
		Permits = { Read = { "Basic" }, ReadParallel = { "Basic" }, Write = { "Basic" }, WriteParallel = { "Basic" } },
		Display = {
			Description = "The red component of this color from zero to one.",
		},
	}),
	G = c.Property({
		Type = t.Float,
		UnmodifiedValue = "0.0f",
		ImplementMember = true,
		Permits = { Read = { "Basic" }, ReadParallel = { "Basic" }, Write = { "Basic" }, WriteParallel = { "Basic" } },
		Display = { Description = "The green component of this color from zero to one." },
	}),
	B = c.Property({
		Type = t.Float,
		UnmodifiedValue = "0.0f",
		ImplementMember = true,
		Permits = { Read = { "Basic" }, ReadParallel = { "Basic" }, Write = { "Basic" }, WriteParallel = { "Basic" } },
		Display = { Description = "The blue component of this color from zero to one." },
	}),

	ToHSV = c.Method({
		Display = { Description = "Converts this Color3 into hue, saturation, and value components." },
		ImplementMethod = {
			Parameters = {},
			Permits = { Call = { "Basic" }, CallParallel = { "Basic" } },
			ReturnType = t.Tuple(t.Float, t.Float, t.Float),
		},
	}),

	__add = c.Method({
		Display = { Description = "Sum of all components." },
		ImplementMethod = {
			Permits = { Call = { "Basic" }, CallParallel = { "Basic" } },
			Parameters = { c.Parameter("other", "Other Color3.", t.Color3) },
			ReturnType = t.Color3,
		},
	}),

	new = c.StaticMethod({
		Display = { Description = "Constructs a new Color3 with RGB components from zero to one" },
		ImplementMethod = {
			Permits = { Call = { "Basic" }, CallParallel = { "Basic" } },
			Parameters = {
				c.Parameter("r", "Red component.", t.Float, "0.0f"),
				c.Parameter("g", "Green component.", t.Float, "0.0f"),
				c.Parameter("b", "Blue component.", t.Float, "0.0f"),
			},
			ReturnType = t.Color3,
		},
	}),
})
