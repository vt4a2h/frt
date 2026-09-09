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

#include <catch2/catch_all.hpp>
#include <frt/expected.hpp>
#include <test/helpers/data.hpp>
#include <test/helpers/types.hpp>

namespace test::expected
{
using namespace test::helpers;
using namespace std::literals;

using Expected = frt::expected<std::vector<int>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

inline const auto &expectedValue = expectedVectorValue;
inline const auto &expectedError = expectedStringValue;

constexpr auto makeExpectedValue() { return makeExpectedVector(); }
constexpr auto makeExpectedError() { return makeExpectedString(); }
} // namespace test::expected
