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
  static_assert(false, "The result type must be representable as Either. Provide a frt::meta_either specialization for "
                       "your type, please.");
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

namespace detail::trav_alg_
{

template <class C, class U>
concept HasPushBack = requires(C c, U u) { c.push_back(u); };

template <class C, class U>
concept HasEmplaceBack = requires(C c, U u) { c.emplace_back(u); };

template <class C, class U>
concept HasInsert = requires(C c, U u) { c.insert(u); };

template <class C, class U>
concept HasEmplace = requires(C c, U u) { c.emplace(u); };

template <class C>
concept HasReserve = requires(C c, std::size_t n) { c.reserve(n); };

template <class C, class E>
  requires(HasPushBack<C, E> && !HasEmplaceBack<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.push_back(std::forward<E>(e));
}

// Prefer emplace_back over push_back for the sequential containers
template <class C, class E>
  requires(HasEmplaceBack<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.emplace_back(std::forward<E>(e));
}

// Prefer emplace over insert for the associative containers
template <class C, class E>
  requires(HasInsert<C, E> && !HasEmplace<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.insert(std::forward<E>(e));
}

template <class C, class E>
  requires(HasEmplace<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.emplace(std::forward<E>(e));
}

template <class C, class E>
void add_elem_impl(C &, E &&)
{
  static_assert(
      false, "The result container must support push_back, or emplace_back, or insert interface for adding elements. "
             "To support alternative interface, provide a frt::meta_container specialization for your type, please.");
}

template <HasReserve C>
void reserve_impl(C &c, std::size_t n)
{
  c.reserve(n);
}

template <class C>
void reserve_impl(C &, std::size_t)
{
  // Do nothing if we cannot reserve
}

} // namespace detail::trav_alg_

template <class C>
struct meta_container
{
  using E = std::ranges::range_value_t<C>;

  static constexpr auto add_element = []<std::convertible_to<E> ElemT>(C &c, ElemT &&e) {
    detail::trav_alg_::add_elem_impl(c, std::forward<ElemT>(e));
  };

  static constexpr auto reserve = [](C &c, std::size_t n) { detail::trav_alg_::reserve_impl(c, n); };
};

template <class C>
concept MetaContainerHasAddElement =
    requires(meta_container<C> mc, C c, std::ranges::range_value_t<C> e) { mc.add_element(c, e); };

template <class C>
concept MetaContainerHasReserve = requires(meta_container<C> mc, C c, std::size_t n) { mc.reserve(c, n); };

namespace detail::trav_alg_
{

using namespace detail::common_;

template <template <class...> class, class>
struct select_container
{
  static_assert(false, "Cannot select a container for the given type.");
};

// A sequential container of any elements (possibly pairs)
template <template <class...> class Container, class ElemT>
  requires(requires { typename Container<ElemT>; })
struct select_container<Container, ElemT>
{
  using type = Container<ElemT>;
};

// An associative container of any keys and values. The element type must be a pair-like (not a tuple-like).
template <template <class...> class Container, class ElemT>
  requires(requires {
    typename ElemT::first_type;
    typename ElemT::second_type;
    typename Container<typename ElemT::first_type, typename ElemT::second_type>;
  })
struct select_container<Container, ElemT>
{
  using type = Container<typename ElemT::first_type, typename ElemT::second_type>;
};

template <template <class...> class Container, class ElemT>
using select_container_t = select_container<Container, ElemT>::type;

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

  using container_t = select_container_t<OutputContainer, func_ret_right_t>;
  static_assert(std::move_constructible<container_t>, "The result container must be move constructible.");

  using output_t = meta_t::template rebind_right<container_t>;
  using output_meta_t = meta_either<output_t>;

  using meta_container_t = meta_container<container_t>;
  static_assert(MetaContainerHasAddElement<container_t>,
                "The ftr::meta_container must provide an add_element interface.");

  container_t resultContainer;
  if constexpr (std::ranges::sized_range<container_t> && MetaContainerHasReserve<container_t>) {
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
