#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <lualib.h>
#include <memory>

namespace gargantuan {
	int LibSignal_new(lua_State *L) {
		auto signal = std::make_shared<UserSignal>();
		return StackValue<UserSignal::Pointer>::Push(L, signal);
		return 1;
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
