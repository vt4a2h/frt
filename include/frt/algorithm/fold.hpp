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

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

#include <frt/concept/concept.hpp>
#include <frt/concept/detail/common.hpp>
#include <frt/detail/attributes.hpp>
#include <frt/meta/meta_either.hpp>

namespace frt
{

namespace detail::fold_alg_
{

template <class ResT, class MetaEither = meta_either<common_::bare_t<ResT>>>
concept ValidMetaFoldInResult = requires(MetaEither me, ResT &&r) {
  typename MetaEither::left_t;
  typename MetaEither::right_t;

  me.is_left(r);

  me.right(std::forward<ResT>(r));
  me.left(std::forward<ResT>(r));
};

template <class R>
using FwdLeftT = common_::ForwardAlternativeT<R, typename meta_either<common_::bare_t<R>>::left_t>;

template <class R>
using FwdRightT = common_::ForwardAlternativeT<R, typename meta_either<common_::bare_t<R>>::right_t>;

template <class F, class Arg>
concept InvocableNonVoidResult = common_::InvocableV<F, Arg> && !common_::IsVoid<common_::InvocableResultT<F, Arg>>;

template <class F, class Arg>
concept InvocableVoidResult = common_::InvocableV<F, Arg> && common_::IsVoid<common_::InvocableResultT<F, Arg>>;

template <IsResultType Result, class LeftFunc, class RightFunc>
  requires(ValidMetaFoldInResult<Result>)
constexpr decltype(auto) fold(Result &&result, LeftFunc &&lf,
                              RightFunc &&rf) noexcept(common_::NothrowInvocableV<LeftFunc, FwdLeftT<Result>> &&
                                                       common_::NothrowInvocableV<RightFunc, FwdRightT<Result>>)
{
  using namespace detail::common_;

  using meta_t = meta_either<bare_t<Result>>;
  static_assert(ValidMetaFoldInResult<Result>,
                "frt::meta_either for the incoming result type Result must provide the following interface: typename "
                "left_t, typename right_t, is_left(Result{}), right(Result{}), and left(Result{}).");

  using fwd_left_t = FwdLeftT<Result>;
  using fwd_right_t = FwdRightT<Result>;

  static_assert(InvocableV<LeftFunc, fwd_left_t>,
                "The left function must be invocable for the left alternative. If the alternative has type void, "
                "then the left function must take no arguments.");
  static_assert(InvocableV<RightFunc, fwd_right_t>,
                "The right function must be invocable for the right alternative. If the alternative has type void, "
                "then the right function must take no arguments.");
  static_assert(SameReturnType<LeftFunc, fwd_left_t, RightFunc, fwd_right_t>,
                "Both left and right functions must have the same return type.");

  if (meta_t{}.is_left(result))
    FRT_UNLIKELY
    {
      if constexpr (IsVoid<fwd_left_t>) {
        return std::invoke(std::forward<LeftFunc>(lf));
      } else {
        return std::invoke(std::forward<LeftFunc>(lf), meta_t{}.left(std::forward<Result>(result)));
      }
    }
  else {
    if constexpr (IsVoid<fwd_right_t>) {
      return std::invoke(std::forward<RightFunc>(rf));
    } else {
      return std::invoke(std::forward<RightFunc>(rf), meta_t{}.right(std::forward<Result>(result)));
    }
  }
}

template <IsResultType Result, class Func>
  requires(ValidMetaFoldInResult<Result>)
constexpr decltype(auto) fold(Result &&result, Func &&f) noexcept(common_::NothrowInvocableV<Func, FwdLeftT<Result>> &&
                                                                  common_::NothrowInvocableV<Func, FwdRightT<Result>>)
{
  using namespace detail::common_;

  using meta_t = meta_either<bare_t<Result>>;
  static_assert(ValidMetaFoldInResult<Result>,
                "frt::meta_either for the incoming result type Result must provide the following interface: typename "
                "left_t, typename right_t, is_left(Result{}), right(Result{}), and left(Result{}).");

  using fwd_left_t = FwdLeftT<Result>;
  using fwd_right_t = FwdRightT<Result>;

  static_assert(InvocableV<Func, fwd_left_t>,
                "The function must be invocable for the left alternative. If the alternative has type void, "
                "then the function must take no arguments.");
  static_assert(InvocableV<Func, fwd_right_t>,
                "The function must be invocable for the right alternative. If the alternative has type void, "
                "then the function must take no arguments.");
  static_assert(SameReturnType<Func, fwd_left_t, Func, fwd_right_t>,
                "The function must return the same type for both alternatives.");

  if (meta_t{}.is_left(result))
    FRT_UNLIKELY
    {
      if constexpr (IsVoid<fwd_left_t>) {
        return std::invoke(std::forward<Func>(f));
      } else {
        return std::invoke(std::forward<Func>(f), meta_t{}.left(std::forward<Result>(result)));
      }
    }
  else {
    if constexpr (IsVoid<fwd_right_t>) {
      return std::invoke(std::forward<Func>(f));
    } else {
      return std::invoke(std::forward<Func>(f), meta_t{}.right(std::forward<Result>(result)));
    }
  }
}

template <class F>
struct fold_closure_fn
{
  F f_;

