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

#include <frt/detail/iterator.hpp>

#include <frt/either.hpp>

namespace frt
{

template <class T>
struct optional;

namespace detail::optional_
{

using namespace detail::common_;

struct InPlaceTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceTag(Secret) noexcept {}
};

struct NullOptTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit NullOptTag(Secret) noexcept {}
};

template <class T>
concept is_tag_ = std::same_as<bare_t<T>, InPlaceTag> || std::same_as<bare_t<T>, NullOptTag>;

template <class>
inline constexpr bool is_optional = false;

template <class T>
inline constexpr bool is_optional<optional<T>> = true;

template <class T>
concept is_optional_ = is_optional<bare_t<T>>;

template <class T>
inline constexpr bool is_bool_ = std::same_as<bool, bare_t<T>>;

template <class T, class U>
concept not_from_optional_ = requires {
  requires !std::constructible_from<bare_t<T>, optional<U>>;
  requires !std::convertible_to<optional<U>, bare_t<T>>;
};

template <class U, class T>
concept CopyConstructibleFromOtherOptional = requires {
  requires !std::same_as<bare_t<T>, bare_t<U>>;
  requires std::constructible_from<bare_t<T>, std::add_const_t<std::add_lvalue_reference_t<bare_t<U>>>>;
  requires is_bool_<T> || not_from_optional_<T, U>;
};

template <class U, class T>
concept CopyConstructibleFromEither = std::constructible_from<bare_t<T>, c_l_ref_t<U>>;

template <class U, class T>
concept NothrowCopyConstructibleFromEither = std::is_nothrow_constructible_v<bare_t<T>, c_l_ref_t<U>>;

template <class U, class T>
concept CopyAssignableFromEither = std::assignable_from<l_ref_t<T>, c_l_ref_t<U>>;

template <class U, class T>
concept NothrowCopyAssignableFromEither = std::is_nothrow_assignable_v<l_ref_t<T>, c_l_ref_t<U>>;

template <class U, class T>
concept MoveConstructibleFromOtherOptional = requires {
  requires !std::same_as<bare_t<T>, bare_t<U>>;
  requires std::constructible_from<bare_t<T>, bare_t<U>>;
  requires is_bool_<T> || not_from_optional_<T, U>;
};

template <class U, class T>
concept MoveConstructibleFromEither = std::constructible_from<bare_t<T>, r_ref_t<U>>;

template <class U, class T>
concept NothrowMoveConstructibleFromEither = std::is_nothrow_constructible_v<bare_t<T>, r_ref_t<U>>;

template <class U, class T>
concept MoveAssignableFromEither = std::assignable_from<l_ref_t<T>, r_ref_t<U>>;

template <class U, class T>
concept NothrowMoveAssignableFromEither = std::is_nothrow_assignable_v<l_ref_t<T>, r_ref_t<U>>;

template <class T>
concept ValidType = requires {
  requires !std::is_reference_v<T>;
  requires !std::same_as<T, void>;
  requires !is_tag_<T>;
  requires !std::is_array_v<bare_t<T>>;
  requires std::destructible<bare_t<T>>;
};

template <class U, class T>
concept ValidOtherType = requires {
  requires ValidType<T>;
  requires std::constructible_from<T, U>;
  requires !std::same_as<bare_t<U>, InPlaceTag>;
  requires !is_optional_<U>;
  requires !is_bool_<T> || !is_optional<U>;
};

template <class T, class U>
concept EqualityComparable = requires(const optional<T> &lhs, const optional<U> &rhs) {
  { lhs.asEither() == rhs.asEither() };
  { lhs.asEither() != rhs.asEither() };
};

template <class T, class U>
concept ThreeWayComparable = requires(const optional<T> &lhs, const optional<U> &rhs) {
  { lhs.asEither() <=> rhs.asEither() };
};

template <class T, class U>
concept NothrowEqualityComparable =
    EqualityComparable<T, U> && requires(const optional<T> &lhs, const optional<U> &rhs) {
      { lhs.asEither() == rhs.asEither() } noexcept;
      { lhs.asEither() != rhs.asEither() } noexcept;
    };

