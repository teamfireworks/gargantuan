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
local stringext = require("@std/stringext")

const args = cli.parser()
do
	args:add("script", "positional", { help = "this script", hidden = true })
	args:add("schemas", "variadic", { help = "which schemas to generate, left unspecified regenerates all schemas" })
	args:add("help", "flag", { help = "whether to show this message", aliases = { "h" } })
	args:add(
		"workers",
		"option",
		{ help = "amount of worker threads to spawn, defaults to number of logical cores on this system" }
	)

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

const configuration = useConfiguration()
const codegenConfiguration = configuration.gargantuan.codegen
local totalThreads, completedThreads = 0, 0

const function generateTarget(numWorkers: number, target: useConfiguration.GargantuanCodegenTarget)
	const inputs = {}

	for _, file in fs.listDirectory(target.schemaDir) do
		if file.type == "file" and stringext.hasSuffix(file.name, ".luau") then
			const basename = stringext.removeSuffix(file.name, ".luau")
			table.insert(inputs, {
				basename = basename,
				contents = fs.readFileToString(path.join(target.schemaDir, file.name)),
				output = path.format(path.join(codegenConfiguration.workerPath, basename .. ".hpp")),
			})
		end
	end

	for index = 1, numWorkers do
		totalThreads += 1
		task.spawn(function()
			const worker = vm.create(codegenConfiguration.workerPath)
			worker[target.workerMethod]({
				registryPath = target.registryPath,
				inputs = inputs,
			})
			completedThreads += 1
			print(`Worker {completedThreads} out of {totalThreads} completed`)
		end)
	end
end

const totalWorkers = args:has("workers") and tonumber(args:get("workers")) or useNumCpus()
const workersPerTarget = if args:has("noinstances") or args:has("nodatatypes")
	then totalWorkers
	else math.ceil(totalWorkers / 2)

if not args:has("noinstances") then
	generateTarget(workersPerTarget, codegenConfiguration.targets.instances)
end

if not args:has("nodatatypes") then
	generateTarget(workersPerTarget, codegenConfiguration.targets.datatypes)
end

repeat
	task.wait(0.1)
until completedThreads == totalThreads
