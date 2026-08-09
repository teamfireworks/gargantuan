const c, t, p = require("@tools/classes").unpack()

return c({
	name = "ProcessService",
	description = "Provides access to the system process.",
	since = "0.1.0",

	methods = {
		ExitAsync = {
			description = "Exits the system process with the specified exit code. Yields indefinitely.",
			asLuau = true,
			luauSignature = "(self: ProcessService, exitCode: number) -> never",
		},
		WriteToStdout = {
			description = "Writes to the standard output without a new line.",
			asLuau = true,
			luauSignature = "(self: ProcessService, ...string) -> ()",
		},
		FlushStdout = {
			description = "Flushes the standard output.",
		},
	},
})
