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

#include <compare>

namespace frt
{

namespace detail::common_
{

struct Void
{
  friend constexpr bool operator==(Void, Void) noexcept = default;
  friend constexpr std::partial_ordering operator<=>(Void, Void) noexcept = default;
};

} // namespace detail::common_

//! Void as a regular type.
using void_t = detail::common_::Void;

//! Pre-defined value of the \c frt::void_t type.
inline constexpr detail::common_::Void void_val;

} // namespace frt
