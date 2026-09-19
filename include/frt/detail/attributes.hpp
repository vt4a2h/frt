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

#ifdef FRT_OPTIMIZE_EXECUTION_PATH
#define FRT_LIKELY [[likely]]
#define FRT_UNLIKELY [[unlikely]]
#else
#define FRT_LIKELY
#define FRT_UNLIKELY
#endif
