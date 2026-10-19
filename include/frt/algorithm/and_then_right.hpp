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
                "The function must be invocable with a value of left_t and all extra arguments. "
                "Consider using frt::unwrapped if an argument of left_t shouldn't go first.");

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
  static_assert(std::invocable<F, Args...>,
                "The function must be invocable with a value of left_t and all extra arguments. "
                "Consider using frt::unwrapped if an argument of left_t shouldn't go first.");

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

template <class F, class... Args>
struct and_then_right_closure_fn
{
  F f_;
  std::tuple<Args &&...> func_and_args_;

  template <IsResultType Result, class Cf = and_then_right_closure_fn>
  friend constexpr decltype(auto) operator|(Result &&result, Cf &&c) noexcept
  {
    return std::apply(
        [&result, &c]<class... Ts>(Ts &&...ts) mutable {
          return and_then_right(std::forward<Result>(result), std::forward<Cf>(c).f_, std::forward<Ts>(ts)...);
        },
        std::forward<Cf>(c).func_and_args_);
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
  [[nodiscard]] constexpr decltype(auto) operator()(F &&f, Args &&...args) const noexcept
  {
    return and_then_right_closure_fn<F, Args...>{.f_ = std::forward<F>(f),
                                                 .func_and_args_ = std::forward_as_tuple(std::forward<Args>(args)...)};
  }
};

} // namespace detail::and_then_right_alg_

inline constexpr detail::and_then_right_alg_::and_then_right_fn and_then_right{};

} // namespace frt
