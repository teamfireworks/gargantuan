#include "gargantuan.core/classes/Script.hpp"
#include "gargantuan.core/reflection/InstanceClassRegistry.hpp"
#include "gargantuan.runtime/ScriptEngine.hpp"
#include <lua.h>

namespace gargantuan {
	G_INSTANCE_IMPL(
		Script,
		.Superclass = "LuaSourceContainer",
		.Properties = {
			{"Enabled", Property::fromReadWrite<bool>(&Script::IsEnabled, &Script::SetEnabled).SetSerializable()},
			{"RunContext", Property::fromMember<&Script::RunContext>()},
		},
	);

	Script::Script() {
		Destroying->Connect([this](std::monostate _) { this->Cleanup(); });
	}

	bool Script::IsEnabled(Instance *ptr) {
		auto self = ptr->Cast<Script>();
		return self->Status != ScriptStatus::Disabled;
	}

	void Script::SetEnabled(Instance *ptr, bool enabled) {
		auto self = ptr->Cast<Script>();
		if (self->Status == ScriptStatus::Disabled && enabled) {
			self->Status = ScriptStatus::Idle;
		} else if (self->Status != ScriptStatus::Disabled && !enabled) {
			self->Cleanup();
			self->Status = ScriptStatus::Disabled;
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
		case ScriptStatus::Finished:
			return false;

		default:
			return true;
		}
	}

	ScriptStatus Script::Step(lua_State *L) {
		if (!ShouldStep()) return Status;
		Status = ScriptStatus::Running;

		auto scriptEngine = ScriptEngine::Get(L);
		CompileBytecode(&scriptEngine->CompileOptions);
		if (BytecodeCompileStatus != BytecodeCompileStatus::Success) {
			Status = ScriptStatus::Error;
			ErrorMessage = BytecodeCompileError.value();
			return Status;
		}

		if (!Thread) {
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
		}

		auto status = lua_resume(Thread, L, 0);
		switch (status) {
		case LUA_YIELD:
		case LUA_BREAK:
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
