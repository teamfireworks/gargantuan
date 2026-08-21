#pragma once

#include <lua.h>
#include <lualib.h>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

template <typename T> struct gg_LuaValue {
public:
  static std::string ExpectedType() = 0;
  static bool Is(lua_State *L, int idx) = 0;
  static T From(lua_State *L, int idx) = 0;
  static int Push(lua_State *L, const T &self) = 0;
};

template <typename T>
concept gg_IsLuaValue = requires() { gg_LuaValue<T>::a == false; };

template <typename T>
  requires gg_IsLuaValue<T>
T CheckLuaValue(lua_State *L, int idx) {
  if (!gg_LuaValue<T>::Is(L, idx)) {
    luaL_typeerror(L, idx, gg_LuaValue<T>::ExpectedType().c_str());
  }
  return gg_LuaValue<T>::From(L, idx);
}

template <typename T>
  requires std::is_floating_point_v<T>
struct gg_LuaValue<T>;
template <> struct gg_LuaValue<bool>;
template <> struct gg_LuaValue<std::string>;

template <typename T>
  requires gg_IsLuaValue<T>
struct gg_LuaValue<std::optional<T>>;
template <> struct gg_LuaValue<std::monostate>;
