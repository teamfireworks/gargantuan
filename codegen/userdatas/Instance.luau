-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const l, c, t = require("@gluten/codegen").Unpack()

return l.Userdata("Instance", {
	Description = "Internal implementation of the root Instance class.",
	Since = vector.create(0, 1, 0),
}, {
	__index = c.StaticMethod({
		Display = {
			Description = "Indexes properties and children.",
		},
		ImplementManual = {
			Signature = "(never) -> never",
			Permits = {
				Call = { "Basic" },
				CallParallel = { "Basic" },
			},
		},
	}),

	__namecall = c.StaticMethod({
		Display = {
			Description = "Calls methods on this instance class.",
		},
		ImplementManual = {
			Signature = "(never) -> never",
			Permits = {
				Call = { "Basic" },
				CallParallel = { "Basic" },
			},
		},
	}),

	__newindex = c.StaticMethod({
		Display = {
			Description = "Sets properties.",
		},
		ImplementManual = {
			Signature = "(never) -> never",
			Permits = {
				Call = { "Basic" },
				CallParallel = { "Basic" },
			},
		},
	}),

	new = c.StaticMethod({
		Display = {
			Description = "",
		},
		ImplementMethod = {
			Permits = { Call = { "Basic" }, CallParallel = { "Basic" } },
			Parameters = { c.Parameter("className", "Name of the instance class to construct", t.String) },
			ReturnType = t.Instance(),
		},
	}),

	EntityId = c.Property({
		Type = t.EntityHandle,
		UnmodifiedValue = "{}",
		ImplementProtectedMember = true,
		Permits = { Read = { "Basic" }, ReadParallel = { "Basic" }, Write = { "Basic" }, WriteParallel = { "Basic" } },
		Display = {
			Description = "Internal handle to entity representing instance."
		},
	})
})
