// Copyright 2026 Brooke Nguyen o/b/o Team Fireworks Games
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "core/reflection/gg_LuaValue.hpp"

#include <lua.h>
#include <optional>
#include <string>

template <typename T>
  requires std::is_floating_point_v<T>
struct gg_LuaValue<T> {
  static std::string ExpectedType() { return "number"; }

  static bool Is(lua_State *L, int idx) { return lua_isnumber(L, idx); }

  static T From(lua_State *L, int idx) {
    return static_cast<T>(lua_tonumber(L, idx));
  }

  static int Push(lua_State *L, const T &self) {
    lua_pushnumber(L, self);
    return 1;
  }
};

template <> struct gg_LuaValue<bool> {
  static std::string ExpectedType() { return "boolean"; }

  static bool Is(lua_State *L, int idx) { return lua_isboolean(L, idx); }

  static bool From(lua_State *L, int idx) { return lua_toboolean(L, idx); }

  static int Push(lua_State *L, const bool &self) {
    lua_pushboolean(L, self);
    return 1;
  }
};

template <> struct gg_LuaValue<std::string> {
  static std::string ExpectedType() { return "string"; };

  static bool Is(lua_State *L, int idx) { return lua_isstring(L, idx); };

  static std::string From(lua_State *L, int idx) {
    size_t size;
    auto str = lua_tolstring(L, idx, &size);
    return {str, size};
  }

  static int Push(lua_State *L, const std::string &self) {
    lua_pushlstring(L, self.data(), self.size());
    return 1;
  }
};

template <typename T>
  requires gg_IsLuaValue<T>
struct gg_LuaValue<std::optional<T>> {
  using Inner = gg_LuaValue<T>;

  static std::string ExpectedType() { return Inner::ExpectedType() + "?"; }

  static bool Is(lua_State *L, int idx) {
    return lua_isnoneornil(L, idx) || Inner::Is(L, idx);
  }

  static std::optional<T> From(lua_State *L, int idx) {
    return lua_isnoneornil(L, idx) ? std::nullopt : Inner::From(L, idx);
  }

  static int Push(lua_State *L, const std::optional<T> &self) {
    if (self.has_value()) {
      return Inner::Push(L, self.value());
    } else {
      lua_pushnil(L);
      return 1;
    }
  }
};

template <> struct gg_LuaValue<std::monostate> {
  static std::string ExpectedType() { return "nil"; }

  static bool Is(lua_State *L, int idx) { return lua_isnone(L, idx); }

  static std::monostate From(lua_State *L, int idx) { return {}; }

  static int Push(lua_State *L, const std::monostate &self) { return 0; }
};
