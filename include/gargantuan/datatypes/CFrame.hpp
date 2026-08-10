#pragma once

#include "gargantuan/reflection/Enums.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <lua.h>
#include <tuple>

namespace gargantuan {
	G_ENUM(RotationOrder, XYZ, XZY, YZX, YXZ, ZXY, ZYX);

	G_USERDATA_DECL(
		CFrame,

		static constexpr float CF_EPSILON = 1e-6;
		static constexpr glm::mat3 DEFAULT_ROTATION =
			glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
		typedef std::
			tuple<double, double, double, double, double, double, double, double, double, double, double, double>
				Components;

		glm::vec3 Position = {0, 0, 0};
		glm::mat3 Rotation = {};

		CFrame();
		CFrame(glm::vec3 position);
		CFrame(float x, float y, float z);
		CFrame(glm::vec3 position, glm::vec3 lookAtTarget);
		CFrame(glm::vec3 position, glm::mat3 rotation);
		CFrame(
			float x,
			float y,
			float z,
			float r00,
			float r01,
			float r02,
			float r10,
			float r11,
			float r12,
			float r20,
			float r21,
			float r22
		);

		static CFrame lookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);
		static CFrame lookAlong(glm::vec3 from, glm::vec3 to, glm::vec3 up);
		static CFrame fromRotationBetweenVectors(glm::vec3 from, glm::vec3 to);
		static CFrame fromEulerAngles(float rx, float ry, float rz, Enums::RotationOrder order);
		static CFrame fromEulerAnglesXYZ(float rx, float ry, float rz);
		static CFrame fromEulerAnglesYXZ(float rx, float ry, float rz);
		static CFrame Angles(float x, float y, float z);
		static CFrame fromAxisAngle(glm::vec3 v, float r);
		static CFrame fromOrientation(float rx, float ry, float rz);
		static CFrame fromMatrix(glm::vec3 position, glm::vec3 right, glm::vec3 up, glm::vec3 look);
		static CFrame fromQuaternion(float x, float y, float z, float w, glm::vec3 position);

		glm::vec3 GetRightVector() const;
		glm::vec3 GetUpVector() const;
		glm::vec3 GetLookVector() const;

		CFrame Inverse() const;
		CFrame Lerp(const CFrame &goal, double alpha) const;
		CFrame Orthonormalize() const;
		CFrame ToWorldSpace(const CFrame &cf) const;
		CFrame ToObjectSpace(const CFrame &cf) const;
		glm::vec3 PointToWorldSpace(const glm::vec3 &point) const;
		glm::vec3 PointToObjectSpace(const glm::vec3 &point) const;
		glm::vec3 VectorToWorldSpace(const glm::vec3 &point) const;
		glm::vec3 VectorToObjectSpace(const glm::vec3 &point) const;
		Components GetComponents() const;
		std::tuple<double, double, double> ToEulerAngles(Enums::RotationOrder order);
		std::tuple<double, double, double> ToEulerAnglesXYZ() const;
		std::tuple<double, double, double> ToEulerAnglesYXZ() const;
		std::tuple<double, double, double> ToOrientation() const;
		std::tuple<glm::vec3, double> ToAxisAngle() const;
		bool FuzzyEq(const CFrame &other, double epsilon = 1e-5) const;
		double AngleBetween(const CFrame &other) const;
		glm::quat ToQuaternion() const;

		static int LAdd(lua_State *L, CFrame *self);
		static int LSub(lua_State *L, CFrame *self);
		static int LMul(lua_State *L, CFrame *self);
		static int LTostring(lua_State *L, CFrame *self);

		static glm::vec3 SafeUnit(glm::vec3 vec, glm::vec3 fallback);
		static glm::mat3 BuildLookRotation(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0, 1, 0});

		glm::vec3 operator*(const glm::vec3 & other) const { return Position + (Rotation * other); };

		CFrame operator*(const CFrame & other) const {
			glm::vec3 transformedPosition = Position + (Rotation * other.Position);
			glm::mat3 transformedRotation = Rotation * other.Rotation;
			return CFrame(transformedPosition, transformedRotation);
		};
	);
} // namespace gargantuan
