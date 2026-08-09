const c, t, p = require("@tools/classes").unpack()

return c({
	name = "GuiObject",
	extends = "GuiBase2d",
	description = "Abstract class for instances that compose 2D graphical user interface",
	abstract = true,
	since = "0.1.0",

	properties = {
		Active = {
			description = "Whether this instance is actively being interacted with",
			type = t.Bool,
			unmodified = "false",
			writeLevel = "Never",
		},
		AnchorPoint = {
			description = "Origin point of this instance relative to it's absolute size",
			type = t.Vector2,
			unmodified = "gargantuan::Vector2(0.0f, 0.0f)",
			serializable = true,
		},
		AutomaticSize = {
			description = "Whether resizing occurs based on child content",
			type = t.EnumItem("AutomaticSize"),
			unmodified = "Enums::AutomaticSize::None",
			serializable = true,
		},
		BackgroundColor3 = {
			description = "Background color of this instance",
			type = t.Color3,
			unmodified = "gargantuan::Color3(1.0f, 1.0f, 1.0f)",
			serializable = true,
		},
		BackgroundTransparency = {
			description = "Background transparency of this instance",
			type = t.Float,
			unmodified = "0.0f",
			serializable = true,
		},
		ClipsDescendants = {
			description = "Whether overflowing descendants are clipped by this instance",
			type = t.Bool,
			unmodified = "true",
			serializable = true,
		},
		GuiState = {
			description = "Whether this instance is being actively pressed",
			type = t.EnumItem("GuiState"),
			unmodified = "Enums::GuiState::NonInteractable",
			writeLevel = "Never",
		},
		InputSink = {
			description = "Whether this instance absorbs input events",
			type = t.EnumItem("InputSink"),
			unmodified = "Enums::InputSink::None",
			serializable = true,
		},
		Interactable = {
			description = "Whether this instance can be interacted with",
			type = t.Bool,
			unmodified = "false",
			serializable = true,
		},
		LayoutOrder = {
			description = "Controls the sort order when used with UI layouts",
			type = t.Integer,
			unmodified = "0",
			serializable = true,
		},
		Position = {
			description = "Relative position of this instance",
			type = t.UDim2,
			unmodified = "gargantuan::UDim2(0.0f, 0, 0.0f, 0)",
			serializable = true,
		},
		Rotation = {
			description = "Relative rotation of this instance",
			type = t.Float,
			unmodified = "0.0f",
			serializable = true,
		},
		Size = {
			description = "Relative size of this instance",
			type = t.UDim2,
			unmodified = "gargantuan::UDim2(0.0f, 0, 0.0f, 0)",
			serializable = true,
		},
		Visible = {
			description = "Whether this instance should be rendered",
			type = t.Bool,
			unmodified = "true",
			serializable = true,
		},
		ZIndex = {
			description = "The order in which this instance renders relative to others",
			type = t.Integer,
			unmodified = "0",
			serializable = true,
		},
	},
})