  template <IsResultType Result, class Self>
    requires std::same_as<common_::bare_t<Self>, fold_closure_fn>
  friend constexpr decltype(auto)
  operator|(Result &&result, Self &&c) noexcept(std::is_nothrow_invocable_v<common_::FwdT<Self, F>, Result>)
  {
    return std::invoke(std::forward_like<Self>(c.f_), std::forward<Result>(result));
  }
};

struct fold_fn
{
  template <IsResultType Result, class LeftFunc, class RightFunc>
    requires InvocableNonVoidResult<LeftFunc, FwdLeftT<Result>>
  [[nodiscard]] constexpr decltype(auto) operator()(Result &&result, LeftFunc &&lf, RightFunc &&rf) const
      noexcept(noexcept(fold(std::forward<Result>(result), std::forward<LeftFunc>(lf), std::forward<RightFunc>(rf))))
  {
    return fold(std::forward<Result>(result), std::forward<LeftFunc>(lf), std::forward<RightFunc>(rf));
  }

  template <IsResultType Result, class LeftFunc, class RightFunc>
    requires InvocableVoidResult<LeftFunc, FwdLeftT<Result>>
  constexpr void operator()(Result &&result, LeftFunc &&lf, RightFunc &&rf) const
      noexcept(noexcept(fold(std::forward<Result>(result), std::forward<LeftFunc>(lf), std::forward<RightFunc>(rf))))
  {
    fold(std::forward<Result>(result), std::forward<LeftFunc>(lf), std::forward<RightFunc>(rf));
  }

  template <IsResultType Result, class LeftFunc, class RightFunc>
  constexpr decltype(auto) operator()(Result &&result, LeftFunc &&lf, RightFunc &&rf) const
      noexcept(noexcept(fold(std::forward<Result>(result), std::forward<LeftFunc>(lf), std::forward<RightFunc>(rf))))
  {
    return fold(std::forward<Result>(result), std::forward<LeftFunc>(lf), std::forward<RightFunc>(rf));
  }

  template <IsResultType Result, class Func>
    requires InvocableNonVoidResult<Func, FwdLeftT<Result>>
  [[nodiscard]] constexpr decltype(auto) operator()(Result &&result, Func &&f) const
      noexcept(noexcept(fold(std::forward<Result>(result), std::forward<Func>(f))))
  {
    return fold(std::forward<Result>(result), std::forward<Func>(f));
  }

  template <IsResultType Result, class Func>
    requires InvocableVoidResult<Func, FwdLeftT<Result>>
  constexpr void operator()(Result &&result, Func &&f) const
      noexcept(noexcept(fold(std::forward<Result>(result), std::forward<Func>(f))))
  {
    fold(std::forward<Result>(result), std::forward<Func>(f));
  }

  template <IsResultType Result, class Func>
  constexpr decltype(auto) operator()(Result &&result, Func &&f) const
      noexcept(noexcept(fold(std::forward<Result>(result), std::forward<Func>(f))))
  {
    return fold(std::forward<Result>(result), std::forward<Func>(f));
  }

