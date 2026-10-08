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
#include <frt/meta/meta_either.hpp>
#include <frt/optional.hpp>

namespace frt
{

//! The \c frt::meta_either interface implementation for \c frt::optional.
template <class T>
inline constexpr bool is_result_type<optional<T>> = true;

template <class L>
struct meta_either<optional<L>>
{
  using Self = optional<L>;

  using left_t = void_t;
  using right_t = Self::value_type;

  template <class>
  using rebind_left = optional<L>;

  template <class U>
  using rebind_right = optional<U>;

  static constexpr auto left = []<std::same_as<Self> T>(T &&) { return void_val; };
  static constexpr auto right = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).value();
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&) { return Self{}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{in_place_opt, std::forward<U>(val)}; };
};

} // namespace frt
