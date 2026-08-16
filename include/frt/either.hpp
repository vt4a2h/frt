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

#include <functional>
#include <type_traits>
#include <utility>

#include <frt/expects.hpp>

namespace frt
{

namespace detail::either_
{

template <class T>
using bare_t = std::remove_cvref_t<T>;

struct Void
{
  friend constexpr bool operator==(Void, Void) noexcept = default;
  friend constexpr std::partial_ordering operator<=>(Void, Void) noexcept = default;
};
inline constexpr Void void_val;

template <class T>
concept IsVoid = std::is_void_v<bare_t<T>>;

template <class T>
concept IsNotVoid = !IsVoid<T>;

template <class T>
concept ValidType = requires {
  requires std::destructible<bare_t<T>> || IsVoid<T>;
  requires !std::is_reference_v<T>;
  requires !std::is_array_v<bare_t<T>>;
};

} // namespace detail::either_

template <detail::either_::ValidType Left, detail::either_::ValidType Right>
struct either;

namespace detail::either_
{
template <class T>
inline constexpr bool is_either_ = false;

template <ValidType L, ValidType R>
inline constexpr bool is_either_<either<L, R>> = true;

template <class T>
concept IsNotEither = !is_either_<bare_t<T>>;

struct InPlaceLeftTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceLeftTag(Secret) noexcept {}
};

struct InPlaceRightTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceRightTag(Secret) noexcept {}
};

struct UnwrappedTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit UnwrappedTag(Secret) noexcept {}
};

template <class T>
concept is_tag_ = std::same_as<T, InPlaceLeftTag> || std::same_as<T, InPlaceRightTag>;

template <class U, class T>
concept ValidConstructableFrom = requires {
  requires std::constructible_from<T, U>;
  requires !is_either_<bare_t<U>>;
  requires !is_tag_<bare_t<U>>;
};

template <class T>
concept IsHashable = requires(T v) {
  { std::hash<T>{}(v) } -> std::same_as<std::size_t>;
};

template <class L1, class R1, class L2, class R2>
concept EqualityComparable = std::equality_comparable_with<L1, L2> && std::equality_comparable_with<R1, R2>;

template <class L1, class R1, class L2, class R2>
concept ThreeWayComparable = std::three_way_comparable_with<L1, L2> && std::three_way_comparable_with<R1, R2>;

inline constexpr std::size_t void_hash{};

template <class T>
using SelectedType = std::conditional_t<IsVoid<T>, Void, bare_t<T>>;

template <class T>
inline constexpr bool is_unwrapped = std::same_as<std::remove_cvref_t<T>, UnwrappedTag>;

template <class... Args>
constexpr inline std::size_t unwrapped_count = (std::size_t{} + ... + static_cast<std::size_t>(is_unwrapped<Args>));

template <class... Args>
constexpr inline bool single_arg_is_unwrapped = sizeof...(Args) == 1 && unwrapped_count<Args...> == 1;

template <class... Args>
constexpr inline bool contains_zero_or_one_unwrapped = unwrapped_count<Args...> <= 1;

template <class... Args>
inline constexpr bool contains_unwrapped = (... || is_unwrapped<Args>);

template <class F, class T, class... Args>
constexpr inline bool is_invocable_with_extra_args = std::is_invocable_v<F, T, Args...>;

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
constexpr inline bool is_invocable_with_extra_args<F, T, Args...> =
    std::is_invocable_v<F, std::conditional_t<is_unwrapped<Args>, T, Args>...>;

template <class F, class T, class... Args>
constexpr inline bool is_nothrow_invocable_with_extra_args = std::is_nothrow_invocable_v<F, T, Args...>;

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
constexpr inline bool is_nothrow_invocable_with_extra_args<F, T, Args...> =
    std::is_nothrow_invocable_v<F, std::conditional_t<is_unwrapped<Args>, T, Args>...>;

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

  using type = std::invoke_result_t<F, std::conditional_t<is_unwrapped<Args>, T, Args>...>;
};

template <class F, class T, class... Args>
using invoke_result_t = invoke_result<F, T, Args...>::type;

template <class F, class T, class... Args>
[[nodiscard]] constexpr auto
invoke_with_extra_args(F &&f, T &&t, Args &&...args) noexcept(is_nothrow_invocable_with_extra_args<F, T, Args...>)
{
  return std::invoke(f, std::forward<T>(t), std::forward<Args>(args)...);
}

template <class T, class Arg>
constexpr auto &&forward(T &&t, Arg &&arg) noexcept
{
  // We can have only 0 or 1 unwrapped argument. Hence, it will be forwarded only once
  if constexpr (is_unwrapped<Arg>) {
    return std::forward<T>(t);
  } else {
    return std::forward<Arg>(arg);
  }
}

