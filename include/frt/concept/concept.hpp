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

#include <frt/concept/detail/common.hpp>

namespace frt
{
template <class T>
inline constexpr bool is_result_type = false;

template <class T>
concept IsResultType = is_result_type<detail::common_::bare_t<T>>;
} // namespace frt
