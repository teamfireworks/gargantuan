const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Attachment",
	description = "Defines a point and orientation relative to an ancestor PVInstance, Bone, or another Attachment.",
	since = "0.1.0",

	properties = {
		CFrame = {
			description = "Relative transform of this attachment.",
			type = t.CFrame,
			unmodified = "gargantuan::CFrame()",
			serializable = true,
		},
	},
})
