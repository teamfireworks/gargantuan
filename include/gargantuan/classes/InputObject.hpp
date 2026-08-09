#pragma once

#include "gargantuan/classes/generated/InputObject.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <magic_enum/magic_enum.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace gargantuan {
	G_ENUM(
		UserInputState,

		Begin,
		Change,
		End,
		Cancel,
		None
	);

	G_ENUM(
		UserInputType,

		// The left mouse button.
		MouseButton1 = 0,
		// The right mouse button.
		MouseButton2 = 1,
		// The middle mouse button.
		MouseButton3 = 2,
		// The mouse wheel.
		MouseWheel = 3,
		// Fires changed events each time the player's cursor position changes
		// and when the mouse enters/leaves the game window.
		MouseMovement = 4,

		Touch = 7,
		Keyboard = 8,
		Focus = 9,
		Accelerometer = 10,
		Gyro = 11,
		Gamepad1 = 12,
		Gamepad2 = 13,
		Gamepad3 = 14,
		Gamepad4 = 15,
		Gamepad5 = 16,
		Gamepad6 = 17,
		Gamepad7 = 18,
		Gamepad8 = 19,

		TextInput = 20,
		InputMethod = 20,
		None = 23
	);

	G_ENUM(
		KeyCode,

		Backspace = 8,
		Tab = 9,
		Clear = 12,
		Return = 13,
		Pause = 19,
		Escape = 27,
		Space = 32,
		QuotedDouble = 34,
		Hash = 35,
		Dollar = 36,
		Percent = 37,
		Ampersand = 38,
		Quote = 39,
		LeftParenthesis = 40,
		RightParenthesis = 41,
		Asterisk = 42,
		Plus = 43,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,

		Zero = 48,
		One = 49,
		Two = 50,
		Three = 51,
		Four = 52,
		Five = 53,
		Six = 54,
		Seven = 55,
		Eight = 56,
		Nine = 57,

		Colon = 58,
		Semicolon = 59,
		LessThan = 60,
		Equals = 61,
		GreaterThan = 62,
		Question = 63,
		At = 64,
		LeftBracket = 91,
		BackSlash = 92,
		RightBracket = 93,
		Caret = 94,
		Underscore = 95,
		Backquote = 96,

		A = 97,
		B = 98,
		C = 99,
		D = 100,
		E = 101,
		F = 102,
		G = 103,
		H = 104,
		I = 105,
		J = 106,
		K = 107,
		L = 108,
		M = 109,
		N = 110,
		O = 111,
		P = 112,
		Q = 113,
		R = 114,
		S = 115,
		T = 116,
		U = 117,
		V = 118,
		W = 119,
		X = 120,
		Y = 121,
		Z = 122,

		LeftCurly = 123,
		Pipe = 124,
		RightCurly = 125,
		Tilde = 126,
		Delete = 127,

		KeypadZero = 256,
		KeypadOne = 257,
		KeypadTwo = 258,
		KeypadThree = 259,
		KeypadFour = 260,
		KeypadFive = 261,
		KeypadSix = 262,
		KeypadSeven = 263,
		KeypadEight = 264,
		KeypadNine = 265,
		KeypadPeriod = 266,
		KeypadDivide = 267,
		KeypadMultiply = 268,
		KeypadMinus = 269,
		KeypadPlus = 270,
		KeypadEnter = 271,
		KeypadEquals = 272,

		Up = 273,
		Down = 274,
		Right = 275,
		Left = 276,
		Insert = 277,
		Home = 278,
		End = 279,
		PageUp = 280,
		PageDown = 281,

		F1 = 282,
		F2 = 283,
		F3 = 284,
		F4 = 285,
		F5 = 286,
		F6 = 287,
		F7 = 288,
		F8 = 289,
		F9 = 290,
		F10 = 291,
		F11 = 292,
		F12 = 293,
		F13 = 294,
		F14 = 295,
		F15 = 296,

		NumLock = 300,
		CapsLock = 301,
		ScrollLock = 302,
		RightShift = 303,
		LeftShift = 304,
		RightControl = 305,
		LeftControl = 306,
		RightAlt = 307,
		LeftAlt = 308,
		RightMeta = 309,
		LeftMeta = 310,
		LeftSuper = 311,
		RightSuper = 312,
		Mode = 313,
		Compose = 314,
		Help = 315,
		Print = 316,
		SysReq = 317,
		Break = 318,
		Menu = 319,
		Power = 320,
		Euro = 321,
		Undo = 322,

		ButtonX = 1000,
		ButtonY = 1001,
		ButtonA = 1002,
		ButtonB = 1003,
		ButtonR1 = 1004,
		ButtonL1 = 1005,
		ButtonR2 = 1006,
		ButtonR3 = 1007,
		ButtonL3 = 1009,
		ButtonStart = 1010,
		ButtonSelect = 1011,
		DPadLeft = 1012,
		DPadRight = 1013,
		DPadUp = 1014,
		DPadDown = 1015,
		Thumbstick1 = 1016,
		Thumbstick2 = 1017,
		Thumbstick1Up = 1018,
		Thumbstick1Down = 1019,
		Thumbstick1Left = 1020,
		Thumbstick1Right = 1021,
		THumbstick2Up = 1022,
		Thumbstick2Down = 1023,
		Thumbstick2Left = 1024,
		Thumbstick2Right = 1025,
		MouseLeftButton = 1026,
		MouseRightButton = 1027,
		MouseMiddleButton = 1028,

		MousePosition = 1033,
		TouchPosition = 1034,
		MouseWheel = 1035,
		TrackpadPan = 1040,
		TrackpadPinch = 1045,

		MouseDelta = 1048,
		TouchDelta = 1049,
		TouchPinch = 1050,

		None = 2048
	);

	G_ENUM(ModifierKey, Shift, Ctrl, Alt, Meta);

	extern const std::unordered_map<SDL_Keycode, Enums::KeyCode> SDL_TO_KEYCODE;
	extern const std::unordered_map<Enums::KeyCode, std::unordered_set<SDL_Keycode>> KEYCODE_TO_SDL;
	extern const std::unordered_map<Enums::ModifierKey, std::unordered_set<Enums::KeyCode>> MODIFIER_TO_KEYCODE;

	class InputObject : public Instance {
		I_InputObject;

		[[nodiscard]] static std::shared_ptr<InputObject> fromEvent(SDL_Event &event);
	};
}

template <> struct magic_enum::customize::enum_range<gargantuan::Enums::KeyCode> {
	static constexpr int min = 0;
	static constexpr int max = 2048;
};
