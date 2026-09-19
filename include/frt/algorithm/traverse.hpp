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

namespace frt
{

namespace detail::trav_alg_
{

using namespace detail::common_;

template <template <typename...> class OutputContainer = std::vector, std::ranges::input_range InputRange, class F>
[[nodiscard]] constexpr decltype(auto) traverse(InputRange &&inputRange, F f) noexcept
{
  using range_val_t = std::ranges::range_value_t<InputRange>;
  using func_ret_result_t = std::invoke_result_t<F, range_val_t>;
  using func_ret_right_t = bare_t<func_ret_result_t>::right_t;
  using container_t = OutputContainer<func_ret_right_t>;
  using output_t = bare_t<func_ret_result_t>::template rebind_right<container_t>;

  container_t resultContainer;
  // note: if supports reserve
  // note: if sized range
  resultContainer.reserve(std::ranges::size(inputRange));

  for (auto &&v : std::forward<InputRange>(inputRange)) {
    if (auto res = std::invoke(f, std::forward<decltype(v)>(v)); res.is_right()) {
      // note if has emplace
      resultContainer.emplace_back(std::move(res).right());
    } else {
      return output_t{in_place_left, std::move(res).left()};
    }
  }

  // note if movable
  return output_t{in_place_right, std::move(resultContainer)};
}

} // namespace detail::trav_alg_

} // namespace frt
