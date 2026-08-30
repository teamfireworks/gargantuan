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
const process = require("@std/process")
const time = require("@std/time")

export type LuauConfiguration = {
	read languagemode: "strict" | "nonstrict" | "nocheck" | nil,
	read lint: { [string]: boolean }?,
	read aliases: { [string]: string }?,
}

export type CodegenTarget = {
	read schemaDir: string,
	read outputDir: string,
	read registryPath: string,
}

export type CodegenClasses = CodegenTarget & {}

export type CodegenUserdatas = CodegenTarget & {
	read tagEnumPath: string,
}

export type GargantuanConfiguration = {
	read rootDir: boolean?,

	read buildDir: string,
	read licenseHeader: { string },
	read codegenHeader: { string },
	read codegen: {
		read classes: CodegenClasses,
		read userdatas: CodegenUserdatas,
	},
}

export type Configuration = {
	read luau: LuauConfiguration,
	read gargantuan: GargantuanConfiguration,
}

const cache: { [string]: { result: Configuration, lastModified: time.Duration } } = {}

const function useConfiguration(dir: path.Pathlike?)
	local currentDir = path.parse(dir or process.cwd())
	local filepath
	while next(currentDir.parts) do
		filepath = path.join(currentDir, ".config.luau")
		if fs.exists(filepath) then
			break
		end
		table.remove(currentDir.parts)
	end

	assert(filepath, "No .config.luau found in ancestors")

	const formattedFilepath = path.format(filepath)
	const metadata = fs.metadata(filepath)
	if cache[formattedFilepath] and cache[formattedFilepath].lastModified <= metadata.modified then
		return cache[formattedFilepath].result
	end

	const result = luau.loadModule(filepath)
	cache[formattedFilepath] = { result = result, lastModified = metadata.modified }

	return result
end

return useConfiguration
