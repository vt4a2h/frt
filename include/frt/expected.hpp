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

namespace detail::expected_
{

using either_::bare_t;
using either_::IsNotVoid;
using either_::IsVoid;

template <class T>
concept ValidValueType = requires {
  requires std::destructible<bare_t<T>> || IsVoid<T>;
  requires !std::is_reference_v<T>;
  requires !std::is_array_v<bare_t<T>>;
};

template <class T>
concept ValidErrorType = requires {
  requires std::destructible<bare_t<T>>;
  requires !std::is_reference_v<T>;
  requires !std::is_array_v<bare_t<T>>;
};

template <class T, class E>
concept DifferentTypes = !std::same_as<bare_t<T>, bare_t<T>>;

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

} // namespace detail::expected_

using in_place_expected_t = detail::expected_::InPlaceExpectedTag;
using in_place_unexpected_t = detail::expected_::InPlaceUnexpectedTag;

inline constexpr detail::expected_::InPlaceExpectedTag in_place_expected{
    detail::expected_::InPlaceExpectedTag::Secret{}};
inline constexpr detail::expected_::InPlaceUnexpectedTag in_place_unexpected{
    detail::expected_::InPlaceUnexpectedTag::Secret{}};

template <class E>
struct unexpected
{
  // TODO

  E err_;
};

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
