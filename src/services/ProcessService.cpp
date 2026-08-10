#include "gargantuan/services/ProcessService.hpp"

#include <iostream>
#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	void ProcessService::MarkExit(int exitCode) {
		Alive = false;
		ExitCode = exitCode;
	}

	int ProcessService::ExitAsync(lua_State *L, Instance *self) {
		auto processService = dynamic_cast<ProcessService *>(self);
		int exitCode = luaL_checknumber(L, 2);
		if (processService->Alive) processService->MarkExit(exitCode);
		return lua_yield(L, 0);
	}

	int ProcessService::WriteToStdout(lua_State *L, Instance *self) {
		auto processService = dynamic_cast<ProcessService *>(self);
		auto numArguments = lua_gettop(L);
		for (int idx = 2; idx <= numArguments; idx++) {
			auto str = luaL_checkstring(L, idx);
			std::cout << str;
		}
		return 0;
	}

	void ProcessService::FlushStdout() {
		std::cout << std::flush;
	}
}
