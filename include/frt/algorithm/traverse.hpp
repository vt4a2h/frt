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

#include <frt/meta/meta_container.hpp>
#include <frt/meta/meta_either.hpp>

namespace frt
{

namespace detail::trav_alg_
{

template <class MetaEither, class ResT>
concept ValidMetaEitherFuncRetT = requires(MetaEither me, ResT &&r) {
  typename MetaEither::right_t;
  typename MetaEither::left_t;
  typename MetaEither::template rebind_right<ResT>;

  me.is_right(r);

  me.right(std::forward<ResT>(r));
  me.left(std::forward<ResT>(r));
};

template <class MetaEither, class Left, class Right>
concept ValidMetaEitherResC = requires(MetaEither me, Left &&l, Right &&r) {
  me.make_left(std::forward<Left>(l));
  me.make_right(std::forward<Right>(r));
};

template <template <typename...> class OutputContainer = std::vector, std::ranges::input_range InputRange, class F>
[[nodiscard]] constexpr decltype(auto) traverse(InputRange &&inputRange, F f) noexcept
{
  using namespace detail::common_;

  using range_val_t = std::ranges::range_value_t<InputRange>;
  static_assert(IsResultType<bare_t<range_val_t>>, "The input range must contain values of the result type.  "
                                                   "Specialize frt::is_result_type for your type as needed.");

  using func_ret_result_t = std::invoke_result_t<F, range_val_t>;
  static_assert(
      IsResultType<bare_t<func_ret_result_t>>,
      "The return type of the function must be a result type. Specialize frt::is_result_type for your type as needed.");
  static_assert(std::move_constructible<bare_t<func_ret_result_t>>,
                "The return type of the function must be move constructible.");

  using meta_t = meta_either<bare_t<func_ret_result_t>>;
  static_assert(
      ValidMetaEitherFuncRetT<meta_t, func_ret_result_t>,
      "frt::meta_either for the function return type T must provide the following interface: "
      "typename left_t, typename right_t, typename template rebind_right<U>, is_right(T{}), right(T{}), left(T{}).");
  static_assert(IsNotVoid<typename meta_t::left_t>, "The type of the left alternative of the function return type must "
                                                    "not be void. Use frt::void_t as needed instead.");

  using func_ret_right_t = meta_t::right_t;
  static_assert(IsNotVoid<func_ret_right_t>,
                "The type of the right alternative of the function return type must not be void.");

  using container_t = meta_container_::select_container_t<OutputContainer, func_ret_right_t>;
  static_assert(std::move_constructible<container_t>, "The result container must be move constructible.");

  using output_t = meta_t::template rebind_right<container_t>;
  using output_meta_t = meta_either<output_t>;
  static_assert(ValidMetaEitherResC<output_meta_t, typename meta_t::left_t, container_t>,
                "The frt::meta_either for the output type must provide the following interface: make_left(Left{}) and "
                "make_right(Right{}). Where Left -- the same left alternative type as before, Right -- the output "
                "container type.");

  using meta_container_t = meta_container<container_t>;
  static_assert(meta_container_::MetaContainerHasAddElement<container_t>,
                "The frt::meta_container must provide an add_element interface.");

  container_t resultContainer;
  if constexpr (std::ranges::sized_range<container_t> && meta_container_::MetaContainerHasReserve<container_t>) {
    meta_container_t{}.reserve(resultContainer, std::ranges::size(inputRange));
  }

  for (auto &&v : std::forward<InputRange>(inputRange)) {
    if (auto res = std::invoke(f, std::forward<decltype(v)>(v)); meta_t{}.is_right(res)) {
      meta_container_t{}.add_element(resultContainer, meta_t{}.right(std::move(res)));
    } else {
      return output_meta_t{}.make_left(meta_t{}.left(std::move(res)));
    }
  }

  return output_meta_t{}.make_right(std::move(resultContainer));
}

template <class F>
struct traverse_closure_fn
{
  F f_;

  template <std::ranges::input_range R>
  friend constexpr decltype(auto) operator|(R &&r, const traverse_closure_fn &c) noexcept
  {
    return traverse<std::vector>(std::forward<R>(r), c.f_);
  }
};

struct traverse_fn
{
  template <std::ranges::input_range InputRange, class F>
  [[nodiscard]] constexpr decltype(auto) operator()(InputRange &&inputRange, F f) const noexcept
  {
    return traverse<std::vector>(std::forward<InputRange>(inputRange), std::forward<F>(f));
  }

  template <class F>
  [[nodiscard]] constexpr decltype(auto) operator()(F f) const noexcept
  {
    return traverse_closure_fn{.f_ = f};
  }
};

template <template <typename...> class OutputContainer, class F>
struct traverse_into_closure_fn
{
  F f_;

