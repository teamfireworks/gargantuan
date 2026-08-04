#include "gargantuan.core/datatypes/Axes.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"
#include "gargantuan.runtime/StackValue.hpp"

#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	int LibAxes_new(lua_State *L) {
		auto self = Axes();
		for (int idx = 1; idx <= lua_gettop(L); idx++) {
			self.SetNormal(CheckStackValue<Enums::NormalId>(L, idx));
		}
		StackValue<Axes>::Push(L, self);
		return 1;
	};

	luaL_Reg LibAxes[] = {
		{"new", LibAxes_new},
		{nullptr, nullptr},
	};

	int OpenLibAxes(lua_State *L) {
		luaL_register(L, "Axes", LibAxes);
		return 0;
	}
}
