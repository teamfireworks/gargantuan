const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Script",
	extends = "LuaSourceContainer",
	description = "Class that runs the source code in an asynchronous thread",
	since = "0.1.0",

	properties = {
		Enabled = {
			description = "Whether this script is actively running",
			type = t.Bool,
			unmodified = "true",
			overrideRead = "GetEnabled",
			overrideWrite = "SetEnabled",
			serializable = true,
		},
		RunContext = {
			description = "Whether to run this script as the client or server.",
			type = t.EnumItem("RunContext"),
			unmodified = "Enums::RunContext::None",
			serializable = true,
		},
	},
})
