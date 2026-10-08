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

#include <frt/optional.hpp>

//! Formatter for `frt::optional<T>`.
template <frt::detail::common_::FormattableAlternative<char> Value>
struct std::formatter<frt::optional<Value>, char> : private std::formatter<std::string>
{
  template <class ParseContext>
  constexpr auto parse(ParseContext &ctx)
  {
    return std::formatter<std::string>::parse(ctx);
  }

  template <class FmtContext>
  constexpr auto format(const frt::optional<Value> &opt, FmtContext &ctx) const
  {
    if (opt.has_value()) {
      return formatter<string>::format(std::format("optional{{{}}}", opt.value()), ctx);
    } else {
      return formatter<string>::format(std::format("optional{{nullopt}}"), ctx);
    }
  }
};
