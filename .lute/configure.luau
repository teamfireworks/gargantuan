-- Copyright (c) 2026 Team Fireworks Games
-- Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
--   https://gargantuan.teamfireworks.org/
--
-- This Source Code Form is subject to the terms of the Mozilla Public License,
-- v. 2.0. If a copy of the MPL was not distributed with this file, You can
-- obtain one at:
--   http://mozilla.org/MPL/2.0/

const cli = require("@batteries/cli")
const process = require("@std/process")
const useConfiguration = require("@lutelibs/useConfiguration")

const args = cli.parser()
args:add("help", "flag", { help = "show this message" })
args:add("type", "option", { help = "build type to use, either 'Debug' or 'Release'", default = "Debug" })
args:add("generator", "option", { help = "generator to use, either 'Ninja' or 'MinGW", default = "Ninja" })
args:add("tracy", "flag", { help = "enable tracy profiler tools" })
args:add("noccache", "flag", { help = "disable CCache compiler launcher" })
args:parse({ ... })

do
	if args:has("help") then
		print(args:usage("configure"))
		print(args:help())
		process.exit(0)
	end

	const configuration = useConfiguration()
	const cmakeArguments = { "cmake", `-B={configuration.gargantuan.buildDir}`, "-S=." }

	const generator = args:get("generator")
	if generator == "Ninja" then
		table.insert(cmakeArguments, "-G=Ninja")
	elseif generator == "MinGW" then
		table.insert(cmakeArguments, "-G='MinGW Makefiles'")
		table.insert(cmakeArguments, "-DCMAKE_PREFIX_PATH=C:/msys64/mingw64")
	else
		error(`unknown generator named '{generator}'`)
	end

	table.insert(cmakeArguments, `-DCMAKE_BUILD_TYPE={args:get("type")}`)
	table.insert(cmakeArguments, `-DGARGANTUAN_TRACY_TOOLS={args:has("tracy") and "ON" or "OFF"}`)

	if not args:has("noccache") then
		table.insert(cmakeArguments, "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache")
	end

	const cmake = process.run(cmakeArguments, { stdio = "inherit" })
	process.exit(cmake.ok and 0 or 1)
end
