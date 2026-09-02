// Copyright (c) 2026 Team Fireworks Games
// Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
//   https://gargantuan.teamfireworks.org/
//
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at:
//   http://mozilla.org/MPL/2.0/

#pragma once

#include "core/generated/userdatas/gg_Color3.hpp"

struct gg_Color3 {
  GG_COLOR3_START;
  gg_Color3(const float &r = 0.0f, const float &g = 0.0f,
            const float &b = 0.0f) noexcept;
  GG_COLOR3_END;
};
