const args = require("@std/args")
const process = require("@std/process")
local configuration = require("@tools/configuration")

const parsed = args.parse("configure", "prepares the build directory"):simple(
	args.named("--type", "build type to use, either 'Debug' or 'Release'"):default("Debug"),
	args.named("--generator", "generator to use, either 'Ninja' or 'MinGW'"):aliases("-G"):default("Ninja"),
	args.flag("--tracy", "enable tracy profiler tools"),
	args.flag("--no-ccache", "disable CCache compiler launcher")
)

do
	const cmakeArguments = { `-B={configuration.buildDirectory}`, "-S=." }

	const generator = parsed:get("--generator")
	if generator == "Ninja" then
		table.insert(cmakeArguments, "-G=Ninja")
	elseif generator == "MinGW" then
		table.insert(cmakeArguments, "-G='MinGW Makefiles'")
		table.insert(cmakeArguments, "-DCMAKE_PREFIX_PATH=C:/msys64/mingw64")
	else
		error(`Unknown generator named '{generator}'`)
	end

	table.insert(cmakeArguments, `-DCMAKE_BUILD_TYPE={parsed:get("--type")}`)
	table.insert(cmakeArguments, `-DGARGANTUAN_TRACY_TOOLS={parsed.flags.tracy and "ON" or "OFF"}`)

	if not parsed.args["no-ccache"] then
		table.insert(cmakeArguments, "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache")
	end

	const cmake = process.run({ program = "cmake", args = cmakeArguments, stdio = "Inherit" })
	process.exit(cmake.ok and 0 or 1)
end
