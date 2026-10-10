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

namespace detail::unwrapped_
{

struct UnwrappedTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit UnwrappedTag(Secret) noexcept {}
};

} // namespace detail::unwrapped_

using unwrapped_t = detail::unwrapped_::UnwrappedTag;
inline constexpr unwrapped_t unwrapped{unwrapped_t::Secret{}};

template <class T>
concept IsUnwrapped = std::same_as<std::remove_cvref_t<T>, unwrapped_t>;

} // namespace frt
