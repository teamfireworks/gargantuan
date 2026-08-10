#include "gargantuan/classes/Script.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <lua.h>

namespace gargantuan {
	Script::Script() {
		Destroying->Connect([this](std::monostate _) { this->Cleanup(); });
	}

	bool Script::GetEnabled() const {
		return Status != ScriptStatus::Disabled;
	}

	void Script::SetEnabled(bool enabled) {
		GetPropertyChangedSignal("Enabled")->Fire({});
		if (Status == ScriptStatus::Disabled && enabled) {
			Status = ScriptStatus::Idle;
		} else if (Status != ScriptStatus::Disabled && !enabled) {
			Cleanup();
			Status = ScriptStatus::Disabled;
		}
	}

	void Script::Cleanup() {
		if (Thread && ThreadReference) {
			auto L = lua_mainthread(Thread);
			lua_unref(L, ThreadReference);
			ThreadReference = LUA_NOREF;
			Thread = nullptr;
		}
	}

	bool Script::ShouldStep() {
		switch (Status) {
		case ScriptStatus::Error:
		case ScriptStatus::Disabled:
		case ScriptStatus::Yielded:
		case ScriptStatus::Finished:
			return false;

		default:
			return true;
		}
	}

	ScriptStatus Script::Step(lua_State *L) {
		if (!ShouldStep() || Thread) return Status;
		Status = ScriptStatus::Running;

		auto scriptEngine = ScriptEngine::Get(L);
		CompileBytecode(&scriptEngine->CompileOptions);
		if (BytecodeCompileStatus != BytecodeCompileStatus::Success) {
			Status = ScriptStatus::Error;
			ErrorMessage = BytecodeCompileError.value();
			return Status;
		}

		Thread = lua_newthread(L);

		auto loadError = LoadIntoState(Thread);
		if (loadError.has_value()) {
			Status = ScriptStatus::Error;
			ErrorMessage = loadError.value();
			return Status;
		}

		lua_pushthread(Thread);
		lua_xmove(Thread, L, 1);
		ThreadReference = lua_ref(L, 1);
		// lua_ref keeps the value on the stack
		lua_pop(L, 1);

		auto status = lua_resume(Thread, L, 0);
		switch (status) {
		case LUA_YIELD:
		case LUA_BREAK:
			Status = ScriptStatus::Yielded;
			break;

		case LUA_OK:
			Status = ScriptStatus::Finished;
			break;

		default:
			Status = ScriptStatus::Error;
			ErrorMessage = lua_tostring(Thread, -1);
			break;
		}

		return Status;
	}
}
