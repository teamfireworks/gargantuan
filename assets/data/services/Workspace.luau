const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "Workspace",
	extends = "WorldRoot", -- uhhhghh
	description = "Provides a physical 3D workspace",
	since = "0.1.0",

	properties = {
		CurrentCamera = {
			description = "The current camera.",
			type = t.Instance("Camera"),
			unmodified = "nullptr",
			writeLevel = "Never",
		},
	},
})
