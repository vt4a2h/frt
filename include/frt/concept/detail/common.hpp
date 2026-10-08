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

#include <type_traits>

namespace frt::detail::common_
{

template <class T>
using bare_t = std::remove_cvref_t<T>;

template <class T>
concept IsVoid = std::is_void_v<bare_t<T>>;

template <class T>
concept IsNotVoid = !IsVoid<T>;

} // namespace frt::detail::common_
