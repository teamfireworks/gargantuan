#include "gargantuan/services/UserInputService.hpp"

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

namespace gargantuan {
	bool IsMouseButtonType(Enums::UserInputType inputType) {
		return inputType == Enums::UserInputType::MouseButton1 || inputType == Enums::UserInputType::MouseButton2 ||
			   inputType == Enums::UserInputType::MouseButton3;
	}

	std::vector<std::shared_ptr<InputObject>> UserInputService::GetKeysPressed() {
		std::vector<std::shared_ptr<InputObject>> result;
		result.reserve(ActiveKeys.size());
		for (const auto &[_, inputObject] : ActiveKeys) {
			result.push_back(inputObject);
		}
		return result;
	}

	Enums::UserInputType UserInputService::GetLastInputType() {
		return LastInputType;
	}

	Vector2 UserInputService::GetMouseDelta() {
		return MouseDelta;
	}

	std::vector<std::shared_ptr<InputObject>> UserInputService::GetMouseButtonsPressed() {
		std::vector<std::shared_ptr<InputObject>> result;
		result.reserve(ActiveMouseButtons.size());
		for (const auto &[_, inputObject] : ActiveMouseButtons) {
			result.push_back(inputObject);
		}
		return result;
	}

	Vector2 UserInputService::GetMouseLocation() {
		return MouseLocation;
	}

	bool UserInputService::IsKeyDown(Enums::KeyCode keyCode) {
		return ActiveKeys.contains(keyCode);
	}

	bool UserInputService::IsMouseButtonPressed(Enums::UserInputType mouseType) {
		return ActiveMouseButtons.contains(mouseType);
	}

	void UserInputService::ProcessEvent(SDL_Event &event) {
		if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED) return WindowFocused->Fire({});
		if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
			ActiveKeys.clear();
			ActiveMouseButtons.clear();
			return WindowFocusReleased->Fire({});
		};

		auto input = InputObject::fromEvent(event);
		if (!input) return;

		auto inputType = input->GetUserInputType();
		auto inputState = input->GetUserInputState();

		if (LastInputType != inputType) {
			LastInputType = inputType;
			LastInputTypeChanged->Fire(inputType);
		}

		if (inputState == Enums::UserInputState::Begin) {
			if (inputType == Enums::UserInputType::Keyboard) {
				if (!ActiveKeys.contains(input->GetKeyCode())) ActiveKeys.emplace(input->GetKeyCode(), input);
				if (input->GetKeyCode() == Enums::KeyCode::Space) JumpRequest->Fire({});
			} else if (IsMouseButtonType(inputType)) {
				if (!ActiveMouseButtons.contains(input->GetUserInputType()))
					ActiveKeys.emplace(input->GetKeyCode(), input);
			}
			InputBegan->Fire({input, false});
		} else if (inputState == Enums::UserInputState::Change) {
			if (inputType == Enums::UserInputType::MouseMovement) {
				MouseDelta = Vector2(input->GetDelta());
				MouseLocation = Vector2(input->GetDelta());
			}
			InputChanged->Fire({input, false});
		} else if (inputState == Enums::UserInputState::End) {
			if (inputType == Enums::UserInputType::Keyboard) {
				if (ActiveKeys.contains(input->GetKeyCode())) ActiveKeys.erase(input->GetKeyCode());
			} else if (IsMouseButtonType(inputType)) {
				if (ActiveMouseButtons.contains(input->GetUserInputType())) ActiveKeys.erase(input->GetKeyCode());
			}
			InputEnded->Fire({input, false});
		}
	}
}
