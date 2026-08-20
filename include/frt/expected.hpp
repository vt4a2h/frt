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

#include <frt/either.hpp>

namespace frt
{

template <class E>
struct unexpected;

namespace detail::expected_
{

using either_::bare_t;
using either_::IsNotVoid;
using either_::IsVoid;

template <class T>
inline constexpr bool is_unexpected_ = false;

template <class E>
inline constexpr bool is_unexpected_<unexpected<E>> = true;

template <class T>
concept ValidUnexpectedType = std::is_object_v<T> && !std::is_array_v<T> && !is_unexpected_<bare_t<T>> &&
                              !std::is_const_v<T> && !std::is_volatile_v<T>;

template <class T>
concept ValidValueType = requires {
  requires std::destructible<bare_t<T>> || IsVoid<T>;
  requires !std::is_reference_v<T>;
  requires !std::is_array_v<bare_t<T>>;
};

template <class T>
concept ValidErrorType = requires {
  requires std::destructible<bare_t<T>>;
  requires ValidUnexpectedType<T>;
};

} // namespace detail::expected_

template <detail::expected_::ValidValueType T, detail::expected_::ValidErrorType E>
struct [[nodiscard]] expected;

namespace detail::expected_
{

template <class T>
inline constexpr bool is_expected_ = false;

template <class T, class E>
inline constexpr bool is_expected_<expected<T, E>> = true;

template <class T, class E>
concept DifferentTypes = !std::same_as<bare_t<T>, bare_t<E>>;

struct InPlaceExpectedTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceExpectedTag(Secret) noexcept {}
};

struct InPlaceUnexpectedTag
{
  struct Secret
  {
    constexpr Secret() noexcept = default;
  };
  constexpr explicit InPlaceUnexpectedTag(Secret) noexcept {}
};

template <class Err, class E>
concept ValidUnexpectedError = !std::same_as<bare_t<Err>, unexpected<E>> &&
                               !std::same_as<bare_t<Err>, InPlaceExpectedTag> && std::constructible_from<E, Err>;

} // namespace detail::expected_

using in_place_expected_t = detail::expected_::InPlaceExpectedTag;
using in_place_unexpected_t = detail::expected_::InPlaceUnexpectedTag;

inline constexpr detail::expected_::InPlaceExpectedTag in_place_expected{
    detail::expected_::InPlaceExpectedTag::Secret{}};
inline constexpr detail::expected_::InPlaceUnexpectedTag in_place_unexpected{
    detail::expected_::InPlaceUnexpectedTag::Secret{}};

template <class T>
concept IsExpected = detail::expected_::is_expected_<detail::either_::bare_t<T>>;

template <class T>
concept IsUnexpected = detail::expected_::is_unexpected_<detail::either_::bare_t<T>>;

/*!
 * \brief The class template frt::unexpected represents an unexpected value stored in frt::expected.
 *
 * In particular, frt::expected has constructors with frt::unexpected as a single argument, which creates an expected
 * object that contains an unexpected value.
 *
 * A program is ill-formed if it instantiates an unexpected with a non-object type, an array type, a specialization of
 * frt::unexpected, or a cv-qualified type.
 */
template <class E>
struct unexpected
{
  static_assert(detail::expected_::ValidUnexpectedType<E>,
                "The type must not be an array type, a non-object type, a specialization of frt::unexpected, or a "
                "cv-qualified type.");

  //! Create from another `unexpected` by copy.
  constexpr unexpected(const unexpected &) = default;

  //! Create from another `unexpected` by move.
  constexpr unexpected(unexpected &&) = default;

  /*!
   * \brief Create `unexpected` from another object.
   *
   * The object must not be another `unexpected` or `in_place_unexpected` tag. The type `E` must be constructable from
   * `Err`.
   */
  template <detail::expected_::ValidUnexpectedError<E> Err = E>
  constexpr explicit unexpected(Err &&e) noexcept(std::is_nothrow_constructible_v<E, Err>) : err_(std::forward<Err>(e))
  {
  }

