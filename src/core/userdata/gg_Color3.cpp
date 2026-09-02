// Copyright (c) 2026 Team Fireworks Games
// Part of Gargantuan Engine - An Independent Game Engine for Roblox Developers:
//   https://gargantuan.teamfireworks.org/
//
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at:
//   http://mozilla.org/MPL/2.0/

#include "core/userdata/gg_Color3.hpp"

#include <glm/common.hpp>

gg_Color3 gg_Color3::fromRGB(const float &r, const float &g,
                             const float &b) noexcept {
  return gg_Color3{r / 255.0f, g / 255.0f, b / 255.0f};
}

gg_Color3 gg_Color3::fromHSV(const float &h, const float &s,
                             const float &v) noexcept {
  auto i = glm::floor(h * 6);
  auto f = h * 6 - i;
  auto p = v * (1 - s);
  auto q = v * (1 - f * s);
  auto t = v * (1 - (1 - f) * s);

  float r, g, b;
  i = glm::mod(i, 6.0f);

  return (i == 0)   ? gg_Color3(v, t, p)
         : (i == 1) ? gg_Color3(q, v, p)
         : (i == 2) ? gg_Color3(p, v, t)
         : (i == 3) ? gg_Color3(p, q, v)
         : (i == 4) ? gg_Color3(t, p, v)
                    : gg_Color3(v, p, q);
}

gg_Color3 gg_Color3::Lerp(const gg_Color3 &goal,
                          const float &alpha) const noexcept {
  return gg_Color3{
      R + (goal.R - R) * alpha,
      G + (goal.G - G) * alpha,
      B + (goal.B - B) * alpha,
  };
}
