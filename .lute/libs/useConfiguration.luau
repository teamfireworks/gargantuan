local fs = require("@std/fs")
local luau = require("@std/luau")
local path = require("@std/path")
local process = require("@std/process")
local time = require("@std/time")

export type LuauConfiguration = {
	read languagemode: "strict" | "nonstrict" | "nocheck" | nil,
	read lint: { [string]: boolean }?,
	read aliases: { [string]: string }?,
}

export type GargantuanCodegenTarget = {
	read schemaDir: string,
	read outputDir: string,
	read registryPath: string,
}

export type GargantuanConfiguration = {
	read rootDir: boolean?,
	read buildDir: string,
	read codegenTargets: {
		read datatypes: GargantuanCodegenTarget,
		read instances: GargantuanCodegenTarget,
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
