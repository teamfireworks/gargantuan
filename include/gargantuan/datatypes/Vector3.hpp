#pragma once

#include "gargantuan/scripting/StackValue.hpp"

#include <glm/glm.hpp>
#include <lua.h>

namespace gargantuan {
	template <> struct StackValue<glm::vec3> {
		static inline std::string_view ReflectedTypedef() {
			return "Vector3";
		};
		static bool Is(lua_State *L, int idx) {
			return lua_isvector(L, idx);
		};
		static glm::vec3 From(lua_State *L, int idx) {
			auto vec = lua_tovector(L, idx);
			return glm::vec3{vec[0], vec[1], vec[2]};
		};
		static int Push(lua_State *L, glm::vec3 value) {
			lua_pushvector(L, value.x, value.y, value.z);
			return 1;
		};
	};
}
