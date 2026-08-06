#include "gargantuan/services/ProcessService.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <iostream>
#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	I_IMPL(
		ProcessService,
		.Description = "Provides runtime access to system processes",
		.Methods = {
			{"ExitAsync", Method{&ProcessService::LExitAsync}},
			{"WriteToStdout", Method{&ProcessService::LWriteToStdout}},
		}
	);

	void ProcessService::ExitAsync(int exitCode) {
		Alive = false;
		ExitCode = exitCode;
	}

	int ProcessService::LExitAsync(lua_State *L, Instance *self) {
		auto processService = self->Cast<ProcessService>();
		int exitCode = luaL_checknumber(L, 2);
		if (processService->Alive) processService->ExitAsync(exitCode);
		return lua_yield(L, 0);
	}

	int ProcessService::LWriteToStdout(lua_State *L, Instance *self) {
		auto processService = self->Cast<ProcessService>();
		auto numArguments = lua_gettop(L);
		for (int idx = 2; idx <= numArguments; idx++) {
			auto str = luaL_checkstring(L, idx);
			std::cout << str;
		}
		return 0;
	}
}
