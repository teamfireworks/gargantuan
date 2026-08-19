local args = require("@std/args")
local configuration = require("@tools/configuration")
local fs = require("@std/fs")

const _parsed = args.parse("clean", "cleans up the build directory for a reconfiguration"):simple()

do
	if fs.is(configuration.buildDirectory) == "Directory" then
		fs.removetree(configuration.buildDirectory)
	end

	for _, target in configuration.classgenTargets do
		if fs.is(target.headerOutput) then
			fs.removetree(target.headerOutput)
		end

		if fs.is(target.implOutput) then
			fs.removetree(target.implOutput)
		end
	end
end
