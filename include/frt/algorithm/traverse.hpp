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

template <class>
struct meta_either
{
  // TODO: always false
};

template <class L, class R>
struct meta_either<either<L, R>>
{
  using Self = either<L, R>;

  using right_t = Self::right_t;
  using left_t = Self::left_t;

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

  using right_t = Self::value_type;
  using left_t = Self::error_type;

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

template <class E>
struct meta_either<expected<void, E>>
{
  using Self = expected<void, E>;

  using right_t = Self::value_type;
  using left_t = void;

  template <class U>
  using rebind_right = Self::template rebind_value_type<U>;

  static constexpr auto left = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).error();
  };
  static constexpr auto right = [] {};

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&val) { return Self{in_place_unexpected, std::forward<U>(val)}; };
  static constexpr auto make_right = [] {};
};

template <class L>
struct meta_either<optional<L>>
{
  using Self = optional<L>;

  using right_t = Self::value_type;
  using left_t = void;

  template <class U>
  using rebind_right = optional<U>;

  static constexpr auto left = [] {};
  static constexpr auto right = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).value();
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = [] { return Self{}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{in_place_opt, std::forward<U>(val)}; };
};

namespace detail::trav_alg_
{

using namespace detail::common_;

template <template <typename...> class OutputContainer = std::vector, std::ranges::input_range InputRange, class F>
[[nodiscard]] constexpr decltype(auto) traverse(InputRange &&inputRange, F f) noexcept
{
  using range_val_t = std::ranges::range_value_t<InputRange>;
  static_assert(IsResultType<bare_t<range_val_t>>, "The input range must contain values of the result type.  "
                                                   "Specialize frt::is_result_type for your type as needed.");

  using func_ret_result_t = std::invoke_result_t<F, range_val_t>;
  static_assert(
      IsResultType<bare_t<func_ret_result_t>>,
      "The return type of te function must be a result type. Specialize frt::is_result_type for your type as needed.");

  using meta_t = meta_either<bare_t<func_ret_result_t>>;

  using func_ret_right_t = meta_t::right_t;
  static_assert(IsNotVoid<func_ret_right_t>,
                "The type of the right alternative of the function return type must not be void.");
  using container_t = OutputContainer<func_ret_right_t>;

  using output_t = meta_t::template rebind_right<container_t>;
  using output_meta_t = meta_either<output_t>;

  container_t resultContainer;
  // TODO: if supports reserve
  // TODO: if sized range
  resultContainer.reserve(std::ranges::size(inputRange));

  for (auto &&v : std::forward<InputRange>(inputRange)) {
    if (auto res = std::invoke(f, std::forward<decltype(v)>(v)); meta_t{}.is_right(res)) {
      // TODO: if has emplace
      resultContainer.emplace_back(meta_t{}.right(std::move(res)));
    } else {
      if constexpr (IsNotVoid<typename meta_t::left_t>) {
        return output_meta_t{}.make_left(meta_t{}.left(std::move(res)));
      } else {
        return output_meta_t{}.make_left();
      }
    }
  }

  // TODO: if movable
  return output_meta_t{}.make_right(std::move(resultContainer));
}

} // namespace detail::trav_alg_

} // namespace frt
