const c, t, p = require("@tools/classes").unpack()

return c({
	name = "Camera",
	description = "Class that defines the 3D view of a world.",
	since = "0.1.0",

	properties = {
		CameraType = {
			description = "Type of camera input",
			type = t.EnumItem("CameraType"),
			unmodified = "Enums::CameraType::Freecam",
		},
		CFrame = {
			description = "World transform of this instance",
			type = t.CFrame,
			unmodified = "gargantuan::CFrame()",
		},

		ViewportSize = {
			description = "Viewport size in pixels",
			type = t.Vector2,
			unmodified = "gargantuan::Vector2()",
		},
		AspectRatio = {
			description = "Viewport aspect ratio",
			type = t.Float,
			unmodified = "1.0f",
			overrideRead = "GetAspectRatio",
		},

		Pitch = {
			description = "Local pitch of this camera",
			type = t.Float,
			unmodified = "0.0f",
			writeLevel = "Never",
		},
		Yaw = {
			description = "Local yaw of this camera",
			type = t.Float,
			unmodified = "0.0f",
			writeLevel = "Never",
		},
		Roll = {
			description = "Local pitch of this camera",
			type = t.Float,
			unmodified = "0.0f",
			writeLevel = "Never",
		},

		-- TODO: Calculate unmodified values for fovies
		FieldOfView = {
			description = "Vertical field of view in degrees",
			type = t.Float,
			unmodified = "70.0f",
		},
		HorizontalFieldOfView = {
			description = "Horizontal field of view in degrees",
			type = t.Float,
			unmodified = "70.0f",
			overrideRead = "GetHorizontalFieldOfView",
			overrideWrite = "SetHorizontalFieldOfView",
		},
		DiagonalFieldOfView = {
			description = "Horizontal field of view in degrees",
			type = t.Float,
			unmodified = "70.0f",
			overrideRead = "GetDiagonalFieldOfView",
			overrideWrite = "SetDiagonalFieldOfView",
		},
	},
})
