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

#include <frt/concept/concept.hpp>
#include <frt/meta/meta_either.hpp>
#include <frt/util/void.hpp>

#include <optional>

namespace frt
{

template <class T>
inline constexpr bool is_result_type<std::optional<T>> = true;

//! The \c frt::meta_either interface implementation for \c std::optional.
template <class L>
struct meta_either<std::optional<L>>
{
  using Self = std::optional<L>;

  using right_t = Self::value_type;
  using left_t = void_t;

  template <class U>
  using rebind_right = std::optional<U>;

  template <class>
  using rebind_left = std::optional<L>;

  static constexpr auto left = []<std::same_as<Self> T>(T &&) { return void_val; };
  static constexpr auto right = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).value();
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&) { return Self{}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{std::in_place, std::forward<U>(val)}; };
};

} // namespace frt
