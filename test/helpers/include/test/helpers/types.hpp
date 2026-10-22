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
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
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

struct CopyMoveTracker
{
  int value{};
  int *copies{};
  int *moves{};

  constexpr CopyMoveTracker(int v, int *c, int *m) noexcept : value{v}, copies{c}, moves{m} {}

  constexpr CopyMoveTracker(const CopyMoveTracker &other) noexcept
      : value{other.value}, copies{other.copies}, moves{other.moves}
  {
    if (copies != nullptr) {
      ++(*copies);
    }
  }

  constexpr CopyMoveTracker(CopyMoveTracker &&other) noexcept
      : value{other.value}, copies{other.copies}, moves{other.moves}
  {
    if (moves != nullptr) {
      ++(*moves);
    }
  }

  constexpr CopyMoveTracker &operator=(const CopyMoveTracker &) noexcept = default;
  constexpr CopyMoveTracker &operator=(CopyMoveTracker &&) noexcept = default;
};

struct NonMovable
{
  NonMovable() = default;
  NonMovable(const NonMovable &) = delete;
  NonMovable &operator=(const NonMovable &) = delete;
  NonMovable(NonMovable &&) = delete;
  NonMovable &operator=(NonMovable &&) = delete;
};

struct NoTag
{
};

template <class Result, auto Tag = NoTag{}>
struct QualAwareCallable
{
  constexpr Result operator()(int v) &
  {
    if constexpr (std::is_same_v<decltype(Tag), NoTag>) {
      return Result{v + 1};
    } else {
      return Result{Tag, v + 1};
    }
  }

  constexpr Result operator()(int v) const &
  {
    if constexpr (std::is_same_v<decltype(Tag), NoTag>) {
      return Result{v + 2};
    } else {
      return Result{Tag, v + 2};
    }
  }

  constexpr Result operator()(int v) &&
  {
    if constexpr (std::is_same_v<decltype(Tag), NoTag>) {
      return Result{v + 3};
    } else {
      return Result{Tag, v + 3};
    }
  }

  constexpr Result operator()(int v) const &&
  {
    if constexpr (std::is_same_v<decltype(Tag), NoTag>) {
      return Result{v + 4};
    } else {
      return Result{Tag, v + 4};
    }
  }
};

template <class Result, auto Tag = NoTag{}>
struct MoveOnlyCallable
{
  std::unique_ptr<int> factor;

  constexpr explicit MoveOnlyCallable(int f) : factor{std::make_unique<int>(f)} {}
  MoveOnlyCallable(const MoveOnlyCallable &) = delete;
  MoveOnlyCallable &operator=(const MoveOnlyCallable &) = delete;
  constexpr MoveOnlyCallable(MoveOnlyCallable &&) noexcept = default;
  constexpr MoveOnlyCallable &operator=(MoveOnlyCallable &&) noexcept = default;

  constexpr Result operator()(int v) &&
  {
    if constexpr (std::is_same_v<decltype(Tag), NoTag>) {
      return Result{v * (*factor)};
    } else {
      return Result{Tag, v * (*factor)};
    }
  }
};

} // namespace test::helpers

template <>
struct std::hash<test::helpers::Hashable>
{
  constexpr std::size_t operator()(const test::helpers::Hashable &h) const noexcept { return h.v_; }
};
