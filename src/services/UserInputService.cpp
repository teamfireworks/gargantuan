#include "gargantuan/services/UserInputService.hpp"
#include "gargantuan/classes/InputObject.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <memory>
#include <vector>

namespace gargantuan {
	I_IMPL(
		UserInputService,
		.Properties =
			{
				{"MouseBehavior", Property::fromMember<&UserInputService::MouseBehavior>(true, false)},
				{"MouseIcon", Property::fromMember<&UserInputService::MouseIcon>(true, false)},
				{"MouseIconContent", Property::fromMember<&UserInputService::MouseIconContent>(true, false)},
				{"MouseIconEnabled", Property::fromMember<&UserInputService::MouseIconEnabled>(true, false)},

				{"KeyboardEnabled", Property::fromMember<&UserInputService::KeyboardEnabled>(true, false)},
				{"OnScreenKeyboardVisible",
				 Property::fromMember<&UserInputService::OnScreenKeyboardVisible>(true, false)},
				{"OnScreenKeyboardPosition",
				 Property::fromMember<&UserInputService::OnScreenKeyboardPosition>(true, false)},
				{"OnScreenKeyboardSize", Property::fromMember<&UserInputService::OnScreenKeyboardSize>(true, false)},

				{"TouchEnabled", Property::fromMember<&UserInputService::TouchEnabled>(true, false)},
				{"TouchScreenEnabled", Property::fromMember<&UserInputService::TouchScreenEnabled>(true, false)},

				{"AccelerometerEnabled", Property::fromMember<&UserInputService::AccelerometerEnabled>(true, false)},
				{"GamepadEnabled", Property::fromMember<&UserInputService::GamepadEnabled>(true, false)},
				{"GyroscopeEnabled", Property::fromMember<&UserInputService::GyroscopeEnabled>(true, false)},

				{"DeviceAccelerationChanged",
				 Property::fromMember<&UserInputService::DeviceAccelerationChanged>(true, false)},
				{"DeviceGravityChanged", Property::fromMember<&UserInputService::DeviceGravityChanged>(true, false)},
				{"DeviceRotationChanged", Property::fromMember<&UserInputService::DeviceRotationChanged>(true, false)},

				{"GamepadConnected", Property::fromMember<&UserInputService::GamepadConnected>(true, false)},
				{"GamepadDisconnected", Property::fromMember<&UserInputService::GamepadDisconnected>(true, false)},

				{"InputBegan", Property::fromMember<&UserInputService::InputBegan>(true, false)},
				{"InputChanged", Property::fromMember<&UserInputService::InputChanged>(true, false)},
				{"InputEnded", Property::fromMember<&UserInputService::InputEnded>(true, false)},

				{"JumpRequest", Property::fromMember<&UserInputService::JumpRequest>(true, false)},

				{"LastInputTypeChanged", Property::fromMember<&UserInputService::LastInputTypeChanged>(true, false)},
				{"PointerAction", Property::fromMember<&UserInputService::PointerAction>(true, false)},

				{"TouchStarted", Property::fromMember<&UserInputService::TouchStarted>(true, false)},
				{"TouchEnded", Property::fromMember<&UserInputService::TouchEnded>(true, false)},
				{"TouchDrag", Property::fromMember<&UserInputService::TouchDrag>(true, false)},
				{"TouchLongPress", Property::fromMember<&UserInputService::TouchLongPress>(true, false)},
				{"TouchMoved", Property::fromMember<&UserInputService::TouchMoved>(true, false)},
				{"TouchPan", Property::fromMember<&UserInputService::TouchPan>(true, false)},
				{"TouchPinch", Property::fromMember<&UserInputService::TouchPinch>(true, false)},
				{"TouchRotate", Property::fromMember<&UserInputService::TouchRotate>(true, false)},
				{"TouchTap", Property::fromMember<&UserInputService::TouchTap>(true, false)},
				{"TouchTapInWorld", Property::fromMember<&UserInputService::TouchTapInWorld>(true, false)},

				{"WindowFocused", Property::fromMember<&UserInputService::WindowFocused>(true, false)},
				{"WindowFocusReleased", Property::fromMember<&UserInputService::WindowFocusReleased>(true, false)},
			},
		.Methods = {
			// G_UD_METHOD(UserInputService, GamepadSupports),
			// G_UD_METHOD(UserInputService, GetConnectedGamepads),
			// G_UD_METHOD(UserInputService, GetDeviceAcceleration),
			// G_UD_METHOD(UserInputService, GetDeviceGravity),
			// G_UD_METHOD(UserInputService, GetDeviceRotation),
			// G_UD_METHOD(UserInputService, GetGamepadConnected),
			// G_UD_METHOD(UserInputService, GetGamepadState),
			// G_UD_METHOD(UserInputService, GetImageForKeyCode),
			{"GetKeysPressed", Method::fromMember<&UserInputService::GetKeysPressed>()},
			{"GetLastInputType", Method::fromMember<&UserInputService::GetLastInputType>()},
			{"GetMouseButtonsPressed", Method::fromMember<&UserInputService::GetMouseButtonsPressed>()},
			{"GetMouseDelta", Method::fromMember<&UserInputService::GetMouseDelta>()},
			{"GetMouseLocation", Method::fromMember<&UserInputService::GetMouseLocation>()},
			// G_UD_METHOD(UserInputService, GetNavigationGamepads),
			// G_UD_METHOD(UserInputService, GetStringForKeyCode),
			// G_UD_METHOD(UserInputService, GetSupportedGamepadKeyCodes),
			// G_UD_METHOD(UserInputService, IsGamepadButtonDown),
			{"IsKeyDown", Method::fromMember<&UserInputService::IsKeyDown>()},
			{"IsMouseButtonPressed", Method::fromMember<&UserInputService::IsMouseButtonPressed>()},
			// G_UD_METHOD(UserInputService, IsNavigationGamepad),
			// G_UD_METHOD(UserInputService, SetNavigationGamepad),
		}
	);

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

		auto inputType = input->UserInputType;
		auto inputState = input->UserInputState;

		if (LastInputType != inputType) {
			LastInputType = inputType;
			LastInputTypeChanged->Fire(inputType);
		}

		if (inputState == Enums::UserInputState::Begin) {
			if (inputType == Enums::UserInputType::Keyboard) {
				if (!ActiveKeys.contains(input->KeyCode)) ActiveKeys.emplace(input->KeyCode, input);
				if (input->KeyCode == Enums::KeyCode::Space) JumpRequest->Fire({});
			} else if (IsMouseButtonType(inputType)) {
				if (!ActiveMouseButtons.contains(input->UserInputType)) ActiveKeys.emplace(input->KeyCode, input);
			}
			InputBegan->Fire({input, false});
		} else if (inputState == Enums::UserInputState::Change) {
			if (inputType == Enums::UserInputType::MouseMovement) {
				MouseDelta = Vector2(input->Delta);
				MouseLocation = Vector2(input->Delta);
			}
			InputChanged->Fire({input, false});
		} else if (inputState == Enums::UserInputState::End) {
			if (inputType == Enums::UserInputType::Keyboard) {
				if (ActiveKeys.contains(input->KeyCode)) ActiveKeys.erase(input->KeyCode);
			} else if (IsMouseButtonType(inputType)) {
				if (ActiveMouseButtons.contains(input->UserInputType)) ActiveKeys.erase(input->KeyCode);
			}
			InputEnded->Fire({input, false});
		}
	}
}
