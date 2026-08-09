const c, t, p = require("@tools/classes").unpack()

return c({
	name = "BasePart",
	description = "Abstract class for workspace objects that physically interact.",
	abstract = true,
	since = "0.1.0",

	properties = {
		Anchored = {
			description = "Whether to not interact with physics",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		CanCollide = {
			description = "Whether other BaseParts can collide with this instance",
			type = t.Bool,
			unmodified = "true",
			serializable = true,
		},
		CastShadow = {
			description = "Whether to cast a shadow from this instance",
			type = t.Bool,
			unmodified = "true",
			serializable = true,
		},
		CFrame = {
			description = "The world transform of this instance",
			type = t.CFrame,
			unmodified = "gargantuan::CFrame()",
			serializable = true,
		},
		Color = {
			description = "The visual color of this instance",
			type = t.Color3,
			unmodified = "gargantuan::Color3(1.0f, 1.0f, 1.0f)",
			serializable = true,
		},
		Size = {
			description = "The world size of this instance",
			type = t.Vector3,
			unmodified = "glm::vec3(1.0f, 1.0f, 1.0f)",
			serializable = true,
		},
		Position = {
			description = "The world position of this instance",
			type = t.Vector3,
			unmodified = "glm::vec3(0.0f, 0.0f, 0.0f)",
			overrideRead = "GetPosition",
			overrideWrite = "SetPosition",
		},
		-- Rotation = {
		-- 	description = "The world rotation of this instance",
		-- 	type = t.Vector3,
		-- 	unmodified = "glm::vec3(0.0f, 0.0f, 0.0f)",
		-- 	overrideRead = "GetRotation",
		-- 	overrideWrite = "SetRotation",
		-- },
		Transparency = {
			description = "The visual transparency of this instance",
			type = t.Float,
			unmodified = "0.0f",
			serializable = true,
		},
	},
})
