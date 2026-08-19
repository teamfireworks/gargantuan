const declare = require("@tools/declare")

return declare.datatype({
	name = "BaseSignal",
	robloxName = "RBXScriptSignal",

	description = "",
	since = "0.1.0",

	members = {
		Connect = declare.property({
			description = "",
			withMember = { type = declare.types.Bool, unmodified = "true" },
		}),
		Disconnect = declare.method({
			description = "",
			withMember = {},
		}),
	},
})
