// Copyright 2026 Brooke Nguyen o/b/o Team Fireworks Games
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

  template <typename Missing>
  struct GARGANTUAN_STACK_VALUE_IS_UNIMPLEMENTED_FOR {
    static constexpr bool value = false;
  };

  static_assert(GARGANTUAN_STACK_VALUE_IS_UNIMPLEMENTED_FOR<T>::value);
};

// lua_userda

template <typename T>
concept gg_IsLuaValue = requires() { gg_LuaValue<T>::a == false; };

template <typename T> T CheckLuaValue(lua_State *L, int idx) {
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
