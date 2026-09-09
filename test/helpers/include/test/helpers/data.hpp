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

#include <string>
#include <vector>

namespace test::helpers
{

inline constexpr int expectedIntValue = 42;
inline constexpr double expectedDoubleValue = 42.0;

inline const std::string expectedStringValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";

inline const std::vector expectedVectorValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

constexpr auto makeExpectedInt() { return 42; }

constexpr auto makeExpectedDouble() { return 42.0; }

constexpr auto makeExpectedString()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

constexpr auto makeExpectedVector() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }

} // namespace test::helpers
