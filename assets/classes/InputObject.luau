const c, t, p = require("@tools/classes").unpack()

return c({
	name = "InputObject",
	description = "Class that describes a user input at an instant.",
	since = "0.1.0",

	properties = {
		Delta = {
			description = "Describes the delta between input movements.",
			type = t.Vector3,
			unmodified = "glm::vec3(0.0f, 0.0f, 0.0f)",
		},
		Position = {
			description = "Describes the positional value of this input.",
			type = t.Vector3,
			unmodified = "glm::vec3(0.0f, 0.0f, 0.0f)",
		},
		KeyCode = {
			description = "Describes the kind of input used.",
			type = t.EnumItem("KeyCode"),
			unmodified = "Enums::KeyCode::None",
		},
		UserInputState = {
			description = "Describes the state of an input being performed.",
			type = t.EnumItem("UserInputState"),
			unmodified = "Enums::UserInputState::None",
		},
		UserInputType = {
			description = "Describes the kind of input being performed.",
			type = t.EnumItem("UserInputType"),
			unmodified = "Enums::UserInputType::None",
		},
	},

	methods = {
		IsModifierKeyDown = {
			description = "Whether the specified modifier key is being pressed.",
			params = { p("modifierKey", t.EnumItem("ModifierKey")) },
			returns = t.Bool,
		},
	},
})
