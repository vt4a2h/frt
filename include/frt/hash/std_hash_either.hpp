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

namespace frt::detail::common_
{

inline constexpr std::size_t void_hash{};

template <class T>
concept IsHashable = requires(T v) {
  { std::hash<T>{}(v) } -> std::same_as<std::size_t>;
};

} // namespace frt::detail::common_

//! Hasher for `frt::void_t`.
template <>
struct std::hash<frt::detail::either_::Void>
{
  constexpr std::size_t operator()(const frt::detail::either_::Void &) const noexcept
  {
    return frt::detail::either_::void_hash;
  }
};

//! Hasher for `frt::either<L, R>`. The result is either hash of the left or of the right alternative.
template <class Left, class Right>
  requires(frt::detail::common_::IsHashable<frt::detail::common_::SelectedType<Left>> &&
           frt::detail::common_::IsHashable<frt::detail::common_::SelectedType<Right>>)
struct std::hash<frt::either<Left, Right>>
{
  constexpr std::size_t operator()(const frt::either<Left, Right> &e) const noexcept
  {
    if (e.is_left()) {
      return std::hash<frt::detail::either_::bare_t<Left>>{}(e.left());
    } else {
      return std::hash<frt::detail::either_::bare_t<Right>>{}(e.right());
    }
  }
};
