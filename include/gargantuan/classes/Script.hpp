#pragma once

#include "gargantuan/classes/generated/Script.hpp"

#include <lua.h>
#include <string>

namespace gargantuan {
	G_ENUM(RunContext, Client, Server, None);

	enum class ScriptStatus : int {
		Disabled,
		Idle,
		Running,
		Yielded,
		Error,
		Finished,
	};

	class Script : public LuaSourceContainer {
		I_Script;

		Script();

		ScriptStatus Status = ScriptStatus::Idle;
		std::string ErrorMessage = "";
		lua_State *Thread = nullptr;
		int ThreadReference = LUA_NOREF;

		void Cleanup();
		bool ShouldStep();
		ScriptStatus Step(lua_State *L);
	};
}
