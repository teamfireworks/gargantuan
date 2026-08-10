#include "gargantuan/classes/Camera.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace gargantuan {
	float Camera::GetAspectRatio() const {
		return ViewportSize.GetY() > 0.0f ? ViewportSize.GetX() / ViewportSize.GetY() : 1.0f;
	}

	float Camera::GetHorizontalFieldOfView() const {
		return glm::degrees(2 * glm::atan(GetAspectRatio() * glm::tan(glm::radians(FieldOfView) / 2)));
	}

	void Camera::SetHorizontalFieldOfView(float fovy) {
		SetFieldOfView(glm::degrees(2 * glm::atan(1 / GetAspectRatio() * glm::tan(glm::radians(fovy) / 2))));
		GetPropertyChangedSignal("HorizontalFieldOfView")->Fire({});
	}

	float Camera::GetDiagonalFieldOfView() const {
		return glm::degrees(
			2 * glm::atan(glm::sqrt(1 + glm::pow(GetAspectRatio(), 2)) * glm::tan(glm::radians(FieldOfView) / 2))
		);
	}

	void Camera::SetDiagonalFieldOfView(float fovy) {
		SetFieldOfView(
			glm::degrees(
				2 * glm::atan(1 / glm::sqrt(1 + glm::pow(GetAspectRatio(), 2)) * glm::tan(glm::radians(fovy) / 2))
			)
		);
		GetPropertyChangedSignal("DiagonalFieldOfView")->Fire({});
	}

	glm::mat4 Camera::GetProjectionMatrix() {
		return glm::perspective(glm::radians(FieldOfView), GetAspectRatio(), 0.1f, 100000.0f);
	}

	glm::mat4 Camera::GetViewMatrix() {
		glm::vec3 position = CFrame.Position;
		return glm::lookAt(position, position + CFrame.GetLookVector(), CFrame.GetUpVector());
	}

	void Camera::OnEvent(SDL_Event &event) {
		if (CameraType != Enums::CameraType::Freecam) {
			return;
		}

		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			int width, height;
			auto window = SDL_GetWindowFromEvent(&event);
			SDL_GetWindowSizeInPixels(window, &width, &height);
			ViewportSize = Vector2(width, height);
		} else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
			auto window = SDL_GetWindowFromEvent(&event);
			SDL_SetWindowRelativeMouseMode(window, true);
		} else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_RIGHT) {
			auto window = SDL_GetWindowFromEvent(&event);
			SDL_SetWindowRelativeMouseMode(window, false);
		} else if (event.type == SDL_EVENT_MOUSE_MOTION &&
				   SDL_GetWindowRelativeMouseMode(SDL_GetWindowFromEvent(&event))) {
			AccumulatedDeltaX += event.motion.xrel;
			AccumulatedDeltaY += event.motion.yrel;
		}
	}

	void Camera::Step(float deltaTime) {
		if (CameraType != Enums::CameraType::Freecam) {
			return;
		}

		if (AccumulatedDeltaX != 0.0f || AccumulatedDeltaY != 0.0f) {
			Yaw -= AccumulatedDeltaX * FreecamSensitivity;

			Pitch -= AccumulatedDeltaY * FreecamSensitivity;
			Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

			AccumulatedDeltaX = 0.0f;
			AccumulatedDeltaY = 0.0f;

			auto rotation = CFrame::fromEulerAnglesYXZ(glm::radians(Pitch), glm::radians(Yaw), 0.0f);
			CFrame = gargantuan::CFrame(CFrame.Position, rotation.Rotation);
		}

		auto keys = SDL_GetKeyboardState(nullptr);

		auto lookVector = CFrame.GetLookVector();
		auto rightVector = CFrame.GetRightVector();
		auto upVector = CFrame.GetUpVector();

		if (keys[SDL_SCANCODE_W]) {
			CFrame.Position += lookVector * FreecamSpeed * deltaTime;
		}

		if (keys[SDL_SCANCODE_S]) {
			CFrame.Position -= lookVector * FreecamSpeed * deltaTime;
		}

		if (keys[SDL_SCANCODE_A]) {
			CFrame.Position -= rightVector * FreecamSpeed * deltaTime;
		}

		if (keys[SDL_SCANCODE_D]) {
			CFrame.Position += rightVector * FreecamSpeed * deltaTime;
		}

		if (keys[SDL_SCANCODE_SPACE]) {
			CFrame.Position += glm::vec3(0, FreecamSpeed * deltaTime, 0);
		}

		// complex and volatile so i can screenshot on macos
		bool shiftPressed = keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
		bool guiPressed = (SDL_GetModState() & SDL_KMOD_GUI) != 0;
		if (shiftPressed && !guiPressed) {
			CFrame.Position -= glm::vec3(0, FreecamSpeed * deltaTime, 0);
		}
	}
}
