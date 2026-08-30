-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const fs = require("@std/fs")
const luau = require("@std/luau")
const path = require("@std/path")
const stringext = require("@std/stringext")
const useConfiguration = require("@gluten/useConfiguration")

const function collectSchemas(target: useConfiguration.CodegenTarget)
	if not fs.exists(target.outputDir) then
		fs.createDirectory(target.outputDir, { makeParents = true })
	end

	const schemas = {}
	for _, entry in fs.listDirectory(target.schemaDir) do
		if entry.type == "file" and stringext.hasSuffix(entry.name, ".luau") then
			schemas[stringext.removeSuffix(entry.name, ".luau")] =
				luau.loadModule(path.join(target.schemaDir, entry.name))
		end
	end
	return schemas
end

return collectSchemas
