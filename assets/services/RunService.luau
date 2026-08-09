const c, t, p = require("@tools/classes").unpack()

return c({
	name = "RunService",
	description = "Provides access to the runtime.",
	since = "0.1.0",

	signals = {
		PreSimulation = {
			description = "Fired before simulating WorldRoots. Listeners receive the delta time",
			args = { t.Float },
		},
		PostSimulation = {
			description = "Fired after simulating WorldRoots. Listeners receive the delta time",
			args = { t.Float },
		},
		PreRender = {
			description = "Fired before rendering the scene. Listeners receive the delta time.",
			args = { t.Float },
		},
	},
})
