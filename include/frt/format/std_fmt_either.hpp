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

#include <frt/either.hpp>

namespace frt::detail::common_
{
template <class T, class CharT>
concept FormattableAlternative = IsVoid<T> || std::formattable<T, CharT>;
} // namespace frt::detail::common_

//! Formatter for `frt::either<L, R>`.
template <frt::detail::common_::FormattableAlternative<char> Left,
          frt::detail::common_::FormattableAlternative<char> Right>
struct std::formatter<frt::either<Left, Right>, char> : private std::formatter<std::string>
{
  template <class ParseContext>
  constexpr auto parse(ParseContext &ctx)
  {
    return std::formatter<std::string>::parse(ctx);
  }

  template <class FmtContext>
  constexpr auto format(const frt::either<Left, Right> &e, FmtContext &ctx) const
  {
    if (e.is_left()) {
      if constexpr (frt::detail::either_::IsVoid<Left>) {
        return formatter<string>::format("either{left: void}", ctx);
      } else {
        return formatter<string>::format(std::format("either{{left: {}}}", e.left()), ctx);
      }
    } else {
      if constexpr (frt::detail::either_::IsVoid<Right>) {
        return formatter<string>::format("either{right: void}", ctx);
      } else {
        return formatter<string>::format(std::format("either{{right: {}}}", e.right()), ctx);
      }
    }
  }
};