  template <std::ranges::input_range R>
  friend constexpr decltype(auto) operator|(R &&r, const traverse_into_closure_fn &c) noexcept
  {
    return traverse<OutputContainer>(std::forward<R>(r), c.f_);
  }
};

template <template <typename...> class OutputContainer>
struct traverse_into_fn
{
  template <std::ranges::input_range InputRange, class F>
  [[nodiscard]] constexpr decltype(auto) operator()(InputRange &&inputRange, F f) const noexcept
  {
    return traverse<OutputContainer>(std::forward<InputRange>(inputRange), std::forward<F>(f));
  }

  template <class F>
  [[nodiscard]] constexpr decltype(auto) operator()(F f) const noexcept
  {
    return traverse_into_closure_fn<OutputContainer, F>{.f_ = f};
  }
};

} // namespace detail::trav_alg_

/*!
 * \brief The traverse algorithm.
 *
 * The traverse algorithm takes a range of objects of a result-like type, applies the passed function to each element,
 * and collects the unwrapped success values into a `std::vector` wrapped in the result type. If any function call
 * yields an error or empty state (the left alternative), traversal short-circuits and immediately returns that failure.
 *
 * If all operations succeed, the returned object contains a `std::vector` of the transformed values:
 * \code{.cpp}
 * using Result = frt::optional<int>;
 * using Output = frt::optional<std::vector<int>>;
 *
 * std::vector input{Result{1}, Result{2}, Result{3}};
 * Output result = frt::traverse(input, [](const Result &v) { return Result{v.value() + 1}; });
 *
 * // result contains std::vector{2, 3, 4}
 * \endcode
 *
 * If any element is in an error or empty state, the result will contain that failure:
 * \code{.cpp}
 * using Result = frt::optional<int>;
 * using Output = frt::optional<std::vector<int>>;
 *
 * std::vector input{Result{1}, Result{}, Result{3}};
 * Output result = frt::traverse(input, [](const Result &v) -> Result {
 *   if (!v) {
 *     return {};
 *   }
 *   return v.value() + 1;
 * });
 *
 * // result contains nothing (empty optional)
 * \endcode
 *
 * The function works with all either-like result types (such as \c frt::either, \c frt::expected, \c frt::optional,
 * \c std::expected, or \c std::optional). Custom result types require specializing \c frt::meta_either and
 * \c frt::is_result_type. By convention, the left alternative represents an error or empty state, while the right
 * alternative represents a success value.
 *
 * Pipe syntax for partial application is also supported:
 * \code{.cpp}
 * using Result = frt::optional<int>;
 * using Output = frt::optional<std::vector<int>>;
 *
 * std::vector input{Result{1}, Result{2}, Result{3}};
 * Output result = input | frt::traverse([](const Result &v) { return Result{v.value() + 1}; });
 *
 * // result contains std::vector{2, 3, 4}
 * \endcode
 * Note that this is an eager operation evaluated immediately; it returns a container wrapped in a result type, not a
 * view.
 *
 * The default output container is `std::vector`. To collect into a different container, use \c frt::traverse_into.
 */
inline constexpr detail::trav_alg_::traverse_fn traverse{};

/*!
 * \brief The traverse algorithm with a customizable output container.
 *
 * This function behaves identically to \c frt::traverse, but allows specifying a custom output container template.
 *
 * It supports standard sequential and associative containers out-of-the-box. Custom container types require
 * a specialization of \c frt::meta_container.
 *
 * For example, elements can be collected into a `std::list` instead of a `std::vector`:
 * \code{.cpp}
 * using Result = frt::optional<int>;
 * using Output = frt::optional<std::list<int>>;
 *
 * std::vector input{Result{1}, Result{2}, Result{3}};
 * Output result = frt::traverse_into<std::list>(input, [](const Result &v) { return Result{v.value() + 1}; });
 *
 * // result contains std::list{2, 3, 4}
 * \endcode
 *
 * When collecting into an associative container (such as `std::map`), the right alternative of the function's return
 * type must be a `std::pair` matching the container's key and value types:
 * \code{.cpp}
 * using P = std::pair<int, int>;
 * using Result = frt::optional<P>;
 * using Output = frt::optional<std::map<int, int>>;
 *
 * std::vector input{Result{P{1, 1}}, Result{P{2, 2}}, Result{P{3, 3}}};
 * Output result = frt::traverse_into<std::map>(input, [](const Result &v) {
 *   return Result{P{v.value().first + 1, v.value().second + 1}};
 * });
 *
 * // result contains std::map{P{2, 2}, P{3, 3}, P{4, 4}}
 * \endcode
 *
 * Pipe syntax is also supported:
 * \code{.cpp}
 * auto result = input | frt::traverse_into<std::list>([](const Result &v) { return Result{v.value() + 1}; });
 * \endcode
 */
template <template <typename...> class OutputContainer>
inline constexpr detail::trav_alg_::traverse_into_fn<OutputContainer> traverse_into{};

} // namespace frt
