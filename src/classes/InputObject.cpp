#include "gargantuan/classes/InputObject.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <memory>
#include <unordered_map>

namespace gargantuan {
	const std::unordered_map<SDL_Keycode, Enums::KeyCode> SDL_TO_KEYCODE = {
		{SDLK_BACKSPACE, Enums::KeyCode::Backspace},
		{SDLK_TAB, Enums::KeyCode::Tab},
		{SDLK_RETURN, Enums::KeyCode::Return},
		{SDLK_RETURN2, Enums::KeyCode::Return},
		{SDLK_PAUSE, Enums::KeyCode::Pause},
		{SDLK_ESCAPE, Enums::KeyCode::Escape},
		{SDLK_SPACE, Enums::KeyCode::Space},
		{SDLK_DBLAPOSTROPHE, Enums::KeyCode::QuotedDouble},
		{SDLK_HASH, Enums::KeyCode::Hash},
		{SDLK_DOLLAR, Enums::KeyCode::Dollar},
		{SDLK_PERCENT, Enums::KeyCode::Percent},
		{SDLK_AMPERSAND, Enums::KeyCode::Ampersand},
		{SDLK_APOSTROPHE, Enums::KeyCode::Quote},
		{SDLK_LEFTPAREN, Enums::KeyCode::LeftParenthesis},
		{SDLK_RIGHTPAREN, Enums::KeyCode::RightParenthesis},
		{SDLK_ASTERISK, Enums::KeyCode::Asterisk},
		{SDLK_PLUS, Enums::KeyCode::Plus},
		{SDLK_COMMA, Enums::KeyCode::Comma},
		{SDLK_MINUS, Enums::KeyCode::Minus},
		{SDLK_PERIOD, Enums::KeyCode::Period},
		{SDLK_SLASH, Enums::KeyCode::Slash},

		{SDLK_0, Enums::KeyCode::Zero},
		{SDLK_1, Enums::KeyCode::One},
		{SDLK_2, Enums::KeyCode::Two},
		{SDLK_3, Enums::KeyCode::Three},
		{SDLK_4, Enums::KeyCode::Four},
		{SDLK_5, Enums::KeyCode::Five},
		{SDLK_6, Enums::KeyCode::Six},
		{SDLK_7, Enums::KeyCode::Seven},
		{SDLK_8, Enums::KeyCode::Eight},
		{SDLK_9, Enums::KeyCode::Nine},

		{SDLK_COLON, Enums::KeyCode::Colon},
		{SDLK_SEMICOLON, Enums::KeyCode::Semicolon},
		{SDLK_LESS, Enums::KeyCode::LessThan},
		{SDLK_EQUALS, Enums::KeyCode::Equals},
		{SDLK_GREATER, Enums::KeyCode::GreaterThan},
		{SDLK_QUESTION, Enums::KeyCode::Question},
		{SDLK_AT, Enums::KeyCode::At},
		{SDLK_LEFTBRACKET, Enums::KeyCode::LeftBracket},
		{SDLK_BACKSLASH, Enums::KeyCode::BackSlash},
		{SDLK_RIGHTBRACKET, Enums::KeyCode::RightBracket},
		{SDLK_CARET, Enums::KeyCode::Caret},
		{SDLK_UNDERSCORE, Enums::KeyCode::Underscore},
		{SDLK_GRAVE, Enums::KeyCode::Backquote},

		{SDLK_A, Enums::KeyCode::A},
		{SDLK_B, Enums::KeyCode::B},
		{SDLK_C, Enums::KeyCode::C},
		{SDLK_D, Enums::KeyCode::D},
		{SDLK_E, Enums::KeyCode::E},
		{SDLK_F, Enums::KeyCode::F},
		{SDLK_G, Enums::KeyCode::G},
		{SDLK_H, Enums::KeyCode::H},
		{SDLK_I, Enums::KeyCode::I},
		{SDLK_J, Enums::KeyCode::J},
		{SDLK_K, Enums::KeyCode::K},
		{SDLK_L, Enums::KeyCode::L},
		{SDLK_M, Enums::KeyCode::M},
		{SDLK_N, Enums::KeyCode::N},
		{SDLK_O, Enums::KeyCode::O},
		{SDLK_P, Enums::KeyCode::P},
		{SDLK_Q, Enums::KeyCode::Q},
		{SDLK_R, Enums::KeyCode::R},
		{SDLK_S, Enums::KeyCode::S},
		{SDLK_T, Enums::KeyCode::T},
		{SDLK_U, Enums::KeyCode::U},
		{SDLK_V, Enums::KeyCode::V},
		{SDLK_W, Enums::KeyCode::W},
		{SDLK_X, Enums::KeyCode::X},
		{SDLK_Y, Enums::KeyCode::Y},
		{SDLK_Z, Enums::KeyCode::Z},

		{SDLK_LEFTBRACE, Enums::KeyCode::LeftCurly},
		{SDLK_PIPE, Enums::KeyCode::Pipe},
		{SDLK_RIGHTBRACE, Enums::KeyCode::RightCurly},
		{SDLK_TILDE, Enums::KeyCode::Tilde},
		{SDLK_DELETE, Enums::KeyCode::Delete},

		{SDLK_KP_0, Enums::KeyCode::KeypadZero},
		{SDLK_KP_1, Enums::KeyCode::KeypadOne},
		{SDLK_KP_2, Enums::KeyCode::KeypadTwo},
		{SDLK_KP_3, Enums::KeyCode::KeypadThree},
		{SDLK_KP_4, Enums::KeyCode::KeypadFour},
		{SDLK_KP_5, Enums::KeyCode::KeypadFive},
		{SDLK_KP_6, Enums::KeyCode::KeypadSix},
		{SDLK_KP_7, Enums::KeyCode::KeypadSeven},
		{SDLK_KP_8, Enums::KeyCode::KeypadEight},
		{SDLK_KP_9, Enums::KeyCode::KeypadNine},
		{SDLK_KP_PERIOD, Enums::KeyCode::KeypadPeriod},
		{SDLK_KP_DIVIDE, Enums::KeyCode::KeypadDivide},
		{SDLK_KP_MULTIPLY, Enums::KeyCode::KeypadMultiply},
		{SDLK_KP_MINUS, Enums::KeyCode::KeypadMinus},
		{SDLK_KP_PLUS, Enums::KeyCode::KeypadPlus},
		{SDLK_KP_ENTER, Enums::KeyCode::KeypadEnter},
		{SDLK_KP_EQUALS, Enums::KeyCode::KeypadEquals},

		{SDLK_UP, Enums::KeyCode::Up},
		{SDLK_DOWN, Enums::KeyCode::Down},
		{SDLK_RIGHT, Enums::KeyCode::Right},
		{SDLK_LEFT, Enums::KeyCode::Left},
		{SDLK_INSERT, Enums::KeyCode::Insert},
		{SDLK_HOME, Enums::KeyCode::Home},
		{SDLK_END, Enums::KeyCode::End},
		{SDLK_PAGEUP, Enums::KeyCode::PageUp},
		{SDLK_PAGEDOWN, Enums::KeyCode::PageDown},

		{SDLK_F1, Enums::KeyCode::F1},
		{SDLK_F2, Enums::KeyCode::F2},
		{SDLK_F3, Enums::KeyCode::F3},
		{SDLK_F4, Enums::KeyCode::F4},
		{SDLK_F5, Enums::KeyCode::F5},
		{SDLK_F6, Enums::KeyCode::F6},
		{SDLK_F7, Enums::KeyCode::F7},
		{SDLK_F8, Enums::KeyCode::F8},
		{SDLK_F9, Enums::KeyCode::F9},
		{SDLK_F10, Enums::KeyCode::F10},
		{SDLK_F11, Enums::KeyCode::F11},
		{SDLK_F12, Enums::KeyCode::F12},
		{SDLK_F13, Enums::KeyCode::F13},
		{SDLK_F14, Enums::KeyCode::F14},
		{SDLK_F15, Enums::KeyCode::F15},

		{SDLK_NUMLOCKCLEAR, Enums::KeyCode::NumLock},
		{SDLK_CAPSLOCK, Enums::KeyCode::CapsLock},
		{SDLK_SCROLLLOCK, Enums::KeyCode::ScrollLock},
		{SDLK_RSHIFT, Enums::KeyCode::RightShift},
		{SDLK_LSHIFT, Enums::KeyCode::LeftShift},
		{SDLK_RCTRL, Enums::KeyCode::RightControl},
		{SDLK_LCTRL, Enums::KeyCode::LeftControl},
		{SDLK_RALT, Enums::KeyCode::RightAlt},
		{SDLK_LALT, Enums::KeyCode::LeftAlt},
		{SDLK_RMETA, Enums::KeyCode::RightMeta},
		{SDLK_LMETA, Enums::KeyCode::LeftMeta},
		{SDLK_LGUI, Enums::KeyCode::LeftSuper},
		{SDLK_RGUI, Enums::KeyCode::RightSuper},
		{SDLK_MODE, Enums::KeyCode::Mode},
		{SDLK_MULTI_KEY_COMPOSE, Enums::KeyCode::Compose},
		{SDLK_HELP, Enums::KeyCode::Help},
		{SDLK_PRINTSCREEN, Enums::KeyCode::Print},
		{SDLK_SYSREQ, Enums::KeyCode::SysReq},
		{SDLK_PAUSE, Enums::KeyCode::Break},
		{SDLK_MENU, Enums::KeyCode::Menu},
		{SDLK_POWER, Enums::KeyCode::Power},
		{SDLK_CURRENCYUNIT, Enums::KeyCode::Euro},
		{SDLK_UNDO, Enums::KeyCode::Undo},
	};

