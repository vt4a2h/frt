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

#include <frt/meta/meta_container.hpp>
#include <frt/meta/meta_either.hpp>

namespace frt
{

namespace detail::trav_alg_
{

template <template <typename...> class OutputContainer = std::vector, std::ranges::input_range InputRange, class F>
[[nodiscard]] constexpr decltype(auto) traverse(InputRange &&inputRange, F f) noexcept
{
  using namespace detail::common_;

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

  using container_t = meta_container_::select_container_t<OutputContainer, func_ret_right_t>;
  static_assert(std::move_constructible<container_t>, "The result container must be move constructible.");

  using output_t = meta_t::template rebind_right<container_t>;
  using output_meta_t = meta_either<output_t>;

  using meta_container_t = meta_container<container_t>;
  static_assert(meta_container_::MetaContainerHasAddElement<container_t>,
                "The ftr::meta_container must provide an add_element interface.");

  container_t resultContainer;
  if constexpr (std::ranges::sized_range<container_t> && meta_container_::MetaContainerHasReserve<container_t>) {
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

template <class F>
struct traverse_closure_fn
{
  F f_;

  template <std::ranges::input_range R>
  friend constexpr decltype(auto) operator|(R &&r, const traverse_closure_fn &c) noexcept
  {
    return traverse<std::vector>(std::forward<R>(r), c.f_);
  }
};

struct traverse_fn
{
  template <std::ranges::input_range InputRange, class F>
  [[nodiscard]] constexpr decltype(auto) operator()(InputRange &&inputRange, F f) const noexcept
  {
    return traverse<std::vector>(std::forward<InputRange>(inputRange), std::forward<F>(f));
  }

  template <class F>
  [[nodiscard]] constexpr decltype(auto) operator()(F f) const noexcept
  {
    return traverse_closure_fn{.f_ = f};
  }
};

template <template <typename...> class OutputContainer, class F>
struct traverse_into_closure_fn
{
  F f_;

  template <std::ranges::input_range R>
  friend constexpr decltype(auto) operator|(R &&r, const traverse_into_closure_fn &c) noexcept
  {
    return traverse<OutputContainer>(std::forward<R>(r), c.f_);
  }
};

template <template <typename...> class OutputContainer>
struct traverse_into_fn
{
  template <std::ranges::input_range InputRange, class F>
  [[nodiscard]] constexpr decltype(auto) operator()(InputRange &&inputRange, F f) const noexcept
  {
    return traverse<OutputContainer>(std::forward<InputRange>(inputRange), std::forward<F>(f));
  }

  template <class F>
  [[nodiscard]] constexpr decltype(auto) operator()(F f) const noexcept
  {
    return traverse_into_closure_fn<OutputContainer, F>{.f_ = f};
  }
};

} // namespace detail::trav_alg_

//! The traverse algorithm. TODO: add better doc
inline constexpr detail::trav_alg_::traverse_fn traverse{};

//! The traverse algorithm with the possibility of specifying an output container. TODO: add better doc
template <template <typename...> class OutputContainer>
inline constexpr detail::trav_alg_::traverse_into_fn<OutputContainer> traverse_into{};

} // namespace frt
