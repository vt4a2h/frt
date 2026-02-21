/*
 * FRT -- Result types for modern C++.
 *
 * Copyright Vitaly Fanaskov 2026-present.
 *
 * Use, modification and distribution are subject to the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or a copy at http://www.boost.org/LICENSE_1_0.txt.
 *
 * Project home: https://github.com/vt4a2h/frt.
 */
#pragma once

#include <initializer_list>
#include <string>
#include <vector>

namespace test::helpers
{

struct TestArgsIl
{
  constexpr TestArgsIl(int v, double v1) : v_{v}, v1_{v1} {}
  constexpr TestArgsIl(std::initializer_list<std::string> vs, int v, double v1) : v_{v}, v1_{v1}, vs_{vs} {}

  friend constexpr bool operator==(const TestArgsIl &, const TestArgsIl &) = default;

  int v_{};
  double v1_{};
  std::vector<std::string> vs_{};
};

struct ComparableTypeFirst
{
  std::string d_{};

  friend constexpr bool operator==(const ComparableTypeFirst &, const ComparableTypeFirst &) noexcept = default;
  friend constexpr auto operator<=>(const ComparableTypeFirst &, const ComparableTypeFirst &) noexcept = default;
};

struct ComparableTypeSecond
{
  int d_{};

  friend constexpr bool operator==(const ComparableTypeSecond &, const ComparableTypeSecond &) noexcept = default;
  friend constexpr auto operator<=>(const ComparableTypeSecond &, const ComparableTypeSecond &) noexcept = default;
};

} // namespace test::helpers
