#include "gargantuan/Log.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <iostream>
#include <lua.h>
#include <lualib.h>
#include <sstream>

namespace gargantuan {
	int print(lua_State *L) {
		int n = lua_gettop(L);
		std::ostringstream ss;
		for (int i = 1; i <= n; i++) {
			size_t l;
			if (i > 1) ss << " ";
			ss << luaL_tolstring(L, i, &l);
			lua_pop(L, 1); // pop result
		}
		ss << std::endl;
		LOG_INFO(Lua, "%s", ss.str().c_str());
		return 0;
	}

	int warn(lua_State *L) {
		int n = lua_gettop(L);
		std::ostringstream ss;
		for (int i = 1; i <= n; i++) {
			size_t l;
			if (i > 1) ss << " ";
			ss << luaL_tolstring(L, i, &l);
			lua_pop(L, 1); // pop result
		}
		ss << std::endl;
		LOG_WARN(Lua, "%s", ss.str().c_str());
		return 0;
	}

	int OpenLibBase(lua_State *L) {
		auto scriptEngine = ScriptEngine::Get(L);

		// Disabling for now since Lest needs stdout
		// TODO.... ProcessService:WriteStdout
		// lua_pushcclosurek(L, print, "print", 0, nullptr);
		// lua_setglobal(L, "print");

		lua_pushcclosurek(L, warn, "warn", 0, nullptr);
		lua_setglobal(L, "warn");

		StackValue<std::shared_ptr<Instance>>::Push(L, scriptEngine->DataModel);
		lua_setglobal(L, "game");

		lua_createtable(L, 0, 0);
		{
			lua_pushliteral(L, "gargantuan");
			lua_setfield(L, -2, "name");

			lua_pushliteral(L, "https://gargantuan.teamfireworks.org/");
			lua_setfield(L, -2, "url");

			lua_createtable(L, 0, 0);
			{
				lua_pushliteral(L, "0.0.0-indev");
				lua_setfield(L, -2, "display");

				lua_createtable(L, 0, 0);
				{
					lua_pushliteral(L, "https://github.com/teamfireworks/gargantuan.git/");
					lua_setfield(L, -2, "url");
				}
				lua_setreadonly(L, -1, true);
				lua_setfield(L, -2, "git");
			}
			lua_setreadonly(L, -1, true);
			lua_setfield(L, -2, "version");
		}
		lua_setreadonly(L, -1, true);
		lua_setglobal(L, "_RUNTIME");

		return 0;
	}
}
