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

#include <frt/expected.hpp>

#include <frt/hash/std_hash_either.hpp>

//! Hasher for `frt::expected<T, E>`.
template <class T, class E>
  requires(frt::detail::common_::IsHashable<frt::detail::common_::SelectedType<T>> &&
           frt::detail::common_::IsHashable<frt::detail::common_::SelectedType<E>>)
struct std::hash<frt::expected<T, E>>
{
  constexpr std::size_t operator()(const frt::expected<T, E> &e) const noexcept
  {
    if (e.has_value()) {
      if constexpr (frt::detail::common_::IsVoid<T>) {
        return std::hash<frt::detail::either_::Void>{}(frt::void_val);
      } else {
        return std::hash<frt::detail::common_::bare_t<T>>{}(e.value());
      }
    } else {
      return std::hash<frt::detail::common_::bare_t<E>>{}(e.error());
    }
  }
};
