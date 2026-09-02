-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

local appendLuaValue = require("@gluten/codegen/appendLuaValue")
local classes = require("@gluten/codegen/classes")
local cli = require("@batteries/cli")
local collectForwardDeclarations = require("@gluten/codegen/collectForwardDeclarations")
local collectIncludePaths = require("@gluten/codegen/collectIncludePaths")
local collectSchemas = require("@gluten/codegen/collectSchemas")
local fs = require("@std/fs")
local generateMacroBody = require("@gluten/codegen/generateMacroBody")
local orderedPairs = require("@gluten/utils/orderedPairs")
local path = require("@std/path")
local prefixLines = require("./gluten/prefixLines")
local process = require("@std/process")
local tableext = require("@std/tableext")
local useConfiguration = require("@gluten/useConfiguration")

const args = cli.parser()
args:parse({ ... })

do
	const configuration = useConfiguration()
	const codegenTarget = configuration.gargantuan.codegen.userdatas
	const schemas = collectSchemas(codegenTarget)
	const tagEnumMembers: { [string]: true } = {}

	for basename, schema: classes.UserdataSchema in schemas do
		tagEnumMembers[schema.Name] = true

		const macroLines = {}
		tableext.extend(macroLines, prefixLines("//", configuration.gargantuan.licenseHeader))
		table.insert(macroLines, "")
		tableext.extend(macroLines, prefixLines("//", configuration.gargantuan.codegenHeader))
		table.insert(macroLines, "")
		table.insert(macroLines, "#pragma once")

		const includePaths = collectIncludePaths(schema.Members)
		includePaths["<core/reflection/gg_LuaValue.hpp>"] = true
		if next(includePaths) then
			for includePath in orderedPairs(includePaths) do
				table.insert(macroLines, `#include {includePath} // IWYU pragma: export`)
			end
			table.insert(macroLines, "")
		end

		const forwardDeclarations = collectForwardDeclarations(schema.Members)
		if next(forwardDeclarations) then
			for declaration in orderedPairs(forwardDeclarations) do
				table.insert(macroLines, declaration .. ";")
			end
			table.insert(macroLines, "")
		end

		const macroBody = generateMacroBody(schema.Members)
		table.insert(macroLines, `#define GG_{string.upper(schema.Name)} \\`)
		tableext.extend(macroLines, macroBody)
		table.insert(macroLines, "")

		appendLuaValue(macroLines, schema)

		const macroPath = path.join(codegenTarget.outputDir, basename .. ".hpp")
		fs.writeStringToFile(macroPath, table.concat(macroLines, "\n"))
		process.run({ "clang-format", "-i", path.format(macroPath) })
	end

	do
		const tagEnumLines = {}
		tableext.extend(tagEnumLines, prefixLines("//", configuration.gargantuan.licenseHeader))
		table.insert(tagEnumLines, "")
		tableext.extend(tagEnumLines, prefixLines("//", configuration.gargantuan.codegenHeader))
		table.insert(tagEnumLines, "")
		table.insert(tagEnumLines, "#pragma once")
		table.insert(tagEnumLines, "")
		table.insert(tagEnumLines, "typedef enum {")
		for tagEnumMember in orderedPairs(tagEnumMembers) do
			table.insert(tagEnumLines, "GG_USERDATA_TAG_" .. string.upper(tagEnumMember) .. ",")
		end
		table.insert(tagEnumLines, "} gg_UserdataTag;")
		table.insert(tagEnumLines, "")
		fs.writeStringToFile(codegenTarget.tagEnumPath, table.concat(tagEnumLines, "\n"))
		process.run({ "clang-format", "-i", path.format(codegenTarget.tagEnumPath) })
	end
end
