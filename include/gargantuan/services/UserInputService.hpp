#pragma once

#include "gargantuan/services/generated/UserInputService.hpp"

#include <memory>
#include <string_view>

namespace gargantuan {
	G_ENUM(MouseBehavior, Default, LockCenter, LockCurrentPosition);
	G_ENUM(PreferredInput, KeyboardAndMouse, Gamepad, Touch);
	G_ENUM(KeyCodeStringFormat, Default, Abbreviated);
	G_ENUM(SwipeDirection, Right, Left, Up, Down, None);

	typedef std::string_view Content;
	typedef std::string_view ContentId;

	class UserInputService : public Instance {
		I_UserInputService;

		void ProcessEvent(SDL_Event &event);

	  protected:
		std::unordered_map<Enums::KeyCode, std::shared_ptr<InputObject>> ActiveKeys;
		std::unordered_map<Enums::UserInputType, std::shared_ptr<InputObject>> ActiveMouseButtons;
		Enums::UserInputType LastInputType = Enums::UserInputType::None;
		Vector2 MouseLocation;
		Vector2 MouseDelta;
	};
}