  template <class LeftFunc, class RightFunc>
    requires(!IsResultType<LeftFunc> && std::is_move_constructible_v<std::decay_t<LeftFunc>> &&
             std::is_constructible_v<std::decay_t<LeftFunc>, LeftFunc> &&
             std::is_move_constructible_v<std::decay_t<RightFunc>> &&
             std::is_constructible_v<std::decay_t<RightFunc>, RightFunc>)
  [[nodiscard]] constexpr decltype(auto) operator()(LeftFunc &&lf, RightFunc &&rf) const
      noexcept(std::is_nothrow_constructible_v<std::decay_t<LeftFunc>, LeftFunc> &&
               std::is_nothrow_constructible_v<std::decay_t<RightFunc>, RightFunc>)
  {
    return fold_closure_fn{
        .f_ = [lf = std::forward<LeftFunc>(lf), rf = std::forward<RightFunc>(rf)]<class Self, IsResultType Result>(
                  this Self &&,
                  Result
                      &&result) noexcept(noexcept(fold(std::forward<Result>(result),
                                                       std::declval<common_::FwdT<Self, std::decay_t<LeftFunc>>>(),
                                                       std::declval<common_::FwdT<Self, std::decay_t<RightFunc>>>()))) {
          return fold(std::forward<Result>(result), std::forward_like<Self>(lf), std::forward_like<Self>(rf));
        }};
  }

  template <class Func>
    requires(!IsResultType<Func> && std::is_move_constructible_v<std::decay_t<Func>> &&
             std::is_constructible_v<std::decay_t<Func>, Func>)
  [[nodiscard]] constexpr decltype(auto) operator()(Func &&f) const
      noexcept(std::is_nothrow_constructible_v<std::decay_t<Func>, Func>)
  {
    return fold_closure_fn{
        .f_ =
            [f = std::forward<Func>(f)]<class Self, IsResultType Result>(this Self &&, Result &&result) noexcept(
                noexcept(fold(std::forward<Result>(result), std::declval<common_::FwdT<Self, std::decay_t<Func>>>()))) {
              return fold(std::forward<Result>(result), std::forward_like<Self>(f));
            }};
  }
};

} // namespace detail::fold_alg_

/*!
 * \\brief Invoke \\p lf for the left alternative or \\p rf for the right alternative and return the result.
 *
 * This function works similarly to pattern matching. It takes into account the value category and constness of the
 * result object. It means, for example, that both \\p lf and \\p rf can modify contained values and accept them by
 * rvalue reference.
 *
 * The \\p lf function must be invocable for the left alternative. If the left alternative has type \\c void, then \\p
 * lf must accept no arguments. The \\p rf function must be invocable for the right alternative. If the right
 * alternative has type \\c void, then \\p rf must accept no arguments.
 *
 * Both functions \\p lf and \\p rf must have the same return type.
 *
 * When only a single function \\p f is supplied, it must be invocable for both alternatives and return the same type.
 * This is equivalent to calling \\c fold(result, f, f).
 *
 * \\code{.cpp}
 * // Direct invocation with two functions:
 * auto res = frt::fold(eitherObj,
 *                      [](const Left &l) { return format("left: {}", l); },
 *                      [](const Right &r) { return format("right: {}", r); });
 *
 * // Direct invocation with a single function:
 * auto res = frt::fold(eitherObj, [](const auto &val) { return format("{}", val); });
 * \\endcode
 *
 * Pipe syntax for partial application is also supported:
 * \\code{.cpp}
 * // Two functions
 * auto res1 = eitherObj | frt::fold(lf, rf);
 *
 * // Single function
 * auto res2 = eitherObj | frt::fold(f);
 * \\endcode
 *
 * For partial application, the callables must satisfy the same type requirements as \\c std::bind_front:
 * \\c std::decay_t<T> must be move-constructible and constructible from \\c T.
 *
 * \\note Partial application captures the callables by value into an intermediate closure object, which may incur
 * a performance penalty due to copying or moving. Prefer direct invocation or \\c std::ref / \\c std::cref when
 * passing expensive-to-copy arguments.
 *
 * The algorithm works with all either-like result types (such as \\c frt::either, \\c frt::expected, \\c frt::optional,
 * \\c std::expected, or \\c std::optional).
 *
 * To use \\c frt::fold with a custom result type, specialize \\c frt::is_result_type (set to \\c true) and
 * \\c frt::meta_either:
 * - \\c left_t -- the type of the left alternative,
 * - \\c right_t -- the type of the right alternative,
 * - \\c is_left(result) -- a callable checking whether the object contains the left alternative,
 * - \\c right(result) -- a callable accessing the right alternative,
 * - \\c left(result) -- a callable accessing the left alternative.
 */
inline constexpr detail::fold_alg_::fold_fn fold{};

} // namespace frt
