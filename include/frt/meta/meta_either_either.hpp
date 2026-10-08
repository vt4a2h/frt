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

#include <frt/concept/concept.hpp>
#include <frt/either.hpp>
#include <frt/meta/meta_either.hpp>

namespace frt
{
template <class L, class R>
inline constexpr bool is_result_type<either<L, R>> = true;

//! The \c frt::meta_either interface implementation for \c frt::either.implementation for \c frt::either.
template <class L, class R>
struct meta_either<either<L, R>>
{
  using Self = either<L, R>;

  using left_t = Self::left_t;
  using right_t = Self::right_t;

  template <class U>
  using rebind_left = Self::template rebind_left<U>;

  template <class U>
  using rebind_right = Self::template rebind_right<U>;

  static constexpr auto left = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).left();
  };
  static constexpr auto right = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).right();
  };

  static constexpr auto is_left = [](const Self &self) { return self.is_left(); };
  static constexpr auto is_right = [](const Self &self) { return self.is_right(); };

  static constexpr auto make_left = []<class U>(U &&val) { return Self{in_place_left, std::forward<U>(val)}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{in_place_right, std::forward<U>(val)}; };
};

} // namespace frt
