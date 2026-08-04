#include "gargantuan.runtime/ScriptEngine.hpp"
#include "gargantuan.runtime/ThreadEngine.hpp"

#include <lua.h>
#include <lualib.h>

namespace gargantuan {
	lua_State *GetThreadFromArgument(lua_State *L, int startIdx, int &argumentsToPass) {
		int argumentCount = lua_gettop(L);

		if (argumentCount < startIdx) {
			luaL_typeerror(L, startIdx, "function or thread");
			return 0;
		}

		startIdx = lua_absindex(L, startIdx);

		lua_State *targetThread = nullptr;
		argumentsToPass = argumentCount - startIdx;

		if (lua_isfunction(L, startIdx)) {
			targetThread = lua_newthread(L);
			lua_pushvalue(L, startIdx);
			lua_xmove(L, targetThread, 1);
		} else if (lua_isthread(L, startIdx)) {
			targetThread = lua_tothread(L, startIdx);
			lua_pushvalue(L, startIdx);
		} else {
			luaL_typeerror(L, startIdx, "function or thread");
			return nullptr;
		}

		if (argumentsToPass > 0) {
			for (int i = startIdx + 1; i <= argumentCount; ++i) {
				lua_pushvalue(L, i);
			}
			lua_xmove(L, targetThread, argumentsToPass);
		}

		return targetThread;
	}

	int LibTask_cancel(lua_State *L) {
		luaL_error(L, "not yet implemented");
	}

	int LibTask_spawn(lua_State *L) {
		int argumentsToPass;
		lua_State *thread = GetThreadFromArgument(L, 1, argumentsToPass);

		auto scriptEngine = ScriptEngine::Get(L);
		scriptEngine->Threads.ResumeThread(thread, scriptEngine->Threads.TakeThreadReference(thread), argumentsToPass);

		return 1;
	}

	int LibTask_defer(lua_State *L) {
		int argumentsToPass;
		lua_State *thread = GetThreadFromArgument(L, 1, argumentsToPass);

		auto scriptEngine = ScriptEngine::Get(L);
		scriptEngine->Threads.QueueDeferredTask(thread, argumentsToPass);

		return 1;
	}

	int LibTask_delay(lua_State *L) {
		double delaySeconds = luaL_checknumber(L, 1);

		int argumentsToPass;
		lua_State *thread = GetThreadFromArgument(L, 2, argumentsToPass);

		auto scriptEngine = ScriptEngine::Get(L);
		scriptEngine->Threads.QueueScheduledTask(
			thread, ThreadEngine::ScheduledTask::Type::Delay, delaySeconds, argumentsToPass
		);

		return 1;
	}

	int LibTask_desynchronize(lua_State *L) {
		luaL_error(L, "not yet implemented");
	}

	int LibTask_synchronize(lua_State *L) {
		luaL_error(L, "not yet implemented");
	}

	int LibTask_wait(lua_State *L) {
		auto scriptEngine = ScriptEngine::Get(L);
		double delaySeconds = luaL_optnumber(L, 1, 0.0f);
		lua_settop(L, 0);
		scriptEngine->Threads.QueueScheduledTask(L, ThreadEngine::ScheduledTask::Type::Wait, delaySeconds, 1);
		return lua_yield(L, 0);
	}

	luaL_Reg LibTask[]{
		{"cancel", LibTask_cancel},
		{"defer", LibTask_defer},
		{"delay", LibTask_delay},
		{"desynchronize", LibTask_desynchronize},
		{"synchronize", LibTask_synchronize},
		{"spawn", LibTask_spawn},
		{"wait", LibTask_wait},
		{nullptr, nullptr}
	};

	int OpenLibTask(lua_State *L) {
		luaL_register(L, "task", LibTask);
		return 0;
	}
} // namespace gargantuan
