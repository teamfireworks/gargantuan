#pragma once

#include <lua.h>
#include <lualib.h>

#include <string>
#include <type_traits>

template <typename T> struct gg_Value {
  public:
	virtual std::string ExpectedType() = 0;
	virtual bool Is(lua_State *L, unsigned int idx) = 0;
	virtual T From(lua_State *L, unsigned int idx) = 0;
	virtual T Push(lua_State *L, const T &self) = 0;
};

template <typename T>
	requires std::is_floating_point_v<T>
struct gg_Value<T> {
	std::string ExpectedType() {
		return "number";
	}

	bool Is(lua_State *L, unsigned int idx) {
		return luaL_checknumber(L, idx);
	}

	T From(lua_State *L, unsigned int idx) {
		return static_cast<T>(lua_tonumber(L, idx));
	}

	T Push(lua_State *L, const T &self) {
		lua_pushnumber(L, self);
		return 1;
	}
};