template <class F, class T, class... Args>
  requires(contains_unwrapped<Args...>)
[[nodiscard]] constexpr auto
invoke_with_extra_args(F &&f, T &&t, Args &&...args) noexcept(is_nothrow_invocable_with_extra_args<F, T, Args...>)
{
  return std::invoke(f, forward(std::forward<T>(t), std::forward<Args>(args))...);
}

template <class E1, class E2>
constexpr inline bool same_right_alternative =
    std::same_as<typename std::remove_cvref_t<E1>::right_t, typename std::remove_cvref_t<E2>::right_t>;

template <class E1, class E2>
constexpr inline bool same_left_alternative =
    std::same_as<typename std::remove_cvref_t<E1>::left_t, typename std::remove_cvref_t<E2>::left_t>;

} // namespace detail::either_

using in_place_left_t = detail::either_::InPlaceLeftTag;
using in_place_right_t = detail::either_::InPlaceRightTag;
using unwrapped_t = detail::either_::UnwrappedTag;

inline constexpr in_place_left_t in_place_left{in_place_left_t::Secret{}};
inline constexpr in_place_right_t in_place_right{in_place_right_t::Secret{}};
inline constexpr unwrapped_t unwrapped{unwrapped_t::Secret{}};

template <class T>
concept IsEither = detail::either_::is_either_<detail::either_::bare_t<T>>;

/*!
 * \brief This is the monad "Either".
 *
 * This is a simple tagged union with two distinct tags, `Left` and `Right`. The closest analog from the standard
 * library is `std::variant<L, R>`. This is a generic version of `frt::expected<E, U>` that should be used when tags do
 * not necessarily represent expected or unexpected results.
 *
 * The underlying types must not be references or arrays.
 *
 * The `frt::expects` is used to ensure the invariant. By default, the program is terminated upon the violation of the
 * contract.
 */
template <detail::either_::ValidType Left, detail::either_::ValidType Right>
struct [[nodiscard]] either
{
private:
  using left_internal_t = detail::either_::SelectedType<Left>;
  using right_internal_t = detail::either_::SelectedType<Right>;

public:
  using left_t = detail::either_::bare_t<Left>;
  using right_t = detail::either_::bare_t<Right>;

  //! The object must be created of either Left or Right. The uninitialized state is not permitted.
  either() = delete;

  //! Create from another `either` by copy.
  constexpr either(const either &src) noexcept(std::is_nothrow_copy_constructible_v<left_internal_t> &&
                                               std::is_nothrow_copy_constructible_v<right_internal_t>)
  {
    initFrom(src);
  }

  //! Create from another `either` by move.
  constexpr either(either &&src) noexcept(std::is_nothrow_move_constructible_v<left_internal_t> &&
                                          std::is_nothrow_move_constructible_v<right_internal_t>)
  {
    initFrom(std::move(src));
  }

  //! Assign from another `either` by copy.
  constexpr either &operator=(const either &src) noexcept(std::is_nothrow_copy_assignable_v<left_internal_t> &&
                                                          std::is_nothrow_copy_assignable_v<right_internal_t>)
  {
    destroy();
    initFrom(src);

    return *this;
  }

  //! Assign from another `either` by move.
  constexpr either &operator=(either &&src) noexcept(std::is_nothrow_move_assignable_v<left_internal_t> &&
                                                     std::is_nothrow_move_assignable_v<right_internal_t>)
  {
    destroy();
    initFrom(std::move(src));

    return *this;
  }

  //! Create from the left alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<left_internal_t, Args...>)
  constexpr either(in_place_left_t, Args &&...args) noexcept(std::is_nothrow_constructible_v<left_internal_t, Args...>)
  {
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = true;
  }

  //! Create from the left alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<left_internal_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_left_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<left_internal_t, std::initializer_list<U> &, Args...>)
  {
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = true;
  }

  //! Create from the right alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<right_internal_t, Args...>)
  constexpr either(in_place_right_t,
                   Args &&...args) noexcept(std::is_nothrow_constructible_v<right_internal_t, Args...>)
  {
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;
  }

  //! Create from the right alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<right_internal_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_right_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<right_internal_t, std::initializer_list<U> &, Args...>)
  {
    std::construct_at(std::addressof(right_), ilist, std::forward<Args>(args)...);
    is_left_ = false;
  }

  //! Default destructor.
  constexpr ~either() { destroy(); }

  //! Check for the left alternative.
  [[nodiscard]] constexpr bool is_left() const noexcept { return is_left_; }

