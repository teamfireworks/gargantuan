// Copyright (c) 2026 Team Fireworks Games
// Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
//   https://gargantuan.teamfireworks.org/
//
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at:
//   http://mozilla.org/MPL/2.0/

#include "core/classes/gg_Instance.hpp"
#include "core/reflection/gg_LuaValue.hpp"
#include <lua.h>

int gg_Instance::__index(lua_State *L) {
  auto self = CheckLuaValue<gg_Instance>(L, 1);
  auto key = CheckLuaValue<const char *>(L, 2);
  return 0;
}

int gg_Instance::__newindex(lua_State *L) {
  auto self = CheckLuaValue<gg_Instance>(L, 1);
  auto key = CheckLuaValue<const char *>(L, 2);
  return 0;
}

int gg_Instance::__namecall(lua_State *L) {
  auto self = CheckLuaValue<gg_Instance>(L, 1);
  auto key = lua_namecallatom(L, nullptr);
  return 0;
}
