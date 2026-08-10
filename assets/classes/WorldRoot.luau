const c, t, p = require("@tools/classes").unpack()

return c({
	name = "WorldRoot",
	description = "Abstract class for a physical 3D world.",
	abstract = true,
	since = "0.1.0",

	properties = {
		Gravity = {
			description = "Determines the acceleration due to gravity applied to falling BaseParts.",
			type = t.Float,
			unmodified = "196.2f",
		},
	},

	methods = {
		StepPhysics = {
			description = "Advances the physics simulation by the specified delta time. Defaults to simulating all descendant BaseParts.",
			params = { p("deltaTime", t.Double), p("instances", t.Optional(t.Array(t.Instance()))) },
		},
	},
})
