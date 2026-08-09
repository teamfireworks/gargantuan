#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/classes/Instance.hpp"

#include <cstdlib>
#include <format>
#include <optional>
#include <string>

namespace gargantuan {
	void LuaSourceContainer::CompileBytecode(lua_CompileOptions *options) {
		if (BytecodeCompileStatus != BytecodeCompileStatus::Uncompiled) return;

		char *rawBytecode = luau_compile(Source.c_str(), Source.length(), options, &BytecodeSize);

		if (!rawBytecode && BytecodeSize == 0) {
			BytecodeCompileStatus = BytecodeCompileStatus::Error;
			BytecodeCompileError = std::format("Failed to compile: {}", std::string(rawBytecode, BytecodeSize));
			return;
		}

		BytecodeCompileStatus = BytecodeCompileStatus::Success;
		Bytecode.assign(rawBytecode, rawBytecode + BytecodeSize);
		std::free(rawBytecode);
	};

	std::optional<std::string> LuaSourceContainer::LoadIntoState(lua_State *L) {
		if (BytecodeCompileStatus != BytecodeCompileStatus::Success) {
			return "Bytecode must be successfully compiled prior to LuaSourceContainer::LoadIntoState";
		};

		StackValue<std::shared_ptr<Instance>>::Push(L, shared_from_this());
		lua_setglobal(L, "script");

		luaL_sandboxthread(L);

		if (luau_load(L, ChunkName.c_str(), Bytecode.data(), BytecodeSize, 0) != LUA_OK) {
			return std::format("Failed to load %s: %s", ChunkName.c_str(), lua_tostring(L, -1));
		};

		return std::nullopt;
	}
}
