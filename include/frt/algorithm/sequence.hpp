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

#include <frt/algorithm/traverse.hpp>

#include <ranges>
#include <vector>

namespace frt
{

namespace detail::sequence_alg_
{

struct sequence_fn : std::ranges::range_adaptor_closure<sequence_fn>
{
  template <std::ranges::input_range InputRange>
  [[nodiscard]] constexpr decltype(auto) operator()(InputRange &&inputRange) const noexcept
  {
    return trav_alg_::traverse<std::vector>(std::forward<InputRange>(inputRange), std::identity{});
  }
};

template <template <typename...> class OutputContainer>
struct sequence_into_fn : std::ranges::range_adaptor_closure<sequence_into_fn<OutputContainer>>
{
  template <std::ranges::input_range InputRange>
  [[nodiscard]] constexpr decltype(auto) operator()(InputRange &&inputRange) const noexcept
  {
    return trav_alg_::traverse<OutputContainer>(std::forward<InputRange>(inputRange), std::identity{});
  }
};

} // namespace detail::sequence_alg_

/*!
 * \brief The traverse algorithm with std::identity as a function.
 *
 * Effectively, it's a shortcut used for covering a range of result objects into a result object of range. For example,
 * `vector<optional<T>>` into `optional<vector<T>>`.
 *
 * \see \c frt::traverse for more details.
 */
inline constexpr detail::sequence_alg_::sequence_fn sequence;

/*!
 * \brief The traverse algorithm with std::identity as a function and a customizable output container.
 *
 * Effectively, it's a shortcut used for covering a range of result objects into a result object of range. For example,
 * `vector<optional<T>>` into `optional<std::list<T>>`.
 *
 * \see \c frt::traverse_into for more details.
 */
template <template <typename...> class OutputContainer>
inline constexpr detail::sequence_alg_::sequence_into_fn<OutputContainer> sequence_into;

} // namespace frt
