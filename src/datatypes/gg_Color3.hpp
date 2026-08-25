// Copyright 2026 rniraclefire o/b/o Team Fireworks Games
// Part of Gargantuan, an independent game engine for Roblox developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <tuple>

#include <glm/ext.hpp>

// NOTE: Implementing this by hand for now to get a reference for implementing
// codegen

struct gg_Color3 {
public:
  double R, G, B;

  static gg_Color3 fromRGB(double r, double g, double b);
  static gg_Color3 fromHSV(double &hue, double &saturation, double &value);
  static gg_Color3 fromOklab(double &lightness, double &a, double &b);
  static gg_Color3 fromOklch(double &lightness, double &chroma, double &hue);

  std::tuple<double, double, double> ToHSV() const;
  std::tuple<double, double, double> ToOklab() const;
  std::tuple<double, double, double> ToOklch() const;

  glm::vec3 PackHSV() const;
  glm::vec3 PackOklab() const;
  glm::vec3 PackOklch() const;

  gg_Color3 Add(const gg_Color3 &other) const;
  gg_Color3 Sub(const gg_Color3 &other) const;
  gg_Color3 Mul(const gg_Color3 &other) const;
  gg_Color3 Div(const gg_Color3 &other) const;
  std::string ToString() const;

private:
  void ToHSV(double &hue, double &saturation, double &value);
  void ToOklab(double &lightness, double &a, double &b);
  void ToOklch(double &lightness, double &chroma, double &hue);
};
