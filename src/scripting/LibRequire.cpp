#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/classes/ModuleScript.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <Luau/Require.h>
#include <cstring>
#include <format>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <optional>
#include <string>

namespace gargantuan {
	static luarequire_WriteResult
	CopyStringToBuffer(const std::string &str, char *buffer, size_t bufferSize, size_t *outputSize) {
		*outputSize = str.size() + 1;
		if (str.size() + 1 > bufferSize) {
			return WRITE_BUFFER_TOO_SMALL;
		}
		std::memcpy(buffer, str.c_str(), *outputSize);
		return WRITE_SUCCESS;
	}

	static void LibRequire_InitConfiguration(luarequire_Configuration *config) {
		config->is_require_allowed = [](lua_State *L, void *ctx, const char *requirer_chunkname) { return true; };

		config->reset = [](lua_State *L, void *ctx, const char *requirer_chunkname) -> luarequire_NavigateResult {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);

			auto instance = scriptEngine->FindRequiredInstanceByPath(requirer_chunkname);
			if (instance) {
				scriptEngine->RequireCurrentInstance = instance;
				return NAVIGATE_SUCCESS;
			}

			return NAVIGATE_NOT_FOUND;
		};

		config->jump_to_alias = [](lua_State *L, void *ctx, const char *alias) -> luarequire_NavigateResult {
			return NAVIGATE_NOT_FOUND;
		};

		config->to_alias_override = [](lua_State *L, void *ctx, const char *alias) -> luarequire_NavigateResult {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);

			if (std::strcmp(alias, "game") == 0) {
				scriptEngine->RequireCurrentInstance = scriptEngine->DataModel;
				return NAVIGATE_SUCCESS;
			}

			return NAVIGATE_NOT_FOUND;
		};

		config->to_alias_fallback = [](lua_State *L, void *ctx, const char *alias) -> luarequire_NavigateResult {
			return NAVIGATE_NOT_FOUND;
		};

		config->to_parent = [](lua_State *L, void *ctx) -> luarequire_NavigateResult {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);

			if (auto parent = scriptEngine->RequireCurrentInstance->ParentPointer) {
				scriptEngine->RequireCurrentInstance = parent->shared_from_this();
				return NAVIGATE_SUCCESS;
			}

			return NAVIGATE_NOT_FOUND;
		};

		config->to_child = [](lua_State *L, void *ctx, const char *name) -> luarequire_NavigateResult {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);

			if (auto child = scriptEngine->RequireCurrentInstance->FindFirstChild(name, std::nullopt)) {
				scriptEngine->RequireCurrentInstance = child;
				return NAVIGATE_SUCCESS;
			}

			return NAVIGATE_NOT_FOUND;
		};

		config->is_module_present = [](lua_State *L, void *ctx) -> bool {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);
			return scriptEngine->RequireCurrentInstance &&
				   std::static_pointer_cast<gargantuan::ModuleScript>(scriptEngine->RequireCurrentInstance) != nullptr;
		};

		config->get_chunkname =
			[](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);
			auto fullName = scriptEngine->RequireCurrentInstance->GetFullName();
			return CopyStringToBuffer(fullName.c_str(), buffer, buffer_size, size_out);
		};

		config->get_loadname = config->get_chunkname;

		config->get_cache_key =
			[](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);
			void *modulePointer = scriptEngine->RequireCurrentInstance.get();
			std::string key = std::format("module: {}", modulePointer);
			return CopyStringToBuffer(key.c_str(), buffer, buffer_size, size_out);
		};

		config->get_config_status = [](lua_State *L, void *ctx) -> luarequire_ConfigStatus {
			// TODO: implement BaseFiletree, DiskFiletree, and VirtualFiletree
			// for projects so we can have luauconfigs
			return CONFIG_ABSENT;
		};

		config->get_config =
			[](lua_State *L, void *ctx, char *buffer, size_t buffer_size, size_t *size_out) -> luarequire_WriteResult {
			// do absolutely fucking nothing
			return WRITE_FAILURE;
		};

		config->load =
			[](lua_State *L, void *ctx, const char *path, const char *chunkname, const char *loadname) -> int {
			auto scriptEngine = static_cast<ScriptEngine *>(ctx);

			if (!scriptEngine->RequireCurrentInstance) luaL_error(L, "Cannot require nil instance");

			auto module = static_pointer_cast<gargantuan::ModuleScript>(scriptEngine->RequireCurrentInstance);
			if (!module) {
				luaL_error(
					L,
					"Cannot require %s because it is not a ModuleScript",
					scriptEngine->RequireCurrentInstance->GetFullName().c_str()
				);
			};

			module->CompileBytecode(&scriptEngine->CompileOptions);
			if (module->BytecodeCompileStatus != BytecodeCompileStatus::Success) {
				lua_pushstring(L, module->BytecodeCompileError.value().c_str());
				lua_error(L);
			}

			if (luau_load(L, chunkname, module->Bytecode.data(), module->BytecodeSize, 0) != LUA_OK) {
				lua_error(L);
				return 0;
			}

			if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
				lua_error(L);
				return 0;
			}

			return 1;
		};
	}

	int OpenLibRequire(lua_State *L) {
		void *scriptEngine = ScriptEngine::Get(L);
		luaopen_require(L, LibRequire_InitConfiguration, scriptEngine);
		return 0;
	}
}
