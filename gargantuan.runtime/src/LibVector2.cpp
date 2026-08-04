#include "gargantuan.core/datatypes/Vector2.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"

#include <lualib.h>

namespace gargantuan {
	int LibVector2_new(lua_State *L) {
		float x = luaL_optnumber(L, 1, 0.0f);
		float y = luaL_optnumber(L, 2, 0.0f);
		StackValue<Vector2>::Push(L, Vector2(x, y));
		return 1;
	}

	luaL_Reg LibVector2[] = {
		{"new", LibVector2_new},
		{nullptr, nullptr},
	};

	int OpenLibVector2(lua_State *L) {
		luaL_register(L, "Vector2", LibVector2);
		return 0;
	}
} // namespace gargantuan
