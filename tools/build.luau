local args = require("@std/args")
local configuration = require("@tools/configuration")
local fs = require("@std/fs")
local process = require("@std/process")
local thread = require("@std/thread")

const parsed = args.parse("build", "compiles the engine"):simple(
	args.named("--jobs", "number of jobs passed to CMake, defaults to the number of CPUs on this system"):aliases("-j"),
	args.flag("--no-declgen", "skip regenerating files in tools/decls")
)

do
	if fs.is(configuration.buildDirectory) == "NotFound" then
		print("Configuring the build directory...")
		const configureResult =
			process.run({ program = "seal", args = { "scripts/configure.luau" }, stdio = "Inherit" })
		if not configureResult.ok then
			error("Failed to configure build directory")
		end
	end

	-- if not parsed.flags["no-classgen"] then
	-- 	const classgenResult = process.run({ program = "seal", args = { "scripts/classgen.luau" }, stdio = "Inherit" })
	-- 	if not classgenResult.ok then
	-- 		error("Failed to generate instance classes")
	-- 	end
	-- end

	const jobs = tonumber(parsed:get("jobs")) or thread.parallelism()
	const cmakeResult = process.run({
		program = "cmake",
		args = { "--build", configuration.buildDirectory, "-j", tostring(jobs) },
		stdio = "Inherit",
	})

	if not cmakeResult.ok then
		error("Failed to run CMake")
	end

	process.exit(0)
end
