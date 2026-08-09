#pragma once

#include "gargantuan/classes/generated/LuaSourceContainer.hpp"

#include <SDL3/SDL.h>
#include <lua.h>
#include <luacode.h>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace gargantuan {
	enum class BytecodeCompileStatus : int { Uncompiled, Success, Error };

	class LuaSourceContainer : public Instance {
		I_LuaSourceContainer;

		std::string ChunkName = GetFullName();

		std::vector<char> Bytecode;
		size_t BytecodeSize;
		BytecodeCompileStatus BytecodeCompileStatus = BytecodeCompileStatus::Uncompiled;
		std::optional<std::string> BytecodeCompileError;

		void CompileBytecode(lua_CompileOptions *options = {});
		[[nodiscard]] std::optional<std::string> LoadIntoState(lua_State *L);
	};

	template <typename T>
		requires std::is_base_of_v<LuaSourceContainer, T> && (!std::is_same_v<LuaSourceContainer, T>)
	[[nodiscard]] static std::shared_ptr<T> ScriptFromFile(const char *filepath) {
		size_t fileSize;
		void *code = SDL_LoadFile(filepath, &fileSize);

		if (code == nullptr) {
			throw std::runtime_error(
				std::format(
					"Failed to load {} from file {}: {}", T::CLASS_DEFINITION.ClassName, filepath, SDL_GetError()
				)
			);
		};

		std::string source((char *)code, fileSize);
		SDL_free(code);

		auto self = std::make_shared<T>();
		self->ChunkName = std::string(filepath);
		self->SetSource(source);
		return self;
	};
}
