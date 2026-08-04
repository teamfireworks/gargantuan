#include "gargantuan.core/datatypes/Vector2.hpp"
#include "gargantuan.runtime/Userdata.hpp"
#include "gargantuan.runtime/UserdataTag.hpp"

#include <cstdlib>
#include <lua.h>
#include <lualib.h>
#include <sstream>

namespace gargantuan {
	G_USERDATA_IMPL(
		Vector2,
		.Tag = UserdataTag::Vector2,
		.Type = "Vector2",
		.Properties =
			{
				{"X", Property::fromRead([](Vector2 *self) { return self->GetX(); })},
				{"Y", Property::fromRead([](Vector2 *self) { return self->GetY(); })},
				{"Unit", Property::fromRead([](Vector2 *self) { return self->GetUnit(); })},
				{"Magnitude", Property::fromRead([](Vector2 *self) { return self->GetMagnitude(); })},
			},
		.Methods = {
			{"Cross", Method::fromMember<&Vector2::Cross>()},
			{"Abs", Method::fromMember<&Vector2::Abs>()},
			{"Ceil", Method::fromMember<&Vector2::Ceil>()},
			{"Floor", Method::fromMember<&Vector2::Floor>()},
			{"Sign", Method::fromMember<&Vector2::Sign>()},
			{"Angle", Method::fromMember<&Vector2::Angle>()},
			{"Dot", Method::fromMember<&Vector2::Dot>()},
			{"Lerp", Method::fromMember<&Vector2::Lerp>()},
			{"Max", Method::fromMember<&Vector2::Max>()},
			{"Min", Method::fromMember<&Vector2::Min>()},
			{"__tostring", Method{Vector2::LTostring}},
			{"__add", Method{Vector2::LAdd}},
			{"__sub", Method{Vector2::LSub}},
			{"__mul", Method{Vector2::LMul}},
			{"__div", Method{Vector2::LDiv}}
		}
	)

	Vector2::Vector2(float x, float y) : Value(x, y) {};
	Vector2::Vector2(glm::vec2 vec) : Value(vec) {};

	float Vector2::GetX() const {
		return Value.x;
	};

	float Vector2::GetY() const {
		return Value.y;
	};

	float Vector2::GetMagnitude() const {
		return glm::length(Value);
	};

	Vector2 Vector2::GetUnit() const {
		return glm::normalize(Value);
	};

	float Vector2::Cross(const Vector2 &other) const {
		return (GetX() * other.GetY()) - (GetY() * GetX());
	};

	Vector2 Vector2::Abs() const {
		return glm::abs(Value);
	};

	Vector2 Vector2::Ceil() const {
		return glm::ceil(Value);
	};

	Vector2 Vector2::Floor() const {
		return glm::floor(Value);
	};

	Vector2 Vector2::Sign() const {
		return glm::sign(Value);
	};

	float Vector2::Angle(const Vector2 &other, bool isSigned) const {
		float angle = atan2(Cross(other), Dot(other));
		return isSigned ? angle : abs(angle);
	};

	float Vector2::Dot(const Vector2 &other) const {
		return GetX() * other.GetX() + GetY() * other.GetY();
	};

	Vector2 Vector2::Lerp(const Vector2 &goal, float alpha) const {
		return Value + (goal.Value - Value) * alpha;
	};

	Vector2 Vector2::Max(const Vector2 &other) const {
		return glm::max(Value, other.Value);
	};

	Vector2 Vector2::Min(const Vector2 &other) const {
		return glm::min(Value, other.Value);
	};

	Vector2 Vector2::FuzzyEq(const Vector2 &other, float epsilon) const {
		return glm::abs(Value.x - this->Value.x) <= epsilon && glm::abs(Value.y - this->Value.y) <= epsilon;
	};

	int Vector2::LTostring(lua_State *L, Vector2 *self) {
		std::ostringstream ss;
		ss << self->GetX() << ", " << self->GetY();
		std::string str = ss.str();
		lua_pushlstring(L, str.c_str(), str.size());
		return 1;
	}

	int Vector2::LAdd(lua_State *L, Vector2 *self) {
		Vector2 other = StackValue<Vector2>::From(L, -1);
		StackValue<Vector2>::Push(L, self->Value + other.Value);
		return 1;
	}

	int Vector2::LSub(lua_State *L, Vector2 *self) {
		Vector2 other = StackValue<Vector2>::From(L, -1);
		StackValue<Vector2>::Push(L, self->Value - other.Value);
		return 1;
	}

	int Vector2::LMul(lua_State *L, Vector2 *self) {
		if (lua_isnumber(L, -1)) {
			float other = lua_tonumber(L, -1);
			StackValue<Vector2>::Push(L, self->Value * other);
		} else if (StackValue<Vector2>::Is(L, -1)) {
			Vector2 other = StackValue<Vector2>::From(L, -1);
			StackValue<Vector2>::Push(L, self->Value * other.Value);
		} else {
			luaL_typeerror(L, 2, "Vector2 or number");
			return 0;
		}
		return 1;
	}

	int Vector2::LDiv(lua_State *L, Vector2 *self) {
		if (lua_isnumber(L, -1)) {
			float other = lua_tonumber(L, -1);
			StackValue<Vector2>::Push(L, self->Value / other);
		} else if (StackValue<Vector2>::Is(L, -1)) {
			Vector2 other = StackValue<Vector2>::From(L, -1);
			StackValue<Vector2>::Push(L, self->Value / other.Value);
		} else {
			luaL_typeerror(L, 2, "Vector2 or number");
			return 0;
		}
		return 1;
	}

} // namespace gargantuan
