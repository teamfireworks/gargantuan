#include "gargantuan.core/services/ProcessService.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"

#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	G_INSTANCE_IMPL(
		ProcessService,
		.Description = "Provides runtime access to system processes",
		.Methods = {
			{"ExitAsync", Method{&ProcessService::LExitAsync}},
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
}
