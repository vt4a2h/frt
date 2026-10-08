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

#include <frt/either.hpp>
#include <frt/expected.hpp>
#include <frt/optional.hpp>

namespace frt
{
template <class T>
inline constexpr bool is_result_type = false;

template <class L, class R>
inline constexpr bool is_result_type<either<L, R>> = true;

template <class T, class E>
inline constexpr bool is_result_type<expected<T, E>> = true;

template <class T>
inline constexpr bool is_result_type<optional<T>> = true;

template <class T>
concept IsResultType = is_result_type<detail::common_::bare_t<T>>;

/*!
 * \brief An interface for working with a custom type as if it were an Either monad.
 *
 * You can provide the following parts:
 * \code{.cpp}
 *
 * // A type of the left alternative
 * using left_t = ...;
 *
 * // A type of the right alternative
 * using right_t = ...;
 *
 * // Change a type of the left alternative
 * template <class U>
 * using rebind_left = ...;
 *
 * // Change a type of the right alternative
 * template <class U>
 * using rebind_right = ...;
 *
 * // A function to access the left alternative
 * // f: T&& -> left_t&&
 * static constexpr auto left = ...;
 *
 * // A function to access the right alternative
 * // f: T&& -> right_t&&
 * static constexpr auto right = ...;
 *
 * // A function to check whether the object contains a left alternative:
 * // f: const T& -> bool
 * static constexpr auto is_left = ...;
 *
 * // A function to check whether the object contains a right alternative:
 * // f: const T& -> bool
 * static constexpr auto is_right = ...;
 *
 * // A function to create an object from the left alternative
 * // f: U&& -> T
 * static constexpr auto make_left = ...;
 *
 * // A function to create an object from the right alternative
 * // f: U&& -> T
 * static constexpr auto make_right = ...;
 * \endcode
 *
 * Both frt and std result types are used via this interface. See their implementations as an example.
 */
template <class>
struct meta_either
{
  static_assert(false, "The result type must be representable as Either. Provide a frt::meta_either specialization for "
                       "your type, please.");
};

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
