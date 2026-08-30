-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const classes = require("./classes")
const types = require("./types")

const function pushType(type: types.Type, paths: { [string]: true })
	if type.CIncludes then
		for _, include in type.CIncludes do
			paths[include] = true
		end
	end
end

const function collectIncludePaths(members: classes.Members)
	const paths: { [string]: true } = {}

	for _, member in members do
		if member.Kind == "ClassProperty" or member.Kind == "ClassConstant" then
			pushType(member.Type, paths)
		elseif member.Kind == "ClassMethod" or member.Kind == "ClassStaticMethod" then
			if member.ImplementManual then
				paths["<lua.h>"] = true
			elseif member.ImplementMethod then
				pushType(member.ImplementMethod.ReturnType, paths)
				for _, parameter in member.ImplementMethod.Parameters do
					pushType(parameter.Type, paths)
				end
			end
		elseif member.Kind == "ClassSignal" then
			-- TODO: define Signal type in types.luau and add it here
			for _, parameter in member.Parameters do
				pushType(parameter.Type, paths)
			end
		end
	end
	return paths
end

return collectIncludePaths
