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

namespace detail::and_then_right_alg_
{

template <class ResT, class MetaEither = meta_either<common_::bare_t<ResT>>>
concept ValidMetaAndThenInResut = requires(MetaEither me, ResT &&r) {
  typename MetaEither::left_t;
  typename MetaEither::right_t;
  typename MetaEither::tag_t;

  me.is_right(r);

  me.right(std::forward<ResT>(r));
  me.left(std::forward<ResT>(r));
};

template <class MetaEither, class Left>
concept ValidMetaAndThenOutResut = requires(MetaEither me, Left &&left) {
  typename MetaEither::left_t;
  typename MetaEither::tag_t;
  me.make_left(std::forward<Left>(left));
};

template <class R>
using FwdRightT = common_::FwdT<R, typename meta_either<common_::bare_t<R>>::right_t>;

template <class R>
using RightT = meta_either<common_::bare_t<R>>::right_t;

template <IsResultType Result, class F, class... Args>
  requires(ValidMetaAndThenInResut<Result> && !common_::IsVoid<RightT<Result>>)
[[nodiscard]] constexpr auto
and_then_right(Result &&result, F &&f,
               Args &&...args) noexcept(common_::is_nothrow_invocable_with_extra_args<F, FwdRightT<Result>, Args...>)
{
  using namespace detail::common_;

  using meta_t = meta_either<bare_t<Result>>;
  static_assert(ValidMetaAndThenInResut<Result>,
                "frt::meta_either for the incoming result type Result must provide the following interface: typename "
                "left_t, typename right_t, typename tag_t, is_right(Result{}), right(Result{}), and left(Result{}).");

  using fwd_t = FwdT<Result, typename meta_t::right_t>;

  static_assert(!single_arg_is_unwrapped<Args...>,
                "The trailing argument pack of a single argument must not contain frt::unwrapped.");
  static_assert(contains_zero_or_one_unwrapped<Args...>,
                "The trailing argument pack must contain zero or one frt::unwrapped.");
  static_assert(is_invocable_with_extra_args<F, fwd_t, Args...>,
                "The function must be invocable with a value of right_t and all extra arguments. "
                "Consider using frt::unwrapped if an argument of right_t shouldn't go first.");

  using ret_t = invoke_result_t<F, fwd_t, Args...>;
  using meta_ret_t = meta_either<bare_t<ret_t>>;

  static_assert(
      ValidMetaAndThenOutResut<meta_ret_t, SelectedType<typename meta_t::left_t>>,
      "frt::meta_either for the return result type R must provide the following interface: typename left_t, "
      "make_left(L{}), typename tag_t. Where L is the type of the left alternative of the function f return type.");

  static_assert(std::same_as<typename meta_ret_t::tag_t, typename meta_t::tag_t>,
                "The function must return an object of the same result type.");
  static_assert(std::same_as<typename meta_ret_t::left_t, typename meta_t::left_t>,
                "The function must return an object of a result type with the same left alternative");

  if (meta_t{}.is_right(result))
    FRT_LIKELY
    {
      return invoke_with_extra_args(std::forward<F>(f), meta_t{}.right(std::forward<Result>(result)),
                                    std::forward<Args>(args)...);
    }
  else {
    return meta_ret_t{}.make_left(meta_t{}.left(std::forward<Result>(result)));
  }
}

template <IsResultType Result, class F, class... Args>
  requires(ValidMetaAndThenInResut<Result> && common_::IsVoid<RightT<Result>>)
[[nodiscard]] constexpr auto and_then_right(Result &&result, F &&f,
                                            Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
{
  using namespace detail::common_;

  using meta_t = meta_either<bare_t<Result>>;
  static_assert(ValidMetaAndThenInResut<Result>,
                "frt::meta_either for the incoming result type Result must provide the following interface: typename "
                "left_t, typename right_t, typename tag_t, is_right(Result{}), right(Result{}), and left(Result{}).");

  static_assert(!contains_unwrapped<Args...>, "The trailing argument pack must not contain frt::unwrapped.");
  static_assert(std::invocable<F, Args...>, "The function must be invocable with all extra arguments.");

  using ret_t = std::invoke_result_t<F, Args...>;
  using meta_ret_t = meta_either<bare_t<ret_t>>;

  static_assert(
      ValidMetaAndThenOutResut<meta_ret_t, SelectedType<typename meta_t::left_t>>,
      "frt::meta_either for the return result type R must provide the following interface: typename left_t, "
      "make_left(L{}), typename tag_t. Where L is the type of the left alternative of the function f return type.");

  static_assert(std::same_as<typename meta_ret_t::tag_t, typename meta_t::tag_t>,
                "The function must return an object of the same result type.");
  static_assert(std::same_as<typename meta_ret_t::left_t, typename meta_t::left_t>,
                "The function must return an object of a result type with the same left alternative");

  if (meta_t{}.is_right(result))
    FRT_LIKELY { return std::invoke(std::forward<F>(f), std::forward<Args>(args)...); }
  else {
    return meta_ret_t{}.make_left(meta_t{}.left(std::forward<Result>(result)));
  }
}

template <class F>
struct and_then_right_closure_fn
{
  F f_;

  template <IsResultType Result>
  friend constexpr decltype(auto) operator|(Result &&result, and_then_right_closure_fn c) noexcept
  {
    return std::invoke(c.f_, std::forward<Result>(result));
  }
};

struct and_then_right_fn
{
  template <IsResultType Result, class F, class... Args>
  [[nodiscard]] constexpr decltype(auto) operator()(Result &&result, F &&f, Args &&...args) const
  {
    return and_then_right(std::forward<Result>(result), std::forward<F>(f), std::forward<Args>(args)...);
  }

  template <class F, class... Args>
    requires((std::is_move_constructible_v<std::remove_cvref_t<Args>> && ...) &&
             (std::is_constructible_v<std::remove_cvref_t<Args>, Args> && ...) &&
             std::is_move_constructible_v<std::remove_cvref_t<F>> && std::is_constructible_v<std::remove_cvref_t<F>, F>)
  [[nodiscard]] constexpr decltype(auto) operator()(F &&f, Args &&...args) const noexcept
  {
    return and_then_right_closure_fn{
        .f_ = [... args = std::forward<Args>(args), f = std::forward<F>(f)]<IsResultType Result>(Result &&result) {
          return and_then_right(std::forward<Result>(result), f, std::move(args)...);
        }};
  }
};

} // namespace detail::and_then_right_alg_

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
 * auto f(right, a, b, c) -> Result;
 *
 * // Equivalent to f(right, a, b, c)
 * frt::and_then_right(resultObj, f, a, b, c);
 *
 * // Equivalent to f(a, right, b, c)
 * frt::and_then_right(resultObj, f, a, frt::unwrapped, b, c);
 * \endcode
 *
 * The \c frt::unwrapped placeholder is replaced with the right alternative. The extra argument pack must contain zero
 * or one \c frt::unwrapped object. If it is a single-argument pack, that argument must not be \c frt::unwrapped.
 *
 * When the right alternative is \p void, using \c frt::unwrapped as an argument is prohibited, and the function is
 * invoked only with the supplied extra arguments:
 * \code{.cpp}
 * auto f(a, b, c) -> Result;
 *
 * // Equivalent to f(a, b, c)
 * frt::and_then_right(resultObj, f, a, b, c);
 * \endcode
 *
 * The function \p f must be invocable with the final combination of arguments and must return a result object of the
 * same result type family (matching \c tag_t in \c frt::meta_either) and with the same left alternative type (\c left_t
 * in \c frt::meta_either). The right alternative type (\c right_t in \c frt::meta_either) may differ from the original
 * one.
 *
 * Pipe syntax for partial application is also supported:
 * \code{.cpp}
 * // Without extra arguments
 * auto res1 = resultObj | frt::and_then_right(f);
 *
 * // With extra arguments and frt::unwrapped
 * auto res2 = resultObj | frt::and_then_right(f, a, frt::unwrapped, b, c);
 * \endcode
 *
 * For partial application, the callable \p f and each extra argument type \c T in \p Args must satisfy the same type
 * requirements as \c std::bind_front and \c std::bind_back: \c std::remove_cvref_t<T> must be move-constructible and
 * constructible from \c T (\c std::is_move_constructible_v<std::remove_cvref_t<T>> and
 * \c std::is_constructible_v<std::remove_cvref_t<T>, T>).
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
 * To use \c frt::and_then_right with a custom result type, specialize \c frt::is_result_type (set to \c true) and
 * \c frt::meta_either:
 * - For the input result type \c Result, the \c frt::meta_either specialization must provide:
 *   - \c left_t -- the type of the left alternative,
 *   - \c right_t -- the type of the right alternative,
 *   - \c tag_t -- a unique non-template tag type identifying the result type family,
 *   - \c is_right(result) -- a callable checking whether the object contains the right alternative,
 *   - \c right(result) -- a callable accessing the right alternative,
 *   - \c left(result) -- a callable accessing the left alternative.
 * - For the return result type of \p f, the \c frt::meta_either specialization must provide:
 *   - \c left_t -- must be the same type as the input result type's \c left_t,
 *   - \c tag_t -- must be the same type as the input result type's \c tag_t,
 *   - \c make_left(left) -- a callable constructing the return result object from the left alternative.
 */
inline constexpr detail::and_then_right_alg_::and_then_right_fn and_then_right{};

} // namespace frt
