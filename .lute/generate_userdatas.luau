-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

local classes = require("@gluten/codegen/classes")
local cli = require("@batteries/cli")
local collectForwardDeclarations = require("@gluten/codegen/collectForwardDeclarations")
local collectIncludePaths = require("@gluten/codegen/collectIncludePaths")
local collectSchemas = require("@gluten/codegen/collectSchemas")
local formatSchemaIdentifier = require("@gluten/codegen/formatSchemaIdentifier")
local formatUserdataMacros = require("@gluten/codegen/userdatas/formatUserdataMacros")
local fs = require("@std/fs")
local generateLuaValue = require("@gluten/codegen/userdatas/generateLuaValue")
local generateMacroBody = require("@gluten/codegen/generateMacroBody")
local generateMacroHeader = require("@gluten/codegen/generateMacroHeader")
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
		print("Generating", basename)
		tagEnumMembers[schema.Name] = true

		const includePaths = collectIncludePaths(schema.Members)
		const forwardDeclarations = collectForwardDeclarations(schema.Members)

		const macroStart, macroEnd = formatUserdataMacros(schema)
		const macroBody = {}
		table.insert(macroBody, `#define {macroStart} \\`)
		tableext.extend(macroBody, generateMacroBody(schema))
		table.insert(macroBody, "")
		table.insert(macroBody, `#define {macroEnd} \\`)
		tableext.extend(macroBody, generateLuaValue(schema, includePaths))

		const body = generateMacroHeader(configuration)
		if next(includePaths) then
			for includePath in orderedPairs(includePaths) do
				table.insert(body, `#include {includePath} // IWYU pragma: export`)
			end
			table.insert(body, "")
		end
		if next(forwardDeclarations) then
			for declaration in orderedPairs(forwardDeclarations) do
				table.insert(body, declaration .. ";")
			end
			table.insert(body, "")
		end
		tableext.extend(body, macroBody)

		const macroPath = path.join(codegenTarget.outputDir, formatSchemaIdentifier(schema) .. ".hpp")
		fs.writeStringToFile(macroPath, table.concat(body, "\n"))
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
