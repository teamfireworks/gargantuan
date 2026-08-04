#pragma once

#include <lua.h>
#include <lualib.h>
#include <source_location>

namespace gargantuan {
	inline void EnsureStackSpace(lua_State *L, int slots) {
		if (slots <= 0) {
			return;
		}

		if (!lua_checkstack(L, slots)) {
			luaL_error(L, "Cannot grow the Luau stack by %d slots", slots);
		}
	}

	inline bool TryEnsureStackSpace(lua_State *L, int slots) {
		return slots <= 0 || lua_checkstack(L, slots) != 0;
	}

	class StackGuard {
	  public:
		StackGuard(lua_State *L, std::source_location loc = std::source_location::current());
		~StackGuard();

		StackGuard &Reserve(int count);
		StackGuard &Unreserve(int count);
		StackGuard &Expect(int count);

		StackGuard(const StackGuard &) = delete;
		StackGuard &operator=(const StackGuard &) = delete;

	  private:
		lua_State *L;
		std::source_location Source;
		int ExpectedSize;
	};
}
