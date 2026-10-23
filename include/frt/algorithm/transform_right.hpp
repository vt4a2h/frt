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

#include <frt/concept/concept.hpp>
#include <frt/concept/detail/common.hpp>
#include <frt/detail/attributes.hpp>
#include <frt/meta/meta_either.hpp>

namespace frt
{

namespace detail::transform_right_alg_
{

template <class ResT, class MetaEither = meta_either<common_::bare_t<ResT>>>
concept ValidMetaTransformRightInResult = requires(MetaEither me, ResT &&r) {
  typename MetaEither::left_t;
  typename MetaEither::right_t;
  typename MetaEither::template rebind_right<int>;

  me.is_right(r);

  me.right(std::forward<ResT>(r));
  me.left(std::forward<ResT>(r));
};

template <class MetaEither, class Left, class Right>
concept ValidMetaTransformRightOutResult = requires(MetaEither me, Left &&left, Right &&right) {
  me.make_left(std::forward<Left>(left));
  me.make_right(std::forward<Right>(right));
};

template <class R>
using FwdRightT = common_::FwdT<R, typename meta_either<common_::bare_t<R>>::right_t>;

template <class R>
using RightT = meta_either<common_::bare_t<R>>::right_t;

template <IsResultType Result, class F, class... Args>
  requires(ValidMetaTransformRightInResult<Result> && !common_::IsVoid<RightT<Result>>)
[[nodiscard]] constexpr auto
transform_right(Result &&result, F &&f,
                Args &&...args) noexcept(common_::is_nothrow_invocable_with_extra_args<F, FwdRightT<Result>, Args...>)
{
  using namespace detail::common_;

  using meta_t = meta_either<bare_t<Result>>;
  static_assert(ValidMetaTransformRightInResult<Result>,
                "frt::meta_either for the incoming result type Result must provide the following interface: typename "
                "left_t, typename right_t, typename template rebind_right<U>, is_right(Result{}), right(Result{}), and "
                "left(Result{}).");

  using fwd_t = FwdT<Result, typename meta_t::right_t>;

  static_assert(!single_arg_is_unwrapped<Args...>,
                "The trailing argument pack of a single argument must not contain frt::unwrapped.");
  static_assert(contains_zero_or_one_unwrapped<Args...>,
                "The trailing argument pack must contain zero or one frt::unwrapped.");
  static_assert(is_invocable_with_extra_args<F, fwd_t, Args...>,
                "The function must be invocable with a value of right_t and all extra arguments. "
                "Consider using frt::unwrapped if an argument of right_t shouldn't go first.");

  using func_ret_t = invoke_result_t<F, fwd_t, Args...>;
  using ret_t = meta_t::template rebind_right<bare_t<func_ret_t>>;
  using meta_ret_t = meta_either<bare_t<ret_t>>;

  static_assert(
      ValidMetaTransformRightOutResult<meta_ret_t, SelectedType<typename meta_t::left_t>, SelectedType<func_ret_t>>,
      "frt::meta_either for the return result type R must provide the following interface: typename left_t, "
      "typename right_t, make_left(L{}), make_right(R{}). Where R is the return type of the function f and "
      "L is the type of the left alternative of the input result type.");

  if (meta_t{}.is_right(result))
    FRT_LIKELY
    {
      if constexpr (IsVoid<func_ret_t>) {
        invoke_with_extra_args(std::forward<F>(f), meta_t{}.right(std::forward<Result>(result)),
                               std::forward<Args>(args)...);
        return meta_ret_t{}.make_right(void_val);
      } else {
        return meta_ret_t{}.make_right(invoke_with_extra_args(
            std::forward<F>(f), meta_t{}.right(std::forward<Result>(result)), std::forward<Args>(args)...));
      }
    }
  else {
    return meta_ret_t{}.make_left(meta_t{}.left(std::forward<Result>(result)));
  }
}

template <IsResultType Result, class F, class... Args>
  requires(ValidMetaTransformRightInResult<Result> && common_::IsVoid<RightT<Result>>)
[[nodiscard]] constexpr auto transform_right(Result &&result, F &&f,
                                             Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
{
  using namespace detail::common_;

  using meta_t = meta_either<bare_t<Result>>;
  static_assert(ValidMetaTransformRightInResult<Result>,
                "frt::meta_either for the incoming result type Result must provide the following interface: typename "
                "left_t, typename right_t, typename template rebind_right<U>, is_right(Result{}), right(Result{}), and "
                "left(Result{}).");

  static_assert(!contains_unwrapped<Args...>, "The trailing argument pack must not contain frt::unwrapped.");
  static_assert(std::invocable<F, Args...>, "The function must be invocable with all extra arguments.");

  using func_ret_t = std::invoke_result_t<F, Args...>;
  using ret_t = meta_t::template rebind_right<bare_t<func_ret_t>>;
  using meta_ret_t = meta_either<bare_t<ret_t>>;

  static_assert(
      ValidMetaTransformRightOutResult<meta_ret_t, SelectedType<typename meta_t::left_t>, SelectedType<func_ret_t>>,
      "frt::meta_either for the return result type R must provide the following interface: typename left_t, "
      "typename right_t, make_left(L{}), make_right(R{}). Where R is the return type of the function f and "
      "L is the type of the left alternative of the input result type.");

  if (meta_t{}.is_right(result))
    FRT_LIKELY
    {
      if constexpr (IsVoid<func_ret_t>) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        return meta_ret_t{}.make_right(void_val);
      } else {
        return meta_ret_t{}.make_right(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
      }
    }
  else {
    return meta_ret_t{}.make_left(meta_t{}.left(std::forward<Result>(result)));
  }
}

template <class F>
struct transform_right_closure_fn
{
  F f_;

  template <IsResultType Result, class Self>
    requires std::same_as<common_::bare_t<Self>, transform_right_closure_fn>
  friend constexpr decltype(auto)
  operator|(Result &&result, Self &&c) noexcept(std::is_nothrow_invocable_v<common_::FwdT<Self, F>, Result>)
  {
    return std::invoke(std::forward_like<Self>(c.f_), std::forward<Result>(result));
  }
};

struct transform_right_fn
{
  template <IsResultType Result, class F, class... Args>
  [[nodiscard]] constexpr decltype(auto) operator()(Result &&result, F &&f, Args &&...args) const
      noexcept(noexcept(transform_right(std::forward<Result>(result), std::forward<F>(f), std::forward<Args>(args)...)))
  {
    return transform_right(std::forward<Result>(result), std::forward<F>(f), std::forward<Args>(args)...);
  }

  template <class F, class... Args>
    requires(!IsResultType<F> && (std::is_move_constructible_v<std::decay_t<Args>> && ...) &&
             (std::is_constructible_v<std::decay_t<Args>, Args> && ...) &&
             std::is_move_constructible_v<std::decay_t<F>> && std::is_constructible_v<std::decay_t<F>, F>)
  [[nodiscard]] constexpr decltype(auto) operator()(F &&f, Args &&...args) const
      noexcept(std::is_nothrow_constructible_v<std::decay_t<F>, F> &&
               (std::is_nothrow_constructible_v<std::decay_t<Args>, Args> && ...))
  {
    return transform_right_closure_fn{
        .f_ = [... args = std::forward<Args>(args), f = std::forward<F>(f)]<class Self, IsResultType Result>(
                  this Self &&,
                  Result
                      &&result) noexcept(noexcept(transform_right(std::forward<Result>(result),
                                                                  std::declval<common_::FwdT<Self, std::decay_t<F>>>(),
                                                                  std::declval<
                                                                      common_::FwdT<Self, std::decay_t<Args>>>()...))) {
          return transform_right(std::forward<Result>(result), std::forward_like<Self>(f),
                                 std::forward_like<Self>(args)...);
        }};
  }
};

} // namespace detail::transform_right_alg_

/*!
 * \brief Applies a function to the right alternative of a result object and returns the result.
 *
 * Has no effect if the result object contains the left alternative; in that case, the function is not invoked and a
 * new result object of the return type containing the existing left alternative is returned.
 *
 * It is possible to supply an optional pack of extra arguments to the function \p f. All arguments are forwarded to
 * \p f preserving their value category and constness.
 *
 * When the right alternative is not \p void:
 * \code{.cpp}
 * auto f(right, a, b, c) -> T;
 *
 * // Equivalent to f(right, a, b, c)
 * frt::transform_right(resultObj, f, a, b, c);
 *
 * // Equivalent to f(a, right, b, c)
 * frt::transform_right(resultObj, f, a, frt::unwrapped, b, c);
 * \endcode
 *
 * The \c frt::unwrapped placeholder is replaced with the right alternative. The extra argument pack must contain zero
 * or one \c frt::unwrapped object. If it is a single-argument pack, that argument must not be \c frt::unwrapped.
 *
 * When the right alternative is \p void, using \c frt::unwrapped as an argument is prohibited, and the function is
 * invoked only with the supplied extra arguments:
 * \code{.cpp}
 * auto f(a, b, c) -> T;
 *
 * // Equivalent to f(a, b, c)
 * frt::transform_right(resultObj, f, a, b, c);
 * \endcode
 *
 * The function \p f must be invocable with the final combination of arguments. The return value will automatically be
 * wrapped into a result object of the same family with the right alternative replaced with the return type of \p f
 * (via \c rebind_right in \c frt::meta_either). If \p f returns \c void, the right alternative becomes \c void.
 *
 * Pipe syntax for partial application is also supported:
 * \code{.cpp}
 * // Without extra arguments
 * auto res1 = resultObj | frt::transform_right(f);
 *
 * // With extra arguments and frt::unwrapped
 * auto res2 = resultObj | frt::transform_right(f, a, frt::unwrapped, b, c);
 * \endcode
 *
 * For partial application, the callable \p f and each extra argument type \c T in \p Args must satisfy the same type
 * requirements as \c std::bind_front and \c std::bind_back: \c std::decay_t<T> must be move-constructible and
 * constructible from \c T (\c std::is_move_constructible_v<std::decay_t<T>> and
 * \c std::is_constructible_v<std::decay_t<T>, T>).
 *
 * \note Partial application captures the callable and all extra arguments by value into an intermediate closure object,
 * which may incur a performance penalty due to copying or moving (even if the result object contains the left
 * alternative and \p f is never invoked). Prefer direct invocation or \c std::ref / \c std::cref when passing
 * expensive-to-copy arguments.
 *
 * The algorithm works with all either-like result types (such as \c frt::either, \c frt::expected, \c frt::optional,
 * \c std::expected, or \c std::optional). By convention, the left alternative represents an error or empty state, while
 * the right alternative represents a success value.
 *
 * To use \c frt::transform_right with a custom result type, specialize \c frt::is_result_type (set to \c true) and
 * \c frt::meta_either:
 * - For the input result type \c Result, the \c frt::meta_either specialization must provide:
 *   - \c left_t -- the type of the left alternative,
 *   - \c right_t -- the type of the right alternative,
 *   - \c rebind_right<U> -- template alias to produce the result type with a new right alternative \c U,
 *   - \c is_right(result) -- a callable checking whether the object contains the right alternative,
 *   - \c right(result) -- a callable accessing the right alternative,
 *   - \c left(result) -- a callable accessing the left alternative.
 * - For the return result type, the \c frt::meta_either specialization must provide:
 *   - \c make_left(left) -- a callable constructing the return result object from the left alternative,
 *   - \c make_right(right) -- a callable constructing the return result object from the right alternative (or \c
 * make_right() when right is \c void).
 */
inline constexpr detail::transform_right_alg_::transform_right_fn transform_right{};

} // namespace frt
