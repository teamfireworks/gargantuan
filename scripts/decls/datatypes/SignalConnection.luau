const declare = require("@tools/declare")

return declare.datatype({
	name = "SignalConnection",
	robloxName = "RBXScriptConnection",

	description = "",
	since = "0.1.0",

	members = {
		Connected = declare.property({
			description = "",
			withMember = { type = declare.types.Bool, unmodified = "true" },
		}),
		Disconnect = declare.method({
			description = "",
			withMember = {},
		}),
	},
})