  //! Check for the right alternative.
  [[nodiscard]] constexpr bool is_right() const noexcept { return !is_left(); }

  //! Get the left alternative. Pre-condition: it must contain the left alternative.
  template <class Self>
    requires(detail::either_::IsNotVoid<left_t>)
  [[nodiscard]] constexpr auto &&left(this Self &&self)
  {
    expects(self.is_left());
    return std::forward_like<Self>(self.left_);
  }

  //! Returns the left alternative or \p default value if the object contains the right alternative.
  template <class U = left_internal_t>
    requires(std::is_copy_constructible_v<left_internal_t> && std::is_convertible_v<U, left_internal_t> &&
             detail::either_::IsNotVoid<left_t>)
  [[nodiscard]] constexpr left_internal_t left_or(U &&default_value) const &
  {
    return is_left() ? left_ : static_cast<left_internal_t>(std::forward<U>(default_value));
  }

  //! Returns the left alternative or \p default value if the object contains the right alternative.
  template <class U = left_internal_t>
    requires(std::is_move_constructible_v<left_internal_t> && std::is_convertible_v<U, left_internal_t> &&
             detail::either_::IsNotVoid<left_t>)
  [[nodiscard]] constexpr left_internal_t left_or(U &&default_value) &&
  {
    return is_left() ? std::move(left_) : static_cast<left_internal_t>(std::forward<U>(default_value));
  }

  //! Get the right alternative. Pre-condition: it must contain the right alternative.
  template <class Self>
    requires(detail::either_::IsNotVoid<right_t>)
  [[nodiscard]] constexpr auto &&right(this Self &&self)
  {
    expects(self.is_right());
    return std::forward_like<Self>(self.right_);
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = right_internal_t>
    requires(std::is_copy_constructible_v<right_internal_t> && std::is_convertible_v<U, right_internal_t> &&
             detail::either_::IsNotVoid<right_t>)
  [[nodiscard]] constexpr right_internal_t right_or(U &&default_value) const &
  {
    return is_right() ? right_ : static_cast<right_internal_t>(std::forward<U>(default_value));
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = right_internal_t>
    requires(std::is_move_constructible_v<right_internal_t> && std::is_convertible_v<U, right_internal_t> &&
             detail::either_::IsNotVoid<right_t>)
  [[nodiscard]] constexpr right_internal_t right_or(U &&default_value) &&
  {
    return is_right() ? std::move(right_) : static_cast<right_internal_t>(std::forward<U>(default_value));
  }

  //! Compare with another either
  template <class L, class R>
    requires(detail::either_::EqualityComparable<left_internal_t, right_internal_t, L, R>)
  [[nodiscard]] friend constexpr bool
  operator==(const either &lhs, const either<L, R> &rhs) noexcept(noexcept(static_cast<bool>(lhs.left_ == rhs.left_)) &&
                                                                  noexcept(static_cast<bool>(lhs.right_ == rhs.right_)))
  {
    if (lhs.is_left_ != rhs.is_left_) {
      return false;
    }

    if (lhs.is_left_) {
      return lhs.left_ == rhs.left_;
    } else {
      return lhs.right_ == rhs.right_;
    }
  }

  //! Three-way compare with another either
  template <class L, class R>
    requires(detail::either_::ThreeWayComparable<left_internal_t, right_internal_t, L, R>)
  [[nodiscard]] friend constexpr std::partial_ordering
  operator<=>(const either &lhs,
              const either<L, R> &rhs) noexcept(noexcept(static_cast<std::partial_ordering>(lhs.left_ <=> rhs.left_)) &&
                                                noexcept(static_cast<std::partial_ordering>(lhs.right_ <=> rhs.right_)))
  {
    if (lhs.is_left_ == rhs.is_left_) {
      return lhs.is_left_ ? lhs.left_ <=> rhs.left_ : lhs.right_ <=> rhs.right_;
    } else {
      return std::partial_ordering::unordered;
    }
  }

  //! Swap value with \p rhs.
  constexpr void swap(either &rhs) noexcept(std::is_nothrow_swappable_v<left_internal_t> &&
                                            std::is_nothrow_swappable_v<right_internal_t>)
  {
    using std::swap;
    if (is_left_ != rhs.is_left_) {
      auto tmp = *this;

      this->destroy();
      this->initFrom(rhs);

      rhs.destroy();
      rhs.initFrom(tmp);
    } else if (is_left_) {
      swap(left_, rhs.left_);
    } else {
      swap(right_, rhs.right_);
    }
  }

  //! Swap \p lhs and \p rhs.
  friend constexpr void swap(either &lhs, either &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  //! Emplace left (from args)
  template <class... Args>
    requires(std::constructible_from<left_internal_t, Args...> && detail::either_::IsNotVoid<left_t>)
  constexpr left_internal_t &
  emplaceLeft(Args &&...args) noexcept(std::is_nothrow_constructible_v<left_internal_t, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = true;

    return left_;
  }

  //! Emplace left (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<left_internal_t, std::initializer_list<U> &, Args...> &&
             detail::either_::IsNotVoid<left_t>)
  constexpr left_internal_t &emplaceLeft(std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<left_internal_t, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = true;

    return left_;
  }

  //! Emplace left (void specialization)
  constexpr void emplaceLeft() noexcept
    requires(detail::either_::IsVoid<left_t>)
  {
    destroy();
    std::construct_at(std::addressof(left_));
    is_left_ = true;
  }

  //! Emplace right (from args)
  template <class... Args>
    requires(std::constructible_from<right_internal_t, Args...> && detail::either_::IsNotVoid<right_t>)
  constexpr right_internal_t &
  emplaceRight(Args &&...args) noexcept(std::is_nothrow_constructible_v<right_internal_t, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }

  //! Emplace right (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<right_internal_t, std::initializer_list<U> &, Args...> &&
             detail::either_::IsNotVoid<right_t>)
  constexpr right_internal_t &emplaceRight(std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<right_internal_t, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), ilist, std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }

