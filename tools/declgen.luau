const args = require("@std/args")
local configuration = require("@tools/configuration")
local thread = require("@std/thread")

type Target = {
	inputDir: string,
	macroOutputDir: string,
	registerOutput: string,
}

const parsed = args.parse("declgen", "procedurally generate Gargantuan source files from declarations"):simple(
	-- inputs
	args.flag("--noclasses", "skip regeneration of classes"),
	args.flag("--nodatatypes", "skip regeneration of datatypes"),
	-- outputs
	args.flag("--nolsp", "skip regeneration of Luau LSP declarations"),
	args.flag("--nomacro", "skip regeneration of macro headerfiles"),
	args.flag("--noregister", "skip regeneration of registration module"),
	args.flag("--noprune", "keep generated files without matching declarations"),
	args.flag("--force", "force regeneration regardless of stale declarations")
)

const function codegen(target: Target) end

if not parsed.flags.noclasses then
	codegen(configuration.declgen.datamodel)
end

if not parsed.flags.nodatatypes then
	codegen(configuration.declgen.datatypes)
end
