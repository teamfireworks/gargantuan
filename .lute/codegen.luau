-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const cli = require("@batteries/cli")
const fs = require("@std/fs")
const path = require("@std/path")
const task = require("@std/task")
const useConfiguration = require("@lutelibs/useConfiguration")
const useNumCpus = require("@lutelibs/useNumCpus")
const vm = require("@lute/vm")
local codegenDatatype = require("@lutelibs/codegenDatatype")
local luau = require("@std/luau")
const stringext = require("@std/stringext")

const args = cli.parser()
do
	args:add("script", "positional", { help = "this script", hidden = true })
	args:add("schemas", "variadic", { help = "which schemas to generate, left unspecified regenerates all schemas" })
	args:add("help", "flag", { help = "whether to show this message", aliases = { "h" } })
	-- args:add(
	-- 	"workers",
	-- 	"option",
	-- 	{ help = "amount of worker threads to spawn, defaults to number of logical cores on this system" }
	-- )

	args:add("force", "flag", { help = "force regeneration regardless if schemas are fresh" })
	args:add("noinstances", "flag", { help = "skip regenerating instance classes" })
	args:add("nodatatypes", "flag", { help = "skip regenerating datatypes" })

	args:add("nomacros", "flag", { help = "skip regenerating header macros" })
	args:add("noregistry", "flag", { help = "skip regenerating registry modules" })
	args:add("nolsp", "flag", { help = "skip regenerating luau-lsp declarations" })

	args:parse({ ... })
	if args:has("help") then
		print("Gargantuan's procedural code generator from schemas specified in the root .config.luau")
		print(args:usage("codegen"))
		print()
		print(args:help())
	end
end

do
	const configuration = useConfiguration()
	const codegenTargets = configuration.gargantuan.codegenTargets

	const function generateTarget(target: useConfiguration.GargantuanCodegenTarget)
		if not fs.exists(target.outputDir) then
			fs.createDirectory(target.outputDir, { makeParents = true })
		end

		for _, entry in fs.listDirectory(target.schemaDir) do
			if entry.type == "file" and stringext.hasSuffix(entry.name, ".luau") then
				const basename = stringext.removeSuffix(entry.name, ".luau")
				const schemaPath = path.join(target.schemaDir, entry.name)
				const outputPath = path.join(target.outputDir, basename .. ".hpp")
				const schema = luau.loadModule(schemaPath)
				codegenDatatype({
					schema = schema,
					schemaPath = schemaPath,
					outputPath = outputPath,
				})
			end
		end
	end

	if not args:has("nodatatypes") then
		generateTarget(codegenTargets.datatypes)
	end
end

-- hi im working on gargantuan codegen
-- for data types rn
-- check #engine-dev <----------------------------------------------------------
