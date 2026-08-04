#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <SDL3/SDL.h>

namespace gargantuan {
	G_ENUM(
		CameraType,
		/// Camera is stationary.
		Fixed,
		/// Camera moves with the subject at a fixed offset and will rotate as the subject rotates.
		Attach,
		/// Camera is stationary but will rotate to keep the subject in the center of the screen.
		Watch,
		/// Camera moves with the subject but does not rotate automatically.
		Track,
		/// Camera moves with the subject and rotates to keep the subject in the center.
		Follow,
		/// Default mode used by Gargantuan.
		Custom,
		/// No default behavior. Used when developers need to script custom behavior.
		Scriptable,
		/// The camera has a fixed Y position, but can be rotated around the player.
		Orbital,
		/// Camera has omnidirectional movement.
		Freecam,
	);

	class Camera : public Instance {
	  public:
		G_INSTANCE_DECL(Camera);

		Enums::CameraType CameraType = Enums::CameraType::Freecam;
		CFrame CFrame;
		float Pitch = 0.0f, Yaw = -90.0f, Roll = 0.0f;
		// Vertical field of view in degrees.
		float FieldOfView = 70.0f;
		Vector2 ViewportSize = gargantuan::Vector2(0.0f, 0.0f);

		float AccumulatedDeltaX = 0.0f;
		float AccumulatedDeltaY = 0.0f;
		float FreecamSpeed = 10.0f;
		float FreecamSensitivity = 0.2f;

		float GetAspectRatio();
		float GetHorizontalFieldOfView();
		float GetDiagonalFieldOfView();
		void SetHorizontalFieldOfView(float fovy);
		void SetDiagonalFieldOfView(float fovy);
		glm::mat4 GetProjectionMatrix();
		glm::mat4 GetViewMatrix();

		void OnEvent(SDL_Event &event);
		void Step(float deltaTime);
	};
} // namespace gargantuan
