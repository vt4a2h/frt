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

namespace frt
{

/*!
 * \brief An interface for working with a custom type as if it were an Either monad.
 *
 * You can provide the following parts:
 * \code{.cpp}
 *
 * // A type of the left alternative
 * using left_t = ...;
 *
 * // A type of the right alternative
 * using right_t = ...;
 *
 * // Change a type of the left alternative
 * template <class U>
 * using rebind_left = ...;
 *
 * // Change a type of the right alternative
 * template <class U>
 * using rebind_right = ...;
 *
 * // A unique non-template type to identify the meta object
 * using tag_t = ...;
 *
 * // A function to access the left alternative
 * // f: T&& -> left_t&&
 * static constexpr auto left = ...;
 *
 * // A function to access the right alternative
 * // f: T&& -> right_t&&
 * static constexpr auto right = ...;
 *
 * // A function to check whether the object contains a left alternative:
 * // f: const T& -> bool
 * static constexpr auto is_left = ...;
 *
 * // A function to check whether the object contains a right alternative:
 * // f: const T& -> bool
 * static constexpr auto is_right = ...;
 *
 * // A function to create an object from the left alternative
 * // f: U&& -> T
 * static constexpr auto make_left = ...;
 *
 * // A function to create an object from the right alternative
 * // f: U&& -> T
 * static constexpr auto make_right = ...;
 * \endcode
 *
 * Both frt and std result types are used via this interface. See their implementations as an example.
 */
template <class...>
struct meta_either
{
  static_assert(false, "The result type must be representable as Either. Provide a frt::meta_either specialization for "
                       "your type, please.");
};

} // namespace frt
