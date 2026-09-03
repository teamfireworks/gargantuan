-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const classes = require("./classes")

const function collectForwardDeclarations(members: classes.Members)
	const statements: { [string]: true } = {}

	for _, member in members do
		if member.Kind == "ClassProperty" or member.Kind == "ClassConstant" then
			if member.Type.CForwardDeclaration then
				statements[member.Type.CForwardDeclaration] = true
			end
		elseif member.Kind == "ClassSignal" then
			-- TODO: add signal type forward statement
			for _, parameter in member.Parameters do
				if parameter.Type.CForwardDeclaration then
					statements[parameter.Type.CForwardDeclaration] = true
				end
			end
		elseif member.Kind == "ClassMethod" or member.Kind == "ClassStaticMethod" then
			if member.ImplementManual then
				statements["struct lua_State"] = true
			elseif member.ImplementMethod then
				for _, parameter in member.ImplementMethod.Parameters do
					if parameter.Type.CForwardDeclaration then
						statements[parameter.Type.CForwardDeclaration] = true
					end
				end
				if member.ImplementMethod.ReturnType.CForwardDeclaration then
					statements[member.ImplementMethod.ReturnType.CForwardDeclaration] = true
				end
			end
		end
	end
	return statements
end

return collectForwardDeclarations