  /*!
   * \brief Create `unexpected` in-place from arguments.
   *
   * The type `E` must be constructable from arguments.
   */
  template <class... Args>
    requires(std::is_constructible_v<E, Args...>)
  constexpr explicit unexpected(in_place_unexpected_t,
                                Args &&...args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
      : err_(std::forward<Args>(args)...)
  {
  }

  /*!
   * \brief Create `unexpected` in-place from an initializer list and arguments.
   *
   * The type `E` must be constructable from an initializer list and arguments.
   */
  template <class U, class... Args>
    requires(std::is_constructible_v<E, std::initializer_list<U> &, Args...>)
  constexpr explicit unexpected(in_place_unexpected_t, std::initializer_list<U> il, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<E, std::initializer_list<U> &, Args...>)
      : err_(il, std::forward<Args>(args)...)
  {
  }

  //! Assign from another `unexpected` by copy.
  constexpr unexpected &operator=(const unexpected &) = default;
  //! Assign from another `unexpected` by move.
  constexpr unexpected &operator=(unexpected &&) = default;

  //! Get contained error.
  template <class Self>
  [[nodiscard]] constexpr auto &&error(this Self &&self)
  {
    return std::forward_like<Self>(self.err_);
  }

  /*!
   * \brief Swap with \p other unexpected.
   *
   * The objects of type `E` must be swappable.
   */
  constexpr void swap(unexpected &other) noexcept(std::is_nothrow_swappable_v<E>)
    requires(std::is_swappable_v<E>)
  {
    using std::swap;
    swap(err_, other.err_);
  }

  //! Compare with another unexpected.
  template <class U>
  [[nodiscard]] friend constexpr bool
  operator==(const unexpected &lhs,
             const unexpected<U> &rhs) noexcept(noexcept(static_cast<bool>(lhs.error() == rhs.error())))
  {
    return lhs.error() == rhs.error();
  }

  //! Three-way compare with another unexpected.
  template <class U>
  [[nodiscard]] friend constexpr auto
  operator<=>(const unexpected &lhs, const unexpected<U> &rhs) noexcept(noexcept(lhs.error() <=> rhs.error()))
  {
    return lhs.error() <=> rhs.error();
  }

  /*!
   * \brief Swap with \p lhs with \p rhs.
   *
   * The objects of type `E` must be swappable.
   */
  friend constexpr void swap(unexpected &lhs, unexpected &rhs) noexcept(noexcept(lhs.swap(rhs)))
    requires(std::is_swappable_v<E>)
  {
    lhs.swap(rhs);
  }

private:
  E err_;
};

//! The deduction guide is provided for `frt::unexpected` to allow deduction from the constructor argument.
template <class E>
unexpected(E) -> unexpected<E>;

/*!
 * \brief The class template \p frt::expected<T, E> provides a way to represent either of two values: an expected value
 * of type \p T, or an unexpected value of type \p E. expected is never valueless.
 *
 * This class is std::expected<T, E> with extended monadic operations and some extra methods. It's implemented based on
 * frt::either<T, E>.
 *
 * Read the official documentation for more info: https://en.cppreference.com/cpp/utility/expected.
 */
template <detail::expected_::ValidValueType T, detail::expected_::ValidErrorType E>
struct [[nodiscard]] expected
{
  static_assert(detail::expected_::DifferentTypes<T, E>, "Value and error types must be different.");

  using value_type = T;
  using error_type = E;

  //! Get value. Pre-condition: it must contain value.
  template <class Self>
    requires(detail::expected_::IsNotVoid<value_type>)
  [[nodiscard]] constexpr auto &&value(this Self &&self)
  {
    return std::forward_like<Self>(self.data_).left();
  }

  //! Get error. Pre-condition: it must contain error.
  template <class Self>
    requires(detail::expected_::IsNotVoid<error_type>)
  [[nodiscard]] constexpr auto &&error(this Self &&self)
  {
    return std::forward_like<Self>(self.data_).right();
  }

  //! Checks whether object contains an expected value.
  constexpr explicit operator bool() const noexcept { return has_value(); }

  //! Checks whether object contains an expected value.
  [[nodiscard]] constexpr bool has_value() const noexcept { return data_.is_left(); }

private:
  either<T, E> data_;
};

} // namespace frt
