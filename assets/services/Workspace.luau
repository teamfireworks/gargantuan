const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Workspace",
	description = "Provides a physical 3D workspace",
	since = "0.1.0",

	properties = {
		CurrentCamera = {
			description = "The current camera.",
			type = t.IsA("Camera"),
			unmodified = "nullptr",
			writeLevel = "Never",
		},
	},
})
