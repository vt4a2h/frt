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

#include <frt/util/unwrapped.hpp>

namespace frt::detail::common_
{

template <class T>
using bare_t = std::remove_cvref_t<T>;

template <class T>
concept IsVoid = std::is_void_v<bare_t<T>>;

template <class T>
concept IsNotVoid = !IsVoid<T>;

template <class... Args>
constexpr inline std::size_t unwrapped_count = (std::size_t{} + ... + static_cast<std::size_t>(IsUnwrapped<Args>));

template <class... Args>
constexpr inline bool single_arg_is_unwrapped = sizeof...(Args) == 1 && unwrapped_count<Args...> == 1;

template <class... Args>
constexpr inline bool contains_zero_or_one_unwrapped = unwrapped_count<Args...> <= 1;

template <class... Args>
inline constexpr bool contains_unwrapped = (... || IsUnwrapped<Args>);

template <class F, class T, class... Args>
constexpr inline bool is_invocable_with_extra_args = std::is_invocable_v<F, T, Args...>;

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
constexpr inline bool is_invocable_with_extra_args<F, T, Args...> =
    std::is_invocable_v<F, std::conditional_t<IsUnwrapped<Args>, T, Args>...>;

template <class F, class T, class... Args>
constexpr inline bool is_nothrow_invocable_with_extra_args = std::is_nothrow_invocable_v<F, T, Args...>;

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
constexpr inline bool is_nothrow_invocable_with_extra_args<F, T, Args...> =
    std::is_nothrow_invocable_v<F, std::conditional_t<IsUnwrapped<Args>, T, Args>...>;

template <class F, class T, class... Args>
struct invoke_result
{
  using type = std::invoke_result_t<F, T, Args...>;
};

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
struct invoke_result<F, T, Args...>
{
  static_assert(is_invocable_with_extra_args<F, T, Args...>);

  using type = std::invoke_result_t<F, std::conditional_t<IsUnwrapped<Args>, T, Args>...>;
};

template <class F, class T, class... Args>
using invoke_result_t = invoke_result<F, T, Args...>::type;

template <class T, class Arg>
constexpr auto &&forward(T &&t, Arg &&arg) noexcept
{
  // We can have only 0 or 1 unwrapped argument. Hence, it will be forwarded only once
  if constexpr (IsUnwrapped<Arg>) {
    return std::forward<T>(t);
  } else {
    return std::forward<Arg>(arg);
  }
}

template <class F, class T, class... Args>
[[nodiscard]] constexpr auto
invoke_with_extra_args(F &&f, T &&t, Args &&...args) noexcept(is_nothrow_invocable_with_extra_args<F, T, Args...>)
{
  return std::invoke(f, std::forward<T>(t), std::forward<Args>(args)...);
}

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
[[nodiscard]] constexpr auto
invoke_with_extra_args(F &&f, T &&t, Args &&...args) noexcept(is_nothrow_invocable_with_extra_args<F, T, Args...>)
{
  return std::invoke(f, forward(std::forward<T>(t), std::forward<Args>(args))...);
}

} // namespace frt::detail::common_
