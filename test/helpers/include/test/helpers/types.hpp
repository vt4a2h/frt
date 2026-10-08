/*
 * FRT -- Result types for modern C++.
 *
 * Copyright (c) 2026-present Vitaly Fanaskov
 *
 * Use, modification and distribution are subject to the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or a copy at https://www.boost.org/LICENSE_1_0.txt
 *
 * Project home: https://github.com/vt4a2h/frt
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

struct NonHashable
{
  friend constexpr bool operator==(const NonHashable &l, const NonHashable &r) noexcept { return l.v_ == r.v_; }
  int v_{};
};

struct Hashable
{
  friend constexpr bool operator==(const Hashable &l, const Hashable &r) noexcept { return l.v_ == r.v_; }
  int v_{};
};

struct Widget
{
  int width{};
  int height{};
};

struct Window
{
  int width{};
  int height{};
};

template <class T>
concept HasWidthAndHeight = requires(T t) {
  t.width;
  t.height;
};

} // namespace test::helpers

template <>
struct std::hash<test::helpers::Hashable>
{
  constexpr std::size_t operator()(const test::helpers::Hashable &h) const noexcept { return h.v_; }
};
