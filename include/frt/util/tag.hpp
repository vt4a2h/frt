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

namespace frt
{

namespace detail::tag_
{
struct InPlaceLeftTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceLeftTag(Secret) noexcept {}
};

struct InPlaceRightTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceRightTag(Secret) noexcept {}
};
} // namespace detail::tag_

using in_place_left_t = detail::tag_::InPlaceLeftTag;
using in_place_right_t = detail::tag_::InPlaceRightTag;

inline constexpr in_place_left_t in_place_left{in_place_left_t::Secret{}};
inline constexpr in_place_right_t in_place_right{in_place_right_t::Secret{}};

} // namespace frt
