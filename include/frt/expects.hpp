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

#include <concepts>
#include <print>
#include <source_location>

namespace frt {

namespace detail {

template <class B>
concept bool_testable = requires(const B &b) {
  { !!b } -> std::same_as<bool>;
};

} // namespace detail

template <detail::bool_testable Condition>
constexpr void expects(const Condition &condition,
                       [[maybe_unused]] std::source_location loc = std::source_location::current()) noexcept
{
  if (condition) {
    return;
  }

  if (std::is_constant_evaluated()) {
    try {
      throw "Expected to be true";
    } catch (...) {
      // Workaround to mark the function noexcept
    }
  } else {
    // TODO: make an extension point
    std::println(stderr, "Expected to be true ({} {}:{}:{})", loc.function_name(), loc.file_name(), loc.line(),
                 loc.column());
    std::abort();
  }
}

} // namespace frt
