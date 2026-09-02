-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const classes = require("@self/classes")
const types = require("@self/types")

const lib = {}

function lib.InstanceClass(
	name: string,
	prelude: classes.InstanceClass,
	members: classes.Members
): classes.InstanceClassSchema
	return {
		Kind = "InstanceClass",
		Name = name,
		Prelude = prelude,
		Members = members,
	}
end

function lib.Userdata(name: string, prelude: classes.Userdata, members: classes.Members): classes.UserdataSchema
	return {
		Kind = "Userdata",
		Name = name,
		Prelude = prelude,
		Members = members,
	}
end

function lib.Unpack()
	return lib, classes, types
end

return table.freeze(lib)