	const std::unordered_map<Enums::KeyCode, std::unordered_set<SDL_Keycode>> KEYCODE_TO_SDL = []() {
		std::unordered_map<Enums::KeyCode, std::unordered_set<SDL_Keycode>> result;
		for (const auto &[sdlKey, keyCode] : SDL_TO_KEYCODE) {
			result[keyCode].insert(sdlKey);
		}
		return result;
	}();

	const std::unordered_map<Enums::ModifierKey, std::unordered_set<Enums::KeyCode>> MODIFIER_TO_KEYCODE = {
		{Enums::ModifierKey::Ctrl, {Enums::KeyCode::LeftControl, Enums::KeyCode::RightControl}},
		{Enums::ModifierKey::Alt, {Enums::KeyCode::LeftAlt, Enums::KeyCode::RightAlt}},
		{Enums::ModifierKey::Shift, {Enums::KeyCode::LeftShift, Enums::KeyCode::RightShift}},
		{Enums::ModifierKey::Meta, {Enums::KeyCode::LeftMeta, Enums::KeyCode::RightMeta}},
	};

	const std::unordered_map<Uint8, Enums::UserInputType> SDL_TO_USER_INPUT_TYPE = {
		{SDL_BUTTON_LEFT, Enums::UserInputType::MouseButton1},
		{SDL_BUTTON_RIGHT, Enums::UserInputType::MouseButton2},
		{SDL_BUTTON_MIDDLE, Enums::UserInputType::MouseButton3},
	};

