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

template <class L>
struct meta_either<optional<L>>
{
  using Self = optional<L>;

  using right_t = Self::value_type;
  using left_t = void;

  template <class U>
  using rebind_right = optional<U>;

  template <class>
  using rebind_left = optional<L>;

  static constexpr auto left = []<std::same_as<Self> T>(T &&) { return void_val; };
  static constexpr auto right = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).value();
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&) { return Self{}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{in_place_opt, std::forward<U>(val)}; };
};

template <class C, class U>
concept HasPushBack = requires(C c, U u) { c.push_back(u); };

template <class C, class U>
concept HasEmplaceBack = requires(C c, U u) { c.emplace_back(u); };

template <class C, class U>
concept HasInsert = requires(C c, U u) { c.insert(u); };

template <class C>
concept HasReserve = requires(C c, std::size_t n) { c.reserve(n); };

template <class C, class E>
struct meta_container
{
  static_assert(
      HasPushBack<C, E> || HasEmplaceBack<C, E> || HasInsert<C, E>,
      "The result container must support push_back, or emplace_back, or insert interface for adding elements. To "
      "support alternative interface, provide a frt::meta_container specialization for your type, please.");

  static constexpr auto add_element = []<std::convertible_to<E> ElemT>(C &c, ElemT &&e) {
    if constexpr (HasPushBack<C, ElemT>)
      c.push_back(std::forward<ElemT>(e));
    else if constexpr (HasEmplaceBack<C, ElemT>)
      c.emplace_back(std::forward<ElemT>(e));
    else if constexpr (HasInsert<C, ElemT>)
      c.insert(std::forward<ElemT>(e));
  };

  static constexpr auto reserve = [](C &c, std::size_t n) {
    if constexpr (HasReserve<C>) {
      c.reserve(n);
    }
  };
};

template <class C, class E>
concept MetaContainerHasAddElement = requires(meta_container<C, E> mc, C c, E e) { mc.add_element(c, e); };

template <class C, class E>
concept MetaContainerHasReserve = requires(meta_container<C, E> mc, C c, std::size_t n) { mc.reserve(c, n); };

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
  static_assert(std::move_constructible<bare_t<func_ret_result_t>>,
                "The return type of te function must be move constructible.");

  using meta_t = meta_either<bare_t<func_ret_result_t>>;

  using func_ret_right_t = meta_t::right_t;
  static_assert(IsNotVoid<func_ret_right_t>,
                "The type of the right alternative of the function return type must not be void.");

  // FIXME: doesn't work for map-like
  using container_t = OutputContainer<func_ret_right_t>;
  static_assert(std::move_constructible<container_t>, "The result container must be move constructible.");

  using output_t = meta_t::template rebind_right<container_t>;
  using output_meta_t = meta_either<output_t>;

  using meta_container_t = meta_container<container_t, func_ret_right_t>;
  static_assert(MetaContainerHasAddElement<container_t, func_ret_right_t>,
                "The ftr::meta_container must provide an add_element interface.");

  container_t resultContainer;
  if constexpr (std::ranges::sized_range<container_t> && MetaContainerHasReserve<container_t, func_ret_right_t>) {
    meta_container_t{}.reserve(resultContainer, std::ranges::size(inputRange));
  }

  for (auto &&v : std::forward<InputRange>(inputRange)) {
    if (auto res = std::invoke(f, std::forward<decltype(v)>(v)); meta_t{}.is_right(res)) {
      meta_container_t{}.add_element(resultContainer, meta_t{}.right(std::move(res)));
    } else {
      return output_meta_t{}.make_left(meta_t{}.left(std::move(res)));
    }
  }

  return output_meta_t{}.make_right(std::move(resultContainer));
}

} // namespace detail::trav_alg_

} // namespace frt