template <class T, class U>
concept NothrowThreeWayComparable =
    ThreeWayComparable<T, U> && requires(const optional<T> &lhs, const optional<U> &rhs) {
      { lhs.asEither() <=> rhs.asEither() } noexcept;
    };

template <class T, class U>
concept EqualityComparableVal = requires(const T &lhs, const U &rhs) {
  { lhs == rhs };
};

template <class T, class U>
concept ThreeWayComparableVal = requires(const T &lhs, const U &rhs) {
  { lhs <=> rhs };
};

template <class T, class U>
concept NothrowEqualityComparableVal = requires(const T &lhs, const U &rhs) {
  { lhs == rhs } noexcept;
};

template <class T, class U>
concept NothrowThreeWayComparableVal = requires(const T &lhs, const U &rhs) {
  { lhs <=> rhs } noexcept;
};

} // namespace detail::optional_

using in_place_opt_t = detail::optional_::InPlaceTag;
using nullopt_t = detail::optional_::NullOptTag;

inline constexpr in_place_opt_t in_place_opt{in_place_opt_t::Secret{}};
inline constexpr nullopt_t nullopt{nullopt_t::Secret{}};

template <class T>
concept IsOptional = detail::optional_::is_optional_<T>;

/*!
 * \brief The class template \c frt::optional manages an optional contained value, i.e., a value that may or may not be
 * present.
 *
 * This class is \c std::optional<T> with extended monadic operations and range support (P3168R2). It's implemented
 * based on \c frt::either<void, T>.
 *
 * A program is ill-formed if it instantiates an optional with a reference type, an array type, void, or a tag type.
 *
 * Read the official documentation for more info: https://en.cppreference.com/w/cpp/utility/optional.html.
 */
template <class T>
struct [[nodiscard]] optional
{
  static_assert(detail::optional_::ValidType<T>,
                "The type T must meet the Destructible requirements. The type must not be a reference, an array, void, "
                "frt::in_place_opt, or frt::nullopt_t.");

  using value_type = T;

  using iterator = detail::optional_::contiguous_iterator<T>;
  using const_iterator = detail::optional_::contiguous_iterator<const T>;

  //! Create an empty `optional`.
  constexpr optional() noexcept = default;

  //! Create an empty `optional` from `nullopt`.
  constexpr explicit(false) optional(nullopt_t) noexcept : data_(in_place_left) {}

