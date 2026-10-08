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

#include <frt/format/std_fmt_either.hpp>

#include <frt/expected.hpp>

//! Formatter for `frt::expected<T, E>`.
template <frt::detail::common_::FormattableAlternative<char> Value,
          frt::detail::common_::FormattableAlternative<char> Error>
struct std::formatter<frt::expected<Value, Error>, char> : private std::formatter<std::string>
{
  template <class ParseContext>
  constexpr auto parse(ParseContext &ctx)
  {
    return std::formatter<std::string>::parse(ctx);
  }

  template <class FmtContext>
  constexpr auto format(const frt::expected<Value, Error> &e, FmtContext &ctx) const
  {
    if (e.has_value()) {
      if constexpr (frt::detail::common_::IsVoid<Value>) {
        return formatter<string>::format("expected{value: void}", ctx);
      } else {
        return formatter<string>::format(std::format("expected{{value: {}}}", e.value()), ctx);
      }
    } else {
      return formatter<string>::format(std::format("expected{{error: {}}}", e.error()), ctx);
    }
  }
};
