#include "gargantuan.core/datatypes/UDim.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"

#include <lualib.h>

namespace gargantuan {
	int LibUDim_new(lua_State *L) {
		float scale = luaL_optnumber(L, 1, 0.0f);
		int offset = luaL_optnumber(L, 1, 0);
		StackValue<UDim>::Push(L, {scale, offset});
		return 1;
	}

	luaL_Reg LibUDim[]{
		{"new", LibUDim_new},
		{nullptr, nullptr},
	};

	int OpenLibUDim(lua_State *L) {
		luaL_register(L, "UDim", LibUDim);
		return 0;
	}
} // namespace gargantuan
