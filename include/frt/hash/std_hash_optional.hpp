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

#include <frt/optional.hpp>

#include <frt/hash/std_hash_either.hpp>

//! Hasher for `frt::optional<T>`.
template <class T>
  requires(frt::detail::common_::IsHashable<T>)
struct std::hash<frt::optional<T>>
{
  constexpr std::size_t operator()(const frt::optional<T> &opt) const noexcept
  {
    if (opt.has_value()) {
      return std::hash<frt::detail::common_::bare_t<T>>{}(opt.value());
    } else {
      return std::hash<frt::detail::either_::Void>{}(frt::void_val);
    }
  }
};
