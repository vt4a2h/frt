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
#include <frt/expected.hpp>
#include <frt/meta/meta_either.hpp>

namespace frt
{

template <class T, class E>
inline constexpr bool is_result_type<expected<T, E>> = true;

//! The \c frt::meta_either interface implementation for \c frt::expected.
template <class T, class E>
struct meta_either<expected<T, E>>
{
  using Self = expected<T, E>;

  using left_t = Self::error_type;
  using right_t = Self::value_type;

  template <class U>
  using rebind_left = Self::template rebind_error_type<U>;

  template <class U>
  using rebind_right = Self::template rebind_value_type<U>;

  static constexpr auto left = []<std::same_as<Self> S>(S &&self) -> decltype(auto) {
    return std::forward<S>(self).error();
  };
  static constexpr auto right = []<std::same_as<Self> S>(S &&self) -> decltype(auto) {
    return std::forward<S>(self).value();
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&val) { return Self{in_place_unexpected, std::forward<U>(val)}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{in_place_expected, std::forward<U>(val)}; };
};

} // namespace frt
