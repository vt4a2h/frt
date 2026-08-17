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

#ifdef FRT_HARDENED

#include <concepts>
#include <cstdio>
#include <exception>
#include <print>
#include <source_location>
#include <string_view>

namespace frt::detail
{

template <class B>
concept bool_testable = requires(const B &b) {
  { !!b } -> std::same_as<bool>;
};

template <bool_testable Condition>
constexpr void expects(const Condition &condition, std::string_view message = "",
                       [[maybe_unused]] std::source_location loc = std::source_location::current()) noexcept
{
  if (condition) [[likely]] {
    return;
  }

  if consteval {
    try {
      throw "Expected to be true";
    } catch (...) {
      auto eptr = std::current_exception();
      std::rethrow_exception(eptr);
    }
  } else {
    std::println(stderr, "Expected to be true: {} ({} {}:{}:{})", message, loc.function_name(), loc.file_name(),
                 loc.line(), loc.column());
    std::abort();
  }
}

} // namespace frt::detail

#define FRT_INTERNAL_ASSERT(condition, ...)                                                                            \
  do {                                                                                                                 \
    frt::detail::expects(condition __VA_OPT__(, ) __VA_ARGS__);                                                        \
  } while (false)
#else
#define FRT_INTERNAL_ASSERT(condition, ...)                                                                            \
  do {                                                                                                                 \
    (void)sizeof(condition);                                                                                           \
  } while (false)
#endif