  //! Emplace right (void specialization)
  constexpr void emplaceRight() noexcept
    requires(detail::either_::IsVoid<right_t>)
  {
    destroy();
    std::construct_at(std::addressof(right_));
    is_left_ = false;
  }

  /*!
   * \brief Applies \p f to the left alternative and returns the result.
   *
   * Has no effect if either contains the right alternative.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simple
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(left, a, b, c) -> either;
   *
   * // Equivalent to f(left, a, b, c)
   * eitherObj.and_then_left(f, a, b, c);
   *
   * // Equivalent to f(a, left, b, c)
   * either.and_then_left(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the left alternative. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \f must be invocable with the final combination of arguments and return \c frt::either<L, R>. The \p L
   * type can be changed, but the \p R type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::either_::IsNotVoid<left_t>)
  [[nodiscard]] constexpr auto and_then_left(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::either_::is_nothrow_invocable_with_extra_args<
          F, decltype(std::forward_like<Self>(std::declval<left_t>())), Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<left_t>()));

    static_assert(!detail::either_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain std::unwrapped.");
    static_assert(detail::either_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one std::unwrapped.");
    static_assert(detail::either_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of a value type shouldn't go first.");

    using ret_t = detail::either_::invoke_result_t<F, fwd_t, Args...>;

    static_assert(IsEither<ret_t>, "The function must return an either");
    static_assert(detail::either_::same_right_alternative<either, ret_t>,
                  "The function must return an either with the same right alternative");

    if (self.is_left()) [[likely]] {
      return detail::either_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).left(),
                                                     std::forward<Args>(args)...);
    } else {
      if constexpr (detail::either_::IsVoid<right_t>) {
        return ret_t{in_place_right};
      } else {
        return ret_t{in_place_right, std::forward<Self>(self).right()};
      }
    }
  }

  /*!
   * \brief The specialization of \c and_then_left() when the left alternative is \p void.
   *
   * Has no effect if either contains the right alternative.
   * Using \p frt::unwrapped as an argument to this function is prohibited.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simple
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(a, b, c) -> either;
   *
   * // Equivalent to f(a, b, c)
   * eitherObj.and_then_left(f, a, b, c);
   *
   * \endcode
   *
   * The function \p f must be invocable with all passed arguments and return \c frt::either<L, R>. The \p L type can
   * be changed, but the \p R type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::either_::IsVoid<left_t>)
  [[nodiscard]] constexpr auto and_then_left(this Self &&self, F &&f,
                                             Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
  {
    static_assert(!detail::either_::contains_unwrapped<Args...>,
                  "The trailing argument pack must not contain the frt::unwrapped if value_type is void");
    static_assert(std::is_invocable_v<F, Args...>, "The function must be invocable with all extra arguments.");

    using ret_t = std::invoke_result_t<F, Args...>;

    static_assert(IsEither<ret_t>, "The function must return an either");
    static_assert(detail::either_::same_right_alternative<either, ret_t>,
                  "The function must return an either with the same right alternative");

    if (self.is_left()) [[likely]] {
      return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else {
      if constexpr (detail::either_::IsVoid<right_t>) {
        return ret_t{in_place_right};
      } else {
        return ret_t{in_place_right, std::forward<Self>(self).right()};
      }
    }
  }

  /*!
   * \brief Applies \p f to the right alternative and returns the result.
   *
   * Has no effect if either contains the left alternative.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simple
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(right, a, b, c) -> either;
   *
   * // Equivalent to f(right, a, b, c)
   * eitherObj.and_then_right(f, a, b, c);
   *
   * // Equivalent to f(a, right, b, c)
   * either.and_then_right(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the right alternative. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments and return \c frt::either<L, R>. The \p
   * R type can be changed, but the \p L type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::either_::IsNotVoid<right_t>)
  [[nodiscard]] constexpr auto and_then_right(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::either_::is_nothrow_invocable_with_extra_args<
          F, decltype(std::forward_like<Self>(std::declval<right_t>())), Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<right_t>()));

    static_assert(!detail::either_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain std::unwrapped.");
    static_assert(detail::either_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one std::unwrapped.");
    static_assert(detail::either_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of a value type shouldn't go first.");

    using ret_t = detail::either_::invoke_result_t<F, fwd_t, Args...>;

    static_assert(IsEither<ret_t>, "The function must return an either");
    static_assert(detail::either_::same_left_alternative<either, ret_t>,
                  "The function must return an either with the same left alternative");

    if (self.is_right()) [[likely]] {
      return detail::either_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).right(),
                                                     std::forward<Args>(args)...);
    } else {
      if constexpr (detail::either_::IsVoid<left_t>) {
        return ret_t{in_place_left};
      } else {
        return ret_t{in_place_left, std::forward<Self>(self).left()};
      }
    }
  }

  /*!
   * \brief The specialization of \c and_then_right() when the right alternative is \p void.
   *
   * Has no effect if either contains the left alternative.
   * Using \p frt::unwrapped as an argument to this function is prohibited.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simple
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(a, b, c) -> either;
   *
   * // Equivalent to f(a, b, c)
   * eitherObj.and_then_right(f, a, b, c);
   *
   * \endcode
   *
   * The function \p f must be invocable with all passed arguments and return \c frt::either<L, R>. The \p R type can
   * be changed, but the \p L type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::either_::IsVoid<right_t>)
  [[nodiscard]] constexpr auto and_then_right(this Self &&self, F &&f,
                                              Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
  {
    static_assert(!detail::either_::contains_unwrapped<Args...>,
                  "The trailing argument pack must not contain the frt::unwrapped if value_type is void");
    static_assert(std::is_invocable_v<F, Args...>, "The function must be invocable with all extra arguments.");

    using ret_t = std::invoke_result_t<F, Args...>;

    static_assert(IsEither<ret_t>, "The function must return an either");
    static_assert(detail::either_::same_left_alternative<either, ret_t>,
                  "The function must return an either with the same left alternative");

    if (self.is_right()) [[likely]] {
      return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else {
      if constexpr (detail::either_::IsVoid<left_t>) {
        return ret_t{in_place_left};
      } else {
        return ret_t{in_place_left, std::forward<Self>(self).left()};
      }
    }
  }

private:
  constexpr void destroy()
  {
    if (is_left_) {
      std::destroy_at(std::addressof(left_));
    } else {
      std::destroy_at(std::addressof(right_));
    }
  }

  template <class D>
  constexpr void initFrom(D &&data)
  {
    if (data.is_left_) {
      std::construct_at(std::addressof(left_), std::forward_like<D>(data.left_));
    } else {
      std::construct_at(std::addressof(right_), std::forward_like<D>(data.right_));
    }

    is_left_ = data.is_left_;
  }

  union
  {
    left_internal_t left_;
    right_internal_t right_;
  };
  bool is_left_;
};

} // namespace frt

template <>
struct std::hash<frt::detail::either_::Void>
{
  constexpr std::size_t operator()(const frt::detail::either_::Void &) const noexcept
  {
    return frt::detail::either_::void_hash;
  }
};

//! Hasher for the `frt::either<L, R>` alternative. The result is either hash of the left or of the right alternative.
template <class Left, class Right>
  requires(frt::detail::either_::IsHashable<frt::detail::either_::SelectedType<Left>> &&
           frt::detail::either_::IsHashable<frt::detail::either_::SelectedType<Right>>)
struct std::hash<frt::either<Left, Right>>
{
  constexpr std::size_t operator()(const frt::either<Left, Right> &e) const noexcept
  {
    if (e.is_left()) {
      return std::hash<frt::detail::either_::bare_t<Left>>{}(e.left());
    } else {
      return std::hash<frt::detail::either_::bare_t<Right>>{}(e.right());
    }
  }
};
