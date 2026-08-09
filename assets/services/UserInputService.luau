const c, t, p = require("@tools/classes").unpack()

return c({
	name = "UserInputService",
	description = "Provides user input events, including keyboard, mouse, touch, and gamepads",
	since = "0.1.0",

	properties = {
		MouseBehavior = {
			description = "Behavior mode of the mouse cursor",
			type = t.EnumItem("MouseBehavior"),
			unmodified = "Enums::MouseBehavior::Default",
		},
		MouseIcon = {
			description = "Content identifier for the mouse icon asset",
			type = t.StringView,
			unmodified = '""',
		},
		MouseIconContent = {
			description = "Raw content for the mouse icon",
			type = t.StringView,
			unmodified = '""',
		},
		MouseIconEnabled = {
			description = "Whether the mouse icon is visible",
			type = t.Bool,
			unmodified = "false",
		},
		KeyboardEnabled = {
			description = "Whether a physical keyboard is available",
			type = t.Bool,
			unmodified = "true",
		},
		OnScreenKeyboardVisible = {
			description = "Whether the touch on-screen keyboard is visible",
			type = t.Bool,
			unmodified = "false",
		},
		OnScreenKeyboardPosition = {
			description = "Screen position of the on-screen keyboard",
			type = t.Vector2,
			unmodified = "gargantuan::Vector2(0.0f, 0.0f)",
		},
		OnScreenKeyboardSize = {
			description = "Dimensions of the on-screen keyboard",
			type = t.Vector2,
			unmodified = "gargantuan::Vector2(0.0f, 0.0f)",
		},
		TouchEnabled = {
			description = "Whether touch input events are enabled",
			type = t.Bool,
			unmodified = "false",
		},
		TouchScreenEnabled = {
			description = "Whether a touchscreen interface is detected",
			type = t.Bool,
			unmodified = "false",
		},
		AccelerometerEnabled = {
			description = "Whether device accelerometer reading is active",
			type = t.Bool,
			unmodified = "false",
		},
		GamepadEnabled = {
			description = "Whether gamepad input processing is active",
			type = t.Bool,
			unmodified = "false",
		},
		GyroscopeEnabled = {
			description = "Whether device gyroscope reading is active",
			type = t.Bool,
			unmodified = "false",
		},
	},

	signals = {
		DeviceAccelerationChanged = {
			description = "Fired when the physical acceleration of the device changes",
			args = { t.IsA("InputObject") },
		},
		DeviceGravityChanged = {
			description = "Fired when the gravity vector of the device changes",
			args = { t.IsA("InputObject") },
		},
		DeviceRotationChanged = {
			description = "Fired when the device orientation changes",
			args = { t.IsA("InputObject"), t.CFrame },
		},
		GamepadConnected = {
			description = "Fired when a gamepad is connected",
			args = { t.IsA("InputObject") },
		},
		GamepadDisconnected = {
			description = "Fired when a gamepad is disconnected",
			args = { t.IsA("InputObject") },
		},
		InputBegan = {
			description = "Fired when a user input starts",
			args = { t.IsA("InputObject"), t.Bool },
		},
		InputChanged = {
			description = "Fired when an active user input changes state",
			args = { t.IsA("InputObject"), t.Bool },
		},
		InputEnded = {
			description = "Fired when a user input ends",
			args = { t.IsA("InputObject"), t.Bool },
		},
		JumpRequest = {
			description = "Fired when a jump action is triggered by the user",
		},
		LastInputTypeChanged = {
			description = "Fired when the most recent input device type changes",
			args = { t.EnumItem("UserInputType") },
		},
		PointerAction = {
			description = "Fired for general pointer/mouse actions",
			args = { t.Float, t.Vector2, t.Float, t.Bool },
		},
		TouchStarted = {
			description = "Fired when a touch gesture begins",
			args = { t.IsA("InputObject"), t.Bool },
		},
		TouchEnded = {
			description = "Fired when a touch gesture ends",
			args = { t.IsA("InputObject"), t.Bool },
		},
		TouchDrag = {
			description = "Fired during a touch dragging gesture",
			args = { t.EnumItem("SwipeDirection"), t.Integer, t.Bool },
		},
		TouchLongPress = {
			description = "Fired when a touch long press gesture is registered",
			args = { t.Array(t.Vector2), t.EnumItem("UserInputState"), t.Bool },
		},
		TouchMoved = {
			description = "Fired when an active touch point moves",
			args = { t.IsA("InputObject"), t.Bool },
		},
		TouchPan = {
			description = "Fired during a touch panning gesture",
			args = { t.Array(t.Vector2), t.Vector2, t.Vector2, t.EnumItem("UserInputState"), t.Bool },
		},
		TouchPinch = {
			description = "Fired during a touch pinch gesture",
			args = { t.Array(t.Vector2), t.Float, t.Float, t.EnumItem("UserInputState"), t.Bool },
		},
		TouchRotate = {
			description = "Fired during a touch rotation gesture",
			args = { t.Array(t.Vector2), t.Float, t.Float, t.EnumItem("UserInputState"), t.Bool },
		},
		TouchTap = {
			description = "Fired when a touch tap occurs",
			args = { t.Array(t.Vector2), t.Bool },
		},
		TouchTapInWorld = {
			description = "Fired when a tap occurs in 3D world space",
			args = { t.Vector2, t.Bool },
		},
		WindowFocused = {
			description = "Fired when the application window gains focus",
		},
		WindowFocusReleased = {
			description = "Fired when the application window loses focus",
		},
	},

	methods = {
		-- GamepadSupports = {
		-- 	description = "Returns whether a specific gamepad supports a given key code",
		-- 	params = {
		-- 		p("gamepadType", t.EnumItem("UserInputType")),
		-- 		p("keyCode", t.EnumItem("KeyCode")),
		-- 	},
		-- 	returns = t.Bool,
		-- },
		-- GetConnectedGamepads = {
		-- 	description = "Returns a list of all currently connected gamepads",
		-- 	returns = t.Array(t.EnumItem("UserInputType")),
		-- },
		-- GetDeviceAcceleration = {
		-- 	description = "Returns an InputObject representing device acceleration",
		-- 	returns = t.IsA("InputObject"),
		-- },
		-- GetDeviceGravity = {
		-- 	description = "Returns an InputObject representing the device gravity vector",
		-- 	returns = t.IsA("InputObject"),
		-- },
		-- GetDeviceRotation = {
		-- 	description = "Returns the device rotation input object and orientation CFrame",
		-- 	returns = t.IsA("InputObject"),
		-- },
		-- GetGamepadConnected = {
		-- 	description = "Returns whether a specific gamepad type is connected",
		-- 	params = { p("gamepadType", t.EnumItem("UserInputType")) },
		-- 	returns = t.Bool,
		-- },
		-- GetGamepadState = {
		-- 	description = "Returns a list of input objects representing current gamepad input states",
		-- 	params = { p("num", t.EnumItem("UserInputType")) },
		-- 	returns = t.Array(t.IsA("InputObject")),
		-- },
		-- GetImageForKeyCode = {
		-- 	description = "Returns the content asset ID associated with a key code icon",
		-- 	params = { p("keyCode", t.EnumItem("KeyCode")) },
		-- 	returns = t.StringView,
		-- },
		GetKeysPressed = {
			description = "Returns an array of input objects for all currently pressed keyboard keys",
			returns = t.Array(t.IsA("InputObject")),
		},
		GetLastInputType = {
			description = "Returns the type of the last input event received",
			returns = t.EnumItem("UserInputType"),
		},
		GetMouseButtonsPressed = {
			description = "Returns an array of input objects for all currently pressed mouse buttons",
			returns = t.Array(t.IsA("InputObject")),
		},
		GetMouseDelta = {
			description = "Returns the mouse movement delta vector since the last frame",
			returns = t.Vector2,
		},
		GetMouseLocation = {
			description = "Returns the current position of the mouse cursor",
			returns = t.Vector2,
		},
		-- GetNavigationGamepads = {
		-- 	description = "Returns a list of gamepads used for UI navigation",
		-- 	returns = t.Array(t.EnumItem("UserInputType")),
		-- },
		-- GetStringForKeyCode = {
		-- 	description = "Returns the display string associated with a key code",
		-- 	params = {
		-- 		p("keyCode", t.EnumItem("KeyCode")),
		-- 		p("format", t.EnumItem("KeyCodeStringFormat")),
		-- 	},
		-- 	returns = t.String,
		-- },
		-- GetSupportedGamepadKeyCodes = {
		-- 	description = "Returns all key codes supported by a given gamepad type",
		-- 	params = { p("gamepadType", t.EnumItem("UserInputType")) },
		-- 	returns = t.Array(t.EnumItem("KeyCode")),
		-- },
		-- IsGamepadButtonDown = {
		-- 	description = "Returns whether a gamepad button is currently pressed",
		-- 	params = {
		-- 		p("gamepadType", t.EnumItem("UserInputType")),
		-- 		p("keyCode", t.EnumItem("KeyCode")),
		-- 	},
		-- 	returns = t.Bool,
		-- },
		IsKeyDown = {
			description = "Returns whether a keyboard key is currently pressed",
			params = { p("keyCode", t.EnumItem("KeyCode")) },
			returns = t.Bool,
		},
		IsMouseButtonPressed = {
			description = "Returns whether a specific mouse button is currently pressed",
			params = { p("mouseType", t.EnumItem("UserInputType")) },
			returns = t.Bool,
		},
		-- IsNavigationGamepad = {
		-- 	description = "Returns whether a gamepad is designated as a navigation gamepad",
		-- 	params = { p("gamepadType", t.EnumItem("UserInputType")) },
		-- 	returns = t.Bool,
		-- },
		-- SetNavigationGamepad = {
		-- 	description = "Sets whether a gamepad should be treated as a navigation gamepad",
		-- 	params = {
		-- 		p("gamepadType", t.EnumItem("UserInputType")),
		-- 		p("enabled", t.Bool),
		-- 	},
		-- },
	},
})
