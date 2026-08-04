#pragma once

#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <lua.h>
#include <string>

namespace gargantuan {
	G_ENUM(RunContext, Client, Server);
	enum class ScriptStatus : int { Disabled, Idle, Running, Error, Finished };

	class Script : public LuaSourceContainer {
	  public:
		G_INSTANCE_DECL(Script);

		Script();

		Enums::RunContext RunContext = Enums::RunContext::Client;
		static bool IsEnabled(Instance *self);
		static void SetEnabled(Instance *self, bool enabled);

		ScriptStatus Status = ScriptStatus::Idle;
		std::string ErrorMessage = "";
		lua_State *Thread = nullptr;
		int ThreadReference = LUA_NOREF;

		void Cleanup();
		bool ShouldStep();
		ScriptStatus Step(lua_State *L);
	};
}
