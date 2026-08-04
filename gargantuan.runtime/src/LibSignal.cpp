#include "gargantuan.core/datatypes/Signal.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"

#include <lualib.h>
#include <memory>

namespace gargantuan {
	int LibSignal_new(lua_State *L) {
		// auto signal = std::make_shared<UserSignal>();
		// return StackValue<UserSignal::Pointer>::Push(L, signal);
		luaL_error(L, "Signal.new is disabled for now");
		return 0;
	}

	luaL_Reg LibSignal[]{
		{"new", LibSignal_new},
		{nullptr, nullptr},
	};

	int OpenLibSignal(lua_State *L) {
		luaL_register(L, "Signal", LibSignal);
		return 0;
	}
}