  //! Create from another `optional` by copy.
  constexpr optional(const optional &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires(std::is_copy_constructible_v<T>)
      : data_(other.data_)
  {
  }

  //! Create from another `optional` by move.
  constexpr optional(optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires(std::is_move_constructible_v<T>)
      : data_(std::move(other.data_))
  {
  }

  //! Create from `either` by copy.
  template <detail::optional_::CopyConstructibleFromEither<T> U>
  constexpr explicit optional(const either<void, U> &inEither) noexcept(
      detail::optional_::NothrowCopyConstructibleFromEither<T, U>)
      : data_(inEither.is_right() ? either<void, T>{in_place_right, inEither.right()} : either<void, T>{in_place_left})
  {
  }

  //! Move from `either`.
  template <detail::optional_::MoveConstructibleFromEither<T> U>
  constexpr explicit optional(either<void, U> &&inEither) noexcept(
      detail::optional_::NothrowMoveConstructibleFromEither<T, U>)
      : data_(inEither.is_right() ? either<void, T>{in_place_right, std::move(inEither).right()}
                                  : either<void, T>{in_place_left})
  {
  }

  //! Assign from another `optional` by copy.
  constexpr optional &operator=(const optional &other) noexcept(std::is_nothrow_copy_assignable_v<T>)
    requires(std::is_copy_assignable_v<T>)
  {
    data_ = other.data_;
    return *this;
  }

  //! Assign from `either` by copy.
  template <detail::optional_::CopyAssignableFromEither<T> U>
  constexpr optional &
  operator=(const either<void, U> &other) noexcept(detail::optional_::NothrowCopyAssignableFromEither<T, U>)
  {
    if (other.is_right())
      FRT_LIKELY { data_.emplaceRight(other.right()); }
    else {
      reset();
    }

    return *this;
  }

  //! Assign from another `optional` by copy.
  template <detail::optional_::CopyConstructibleFromOtherOptional<T> U>
  constexpr optional &operator=(const optional<U> &other) noexcept(std::is_nothrow_constructible_v<T, const U &>)
  {
    if (other.has_value())
      FRT_LIKELY { data_.emplaceRight(other.value()); }
    else {
      reset();
    }

    return *this;
  }

  //! Assign from another `optional` by move.
  constexpr optional &operator=(optional &&other) noexcept(std::is_nothrow_move_assignable_v<T>)
    requires(std::is_move_assignable_v<T>)
  {
    data_ = std::move(other.data_);
    return *this;
  }

  //! Assign from `either` by move.
  template <detail::optional_::MoveAssignableFromEither<T> U>
  constexpr optional &
  operator=(either<void, U> &&other) noexcept(detail::optional_::NothrowMoveAssignableFromEither<T, U>)
  {
    if (other.is_right())
      FRT_LIKELY { data_.emplaceRight(std::move(other).right()); }
    else {
      reset();
    }

    return *this;
  }

  //! Assign from another `optional` by move.
  template <detail::optional_::MoveConstructibleFromOtherOptional<T> U>
  constexpr optional &operator=(optional<U> &&other) noexcept(std::is_nothrow_constructible_v<T, const U &>)
  {
    if (other.has_value())
      FRT_LIKELY { data_.emplaceRight(std::move(other).value()); }
    else {
      reset();
    }

    return *this;
  }

  //! Assign from `nullopt` (resets the optional).
  constexpr optional &operator=(nullopt_t) noexcept
  {
    reset();
    return *this;
  }

  //! Assign from a value.
  template <detail::optional_::ValidOtherType<T> U = std::remove_cvref_t<T>>
  constexpr optional &operator=(U &&value) noexcept(std::is_nothrow_constructible_v<T, U>)
  {
    data_.emplaceRight(std::forward<U>(value));
    return *this;
  }

  //! Create from another `optional` by copy.
  template <detail::optional_::CopyConstructibleFromOtherOptional<T> U>
  constexpr explicit(!std::convertible_to<const U &, T>)
      optional(const optional<U> &other) noexcept(std::is_nothrow_constructible_v<T, const U &>)
      : data_(other.has_value() ? either<void, T>{in_place_right, other.value()} : either<void, T>{in_place_left})
  {
  }

  //! Create from another `optional` by move.
  template <detail::optional_::MoveConstructibleFromOtherOptional<T> U>
  constexpr explicit(!std::convertible_to<U, T>)
      optional(optional<U> &&other) noexcept(std::is_nothrow_constructible_v<T, U>)
      : data_(other.has_value() ? either<void, T>{in_place_right, std::move(other).value()}
                                : either<void, T>{in_place_left})
  {
  }

  //! Create from the value in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<T, Args...>)
  constexpr explicit optional(in_place_opt_t, Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
      : data_{in_place_right, std::forward<Args>(args)...}
  {
  }

  //! Create from the value in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<T, std::initializer_list<U>, Args...>)
  constexpr explicit optional(in_place_opt_t, std::initializer_list<U> il, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
      : data_{in_place_right, il, std::forward<Args>(args)...}
  {
  }

  //! Create from a value.
  template <detail::optional_::ValidOtherType<T> U = std::remove_cvref_t<T>>
  constexpr explicit(!std::constructible_from<T, U>) optional(U &&value) noexcept(std::is_nothrow_constructible_v<T, U>)
      : data_(in_place_right, std::forward<U>(value))
  {
  }

  //! Checks whether the object contains a value.
  constexpr explicit operator bool() const noexcept { return has_value(); }

  //! Checks whether the object contains a value.
  [[nodiscard]] constexpr bool has_value() const noexcept { return data_.is_right(); }

  //! Get value. Pre-condition: it must contain a value.
  template <class Self>
  [[nodiscard]] constexpr auto &&value(this Self &&self) noexcept
  {
    FRT_INTERNAL_ASSERT(self.has_value(), "must contain value");
    return std::forward_like<Self>(self.data_).right();
  }

  //! Access the contained value. Pre-condition: it must contain a value.
  template <class Self>
  [[nodiscard]] constexpr auto &&operator*(this Self &&self) noexcept
  {
    return std::forward<Self>(self).value();
  }

  //! Access the contained value through a pointer. Pre-condition: it must contain a value.
  constexpr const T *operator->() const noexcept { return &value(); }

  //! Access the contained value through a pointer. Pre-condition: it must contain a value.
  constexpr T *operator->() noexcept { return &value(); }

  //! Returns the contained value or \p default_value if the object does not contain a value.
  template <std::convertible_to<T> U = std::remove_cv_t<T>>
    requires(std::is_copy_constructible_v<T>)
  constexpr T value_or(U &&default_value) const & noexcept(std::is_nothrow_constructible_v<T, U &&>)
  {
    return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
  }

  //! Returns the contained value or \p default_value if the object does not contain a value.
  template <std::convertible_to<T> U = std::remove_cv_t<T>>
    requires(std::is_move_constructible_v<T>)
  constexpr T value_or(U &&default_value) && noexcept(std::is_nothrow_constructible_v<T, U &&>)
  {
    return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
  }

  /*!
   * \brief Swap contents with \p other.
   *
   * Swaps the contents with \p other optional.
   */
  constexpr void swap(optional &other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                std::is_nothrow_swappable_v<T>)
  {
    data_.swap(other.data_);
  }

  //! Swap contents of \p lhs and \p rhs.
  friend constexpr void swap(optional &lhs, optional &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  //! Resets the optional, destroying any contained value and making the object empty.
  constexpr void reset() noexcept { data_.emplaceLeft(); }

  /*!
   * \brief Emplace value in-place (with arguments).
   *
   * Destroys any currently contained value and in-place constructs a new value of type \p T from \p args.
   */
  template <class... Args>
    requires(std::constructible_from<T, Args && ...>)
  constexpr T &emplace(Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
  {
    return data_.emplaceRight(std::forward<Args>(args)...);
  }

  /*!
   * \brief Emplace value in-place (with an initializer list and arguments).
   *
   * Destroys any currently contained value and in-place constructs a new value of type \p T from \p il and \p args.
   */
  template <class U, class... Args>
    requires(std::constructible_from<T, std::initializer_list<U> &, Args && ...>)
  constexpr T &
  emplace(std::initializer_list<U> il,
          Args &&...args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args &&...>)
  {
    return data_.emplaceRight(il, std::forward<Args>(args)...);
  }

  /*!
   * \brief Applies \p f to the contained value and returns the result.
   *
   * Has no effect if optional does not contain a value.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(val, a, b, c) -> optional;
   *
   * // Equivalent to f(val, a, b, c)
   * optionalObj.and_then(f, a, b, c);
   *
   * // Equivalent to f(a, val, b, c)
   * optionalObj.and_then(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the contained value. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments and return \c frt::optional<U>.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
  [[nodiscard]] constexpr auto and_then(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::common_::is_nothrow_invocable_with_extra_args<F, decltype(std::forward_like<Self>(std::declval<T>())),
                                                            Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<T>()));

    static_assert(!detail::common_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain frt::unwrapped.");
    static_assert(detail::common_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one frt::unwrapped.");
    static_assert(detail::common_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value of value_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of value_type shouldn't go first.");

    using ret_t = detail::common_::invoke_result_t<F, fwd_t, Args...>;

    static_assert(IsOptional<ret_t>, "The function must return optional");

    if (self.has_value())
      FRT_LIKELY
      {
        return detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).value(),
                                                       std::forward<Args>(args)...);
      }
    else {
      return ret_t{};
    }
  }

  /*!
   * \brief Applies \p f and returns the result if optional does not contain a value.
   *
   * Has no effect if optional contains a value.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   * Using \p frt::unwrapped as an argument to this function is prohibited.
   *
   * \code{.cpp}
   * auto f(a, b, c) -> optional;
   *
   * // Equivalent to f(a, b, c)
   * optionalObj.or_else(f, a, b, c);
   * \endcode
   *
   * The function \p f must be invocable with all passed arguments and return an optional.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
  [[nodiscard]] constexpr auto or_else(this Self &&self, F &&f,
                                       Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
  {
    static_assert(!detail::common_::contains_unwrapped<Args...>,
                  "The trailing argument pack must contain zero frt::unwrapped.");
    static_assert(std::invocable<F, Args...>,
                  "The function must be invocable with a value of error_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of error_type shouldn't go first.");

    using ret_t = std::invoke_result_t<F, Args...>;

    static_assert(IsOptional<ret_t>, "The function must return optional");

    if (!self.has_value())
      FRT_UNLIKELY { return std::invoke(std::forward<F>(f), std::forward<Args>(args)...); }
    else {
      return ret_t{in_place_opt, std::forward<Self>(self).value()};
    }
  }

  /*!
   * \brief Applies \p f to the contained value and returns the result.
   *
   * Has no effect if optional does not contain a value.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(val, a, b, c) -> U;
   *
   * // Equivalent to frt::optional<U>{frt::in_place_opt, f(val, a, b, c)}
   * // Note that the type for the value may differ from the original one
   * frt::optional<T> optionalObj = ...;
   * frt::optional<U> result = optionalObj.transform(f, a, b, c);
   *
   * // Equivalent to f(a, val, b, c)
   * optionalObj.transform(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the contained value. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments and must not return \c void. The
   * return value will automatically be wrapped into \c frt::optional<U>, where U is the return type of \p f. The type
   * \c U may differ from the original \c T.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
  [[nodiscard]] constexpr auto transform(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::common_::is_nothrow_invocable_with_extra_args<F, decltype(std::forward_like<Self>(std::declval<T>())),
                                                            Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<T>()));

    static_assert(!detail::common_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain frt::unwrapped.");
    static_assert(detail::common_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one frt::unwrapped.");
    static_assert(detail::common_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value of value_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of value_type shouldn't go first.");

    using func_ret_t = detail::common_::invoke_result_t<F, fwd_t, Args...>;

    static_assert(detail::common_::IsNotVoid<func_ret_t>, "The function must not return void");

    using ret_t = optional<detail::common_::bare_t<func_ret_t>>;

    if (self.has_value())
      FRT_LIKELY
      {
        return ret_t{in_place_opt,
                     detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).value(),
                                                             std::forward<Args>(args)...)};
      }
    else {
      return ret_t{};
    }
  }

  /*!
   * \brief Invoke \p vf for the value or \p nf for nullopt (without arguments) and return the result.
   *
   * This function works similarly to pattern matching. It takes into account the value category and constness of the
   * `optional` object. It means, for example, that \p vf can modify the contained value and accept it by
   * rvalue reference.
   *
   * The \p vf function must be invocable for the value.
   * The \p nf function must be invocable without arguments.
   *
   * Both functions \p vf and \p nf must have the same return type.
   *
   * \param vf is a function to apply to the value.
   * \param nf is a function to apply when optional is empty.
   */
  template <class Self, class ValueFunc, class NullOptFunc>
  [[nodiscard]] constexpr decltype(auto) fold(this Self &&self, ValueFunc vf,
                                              NullOptFunc nf) noexcept(std::is_nothrow_invocable_v<ValueFunc, T> &&
                                                                       std::is_nothrow_invocable_v<NullOptFunc>)
  {
    using fwd_val_t = decltype(std::forward_like<Self>(std::declval<T>()));

    static_assert(std::invocable<ValueFunc, fwd_val_t>, "The value function must be invocable for the value.");
    static_assert(std::invocable<NullOptFunc>, "The nullopt function must be invocable without arguments.");
    static_assert(std::same_as<std::invoke_result_t<ValueFunc, fwd_val_t>, std::invoke_result_t<NullOptFunc>>,
                  "Both value and nullopt functions must have the same return type.");

    if (self.has_value())
      FRT_LIKELY { return std::invoke(vf, std::forward<Self>(self).value()); }
    else {
      return std::invoke(nf);
    }
  }

  /*!
   * \brief Invoke \p f for the value or for nullopt (without arguments) and return the result.
   *
   * The function \p f must be invocable for both the value and without arguments. This function is implemented in
   * terms of a fold that accepts two functions, but in this case, the function is the same, i.e. fold(f, f).
   *
   * This function has the same contract as fold that accepts two functions. See its description for more details.
   *
   * \param f is a function to apply to the value or when optional is empty.
   */
  template <class Self, class Func>
  [[nodiscard]] constexpr decltype(auto) fold(this Self &&self, Func f) noexcept(std::is_nothrow_invocable_v<Func, T> &&
                                                                                 std::is_nothrow_invocable_v<Func>)
  {
    return std::forward<Self>(self).fold(f, f);
  }

  //! Compare with another optional.
  template <class U>
    requires(detail::optional_::EqualityComparable<T, U>)
  [[nodiscard]] friend constexpr bool
  operator==(const optional &lhs, const optional<U> &rhs) noexcept(detail::optional_::NothrowEqualityComparable<T, U>)
  {
    return lhs.asEither() == rhs.asEither();
  }

  //! Compare with nullopt.
  friend constexpr bool operator==(const optional &opt, nullopt_t) noexcept { return !opt.has_value(); }

  //! Compare with a value.
  template <class U>
  friend constexpr bool operator==(const optional &opt,
                                   const U &value) noexcept(detail::optional_::NothrowEqualityComparableVal<T, U>)
  {
    return opt.has_value() && opt.value() == value;
  }

  //! Three-way compare with another optional.
  template <class U>
    requires(detail::optional_::ThreeWayComparable<T, U>)
  [[nodiscard]] friend constexpr auto
  operator<=>(const optional &lhs, const optional<U> &rhs) noexcept(detail::optional_::NothrowThreeWayComparable<T, U>)
  {
    return lhs.asEither() <=> rhs.asEither();
  }

  //! Three-way compare with nullopt.
  friend constexpr std::strong_ordering operator<=>(const optional &opt, nullopt_t) noexcept
  {
    return opt.has_value() <=> false;
  }

  //! Three-way compare with a value.
  template <class U>
  friend constexpr std::strong_ordering
  operator<=>(const optional &opt, const U &value) noexcept(detail::optional_::NothrowThreeWayComparableVal<T, U>)
  {
    return opt.has_value() ? opt.value() <=> value : std::strong_ordering::less;
  }

  //! Returns an iterator to the beginning.
  [[nodiscard]] constexpr iterator begin() noexcept
  {
    return iterator{has_value() ? std::addressof(value()) : nullptr};
  }

  //! Returns an iterator to the end.
  [[nodiscard]] constexpr iterator end() noexcept
  {
    return iterator{has_value() ? std::addressof(value()) + 1 : nullptr};
  }

  //! Returns a const iterator to the beginning.
  [[nodiscard]] constexpr const_iterator begin() const noexcept
  {
    return const_iterator{has_value() ? std::addressof(value()) : nullptr};
  }

  //! Returns a const iterator to the end.
  [[nodiscard]] constexpr const_iterator end() const noexcept
  {
    return const_iterator{has_value() ? std::addressof(value()) + 1 : nullptr};
  }

  //! Access the underlying either representation.
  template <class Self>
  [[nodiscard]] constexpr auto &&asEither(this Self &&self) noexcept
  {
    return std::forward_like<Self>(self.data_);
  }

private:
  either<void, T> data_{in_place_left};
};

//! The deduction guide is provided for `frt::optional` to allow deduction from the constructor argument.
template <class T>
optional(T) -> optional<T>;

} // namespace frt