	bool InputObject::IsModifierKeyDown(Enums::ModifierKey modifierKey) {
		return MODIFIER_TO_KEYCODE.at(modifierKey).contains(KeyCode);
	}

	std::shared_ptr<InputObject> InputObject::fromEvent(SDL_Event &event) {
		switch (event.type) {

		case SDL_EVENT_KEY_UP:
		case SDL_EVENT_KEY_DOWN: {
			auto keyResult = SDL_TO_KEYCODE.find(event.key.key);
			if (keyResult == SDL_TO_KEYCODE.end()) return nullptr;

			auto self = std::make_shared<InputObject>();
			self->UserInputType = Enums::UserInputType::Keyboard;
			self->UserInputState = event.key.down ? Enums::UserInputState::Begin : Enums::UserInputState::End;
			self->KeyCode = keyResult->second;
			return self;
		}

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP: {
			auto self = std::make_shared<InputObject>();
			self->UserInputType = SDL_TO_USER_INPUT_TYPE.at(event.button.button);
			self->UserInputState = event.button.down ? Enums::UserInputState::Begin : Enums::UserInputState::End;
			self->Position = glm::vec3(event.button.x, event.button.y, 0.0f);
			return self;
		}

		case SDL_EVENT_MOUSE_MOTION: {
			auto self = std::make_shared<InputObject>();
			self->UserInputType = Enums::UserInputType::MouseMovement;
			self->UserInputState = Enums::UserInputState::Change;
			self->Delta = glm::vec3(event.motion.xrel, event.motion.yrel, 0.0f);
			self->Position = glm::vec3(event.motion.x, event.motion.y, 0.0f);
			return self;
		}

		case SDL_EVENT_MOUSE_WHEEL: {
			auto self = std::make_shared<InputObject>();
			self->UserInputType = Enums::UserInputType::MouseWheel;
			self->UserInputState = Enums::UserInputState::Change;
			self->Delta = glm::vec3(event.wheel.x, event.wheel.y, 0.0f);
			self->Position = glm::vec3(event.wheel.mouse_x, event.wheel.mouse_y, 0.0f);
			return self;
		}

		default:
			return nullptr;
		}
	}
}
