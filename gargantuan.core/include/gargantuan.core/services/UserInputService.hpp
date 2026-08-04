#pragma once

#include "gargantuan/classes/InputObject.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace gargantuan {
	G_ENUM(MouseBehavior, Default, LockCenter, LockCurrentPosition);
	G_ENUM(PreferredInput, KeyboardAndMouse, Gamepad, Touch);
	G_ENUM(KeyCodeStringFormat, Default, Abbreviated);
	G_ENUM(SwipeDirection, Right, Left, Up, Down, None);

	typedef std::string_view Content;
	typedef std::string_view ContentId;

	class UserInputService : public Instance {
	  protected:
		std::unordered_map<Enums::KeyCode, std::shared_ptr<InputObject>> ActiveKeys;
		std::unordered_map<Enums::UserInputType, std::shared_ptr<InputObject>> ActiveMouseButtons;
		Enums::UserInputType LastInputType = Enums::UserInputType::None;
		Vector2 MouseLocation;
		Vector2 MouseDelta;

		typedef std::tuple<std::shared_ptr<InputObject>, CFrame> DeviceRotationChangedSignalType;
		typedef std::tuple<std::shared_ptr<InputObject>, bool> InputSignalType;
		typedef std::tuple<float, Vector2, float, bool> PointerActionSignalType;
		typedef std::tuple<Enums::SwipeDirection, int, bool> TouchDragSignalType;
		typedef std::tuple<std::vector<Vector2>, Enums::UserInputState, bool> TouchLongPressSignalType;
		typedef std::tuple<std::vector<Vector2>, Vector2, Vector2, Enums::UserInputState, bool> TouchPanSignalType;
		typedef std::tuple<std::vector<Vector2>, float, float, Enums::UserInputState, bool> TouchSwipeSignalType;
		typedef std::tuple<std::vector<Vector2>, bool> TouchTapSignalType;
		typedef std::tuple<Vector2, bool> TouchTapInWorldSignalType;

	  public:
		G_INSTANCE_DECL(UserInputService);

		Enums::MouseBehavior MouseBehavior;
		ContentId MouseIcon = "";
		Content MouseIconContent = "";
		bool MouseIconEnabled = false;

		bool KeyboardEnabled = true;
		bool OnScreenKeyboardVisible = false;
		Vector2 OnScreenKeyboardPosition = Vector2();
		Vector2 OnScreenKeyboardSize = Vector2();

		bool TouchEnabled = false;
		bool TouchScreenEnabled = false;

		bool AccelerometerEnabled = false;
		bool GamepadEnabled = false;
		bool GyroscopeEnabled = false;

		// std::shared_ptr<VirtualInput> CreateVirtualInput();
		bool GamepadSupports(Enums::UserInputType gamepadType, Enums::KeyCode keyCode);
		std::vector<Enums::UserInputType> GetConnectedGamepads();
		std::shared_ptr<InputObject> GetDeviceAcceleration();
		std::shared_ptr<InputObject> GetDeviceGravity();
		std::tuple<std::shared_ptr<InputObject>, CFrame> GetDeviceRotation();
		// std::shared_ptr<TextBox> GetFocusedTextBox();
		bool GetGamepadConnected(Enums::UserInputType gamepadType);
		std::vector<std::shared_ptr<InputObject>> GetGamepadState(Enums::UserInputType num);
		ContentId GetImageForKeyCode(Enums::KeyCode keyCode);
		std::vector<std::shared_ptr<InputObject>> GetKeysPressed();
		Enums::UserInputType GetLastInputType();
		std::vector<std::shared_ptr<InputObject>> GetMouseButtonsPressed();
		Vector2 GetMouseDelta();
		Vector2 GetMouseLocation();
		std::vector<Enums::UserInputType> GetNavigationGamepads();
		std::string GetStringForKeyCode(Enums::KeyCode keyCode, Enums::KeyCodeStringFormat format);
		std::vector<Enums::KeyCode> GetSupportedGamepadKeyCodes(Enums::UserInputType gamepadType);
		bool IsGamepadButtonDown(Enums::UserInputType gamepadType, Enums::KeyCode keyCode);
		bool IsKeyDown(Enums::KeyCode keyCode);
		bool IsMouseButtonPressed(Enums::UserInputType mouseType);
		bool IsNavigationGamepad(Enums::UserInputType gamepadType);
		void SetNavigationGamepad(Enums::UserInputType gamepadType, bool enabled);

		G_SIGNAL(DeviceAccelerationChanged, std::shared_ptr<InputObject>);
		G_SIGNAL(DeviceGravityChanged, std::shared_ptr<InputObject>);
		G_SIGNAL(DeviceRotationChanged, DeviceRotationChangedSignalType);

		G_SIGNAL(GamepadConnected, std::shared_ptr<InputObject>);
		G_SIGNAL(GamepadDisconnected, std::shared_ptr<InputObject>);

		G_SIGNAL(InputBegan, InputSignalType);
		G_SIGNAL(InputChanged, InputSignalType);
		G_SIGNAL(InputEnded, InputSignalType);

		G_SIGNAL(JumpRequest, std::monostate);

		G_SIGNAL(LastInputTypeChanged, Enums::UserInputType);
		G_SIGNAL(PointerAction, PointerActionSignalType);
		// G_SIGNAL(TextBoxFocused, std::shared_ptr<TextBox>);
		// G_SIGNAL(TextBoxReleased, std::shared_ptr<TextBox>);

		G_SIGNAL(TouchStarted, InputSignalType);
		G_SIGNAL(TouchEnded, InputSignalType);
		G_SIGNAL(TouchDrag, TouchDragSignalType);
		G_SIGNAL(TouchLongPress, TouchLongPressSignalType);
		G_SIGNAL(TouchMoved, InputSignalType);
		G_SIGNAL(TouchPan, TouchPanSignalType);
		G_SIGNAL(TouchPinch, TouchSwipeSignalType);
		G_SIGNAL(TouchRotate, TouchSwipeSignalType);
		G_SIGNAL(TouchTap, TouchTapSignalType);
		G_SIGNAL(TouchTapInWorld, TouchTapInWorldSignalType);

		G_SIGNAL(WindowFocused, std::monostate)
		G_SIGNAL(WindowFocusReleased, std::monostate);

		void ProcessEvent(SDL_Event &event);
	};
}
