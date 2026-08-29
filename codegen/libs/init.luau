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

export type Members = { [string]: classes.ClassMember }
export type Schema<T, K> = {
	Kind: K,
	Prelude: T,
	Members: Members,
}

export type InstanceClass = {
	Name: string,
	Owner: string?,
	Abstract: boolean?,
	Serializable: boolean?,
	Description: string,
	Since: vector,
}

export type InstanceClassSchema = Schema<InstanceClass, "InstanceClass">

export type Datatype = {
	Name: string,
	Description: string,
	Since: vector,
}

export type DatatypeSchema = Schema<Datatype, "Datatype">

const lib = {}

function lib.InstanceClass(prelude: InstanceClass, members: Members): InstanceClassSchema
	return {
		Kind = "InstanceClass",
		Prelude = prelude,
		Members = members,
	}
end

function lib.Datatype(prelude: Datatype, members: Members): DatatypeSchema
	return {
		Kind = "Datatype",
		Prelude = prelude,
		Members = members,
	}
end

function lib.Unpack()
	return lib, classes, types
end

return table.freeze(lib)
