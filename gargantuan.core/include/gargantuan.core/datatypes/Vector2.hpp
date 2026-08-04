#pragma once

#include "gargantuan.runtime/Userdata.hpp"

#include <glm/glm.hpp>

namespace gargantuan {
	G_USERDATA_DECL(
		Vector2,

		glm::vec2 Value{0.0f, 0.0f};

		Vector2(float x = 0.0f, float y = 0.0f);
		Vector2(glm::vec2 vec);

		float GetX() const;
		float GetY() const;
		float GetMagnitude() const;
		Vector2 GetUnit() const;

		float Cross(const Vector2 &other) const;
		Vector2 Abs() const;
		Vector2 Ceil() const;
		Vector2 Floor() const;
		Vector2 Sign() const;
		float Angle(const Vector2 &other, bool isSigned = false) const;
		float Dot(const Vector2 &other) const;
		Vector2 Lerp(const Vector2 &goal, float alpha) const;
		Vector2 Max(const Vector2 &other) const;
		Vector2 Min(const Vector2 &other) const;
		Vector2 FuzzyEq(const Vector2 &other, float epsilon = 1e-5) const;

		static int LTostring(lua_State *L, Vector2 *self);
		static int LAdd(lua_State *L, Vector2 *self);
		static int LSub(lua_State *L, Vector2 *self);
		static int LMul(lua_State *L, Vector2 *self);
		static int LDiv(lua_State *L, Vector2 *self);

		Vector2 operator+(const Vector2 & other) { return Value + other.Value; };
		Vector2 operator-(const Vector2 & other) { return Value - other.Value; };
		Vector2 operator*(const Vector2 & other) { return Value * other.Value; };
		Vector2 operator/(const Vector2 & other) { return Value / other.Value; };
		Vector2 operator*(float other) { return Value * other; };
		Vector2 operator/(float other) { return Value / other; };

		operator glm::vec2() const { return Value; }
	)
}
