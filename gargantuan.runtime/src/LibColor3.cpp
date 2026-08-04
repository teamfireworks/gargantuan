#include "gargantuan.core/datatypes/Color3.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"
#include <lualib.h>

namespace gargantuan {
	int LibColor3_new(lua_State *L) {
		Color3 color = {
			(float)luaL_opt(L, luaL_checknumber, 1, 0.0f),
			(float)luaL_opt(L, luaL_checknumber, 2, 0.0f),
			(float)luaL_opt(L, luaL_checknumber, 3, 0.0f),
		};
		StackValue<Color3>::Push(L, color);
		return 1;
	}

	int LibColor3_fromRGB(lua_State *L) {
		StackValue<Color3>::Push(
			L, Color3::fromRGB(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3))
		);
		return 1;
	}

	int LibColor3_fromHSV(lua_State *L) {
		StackValue<Color3>::Push(
			L, Color3::fromHSV(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3))
		);
		return 1;
	}

	luaL_Reg LibColor3[]{
		{"new", LibColor3_new},
		{"fromRGB", LibColor3_fromRGB},
		{"fromHSV", LibColor3_fromHSV},
		{nullptr, nullptr},
	};

	int OpenLibColor3(lua_State *L) {
		luaL_register(L, "Color3", LibColor3);
		return 0;
	}
} // namespace gargantuan
