#include "gargantuan/scripting/StackGuard.hpp"

#include <SDL3/SDL_log.h>
#include <cassert>
#include <lua.h>
#include <source_location>

namespace gargantuan {

#ifdef NDEBUG
	StackGuard::StackGuard(lua_State *_, std::source_location _loc) {}
	StackGuard::~StackGuard() {}
	StackGuard &StackGuard::Reserve(int _) { return *this; }
	StackGuard &StackGuard::Unreserve(int _) { return *this; }
	StackGuard &StackGuard::Expect(int _) { return *this; }
#else
	StackGuard::StackGuard(lua_State *L, std::source_location loc) : Source(loc), L(L), ExpectedSize(lua_gettop(L)) {}
	StackGuard::~StackGuard() {
		int realSize = lua_gettop(L);
		if (realSize != ExpectedSize) {
			SDL_LogWarn(
				SDL_LOG_CATEGORY_APPLICATION,
				"Expected Luau stack to be %d, got %d (difference of %d items)",
				ExpectedSize,
				realSize,
				realSize - ExpectedSize
			);
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "  Function: %s", Source.function_name());
			SDL_LogWarn(
				SDL_LOG_CATEGORY_APPLICATION, "  In: %s:%d:%d", Source.file_name(), Source.line(), Source.column()
			);
		}
	}

	StackGuard &StackGuard::Expect(int count) {
		ExpectedSize = count;
		return *this;
	}

	StackGuard &StackGuard::Reserve(int count) {
		ExpectedSize += count;
		return *this;
	}

	StackGuard &StackGuard::Unreserve(int count) {
		ExpectedSize -= count;
		return *this;
	}
#endif
}
