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

using namespace detail::common_;

template <class E1, class E2>
constexpr inline bool same_error_type = std::same_as<typename bare_t<E1>::error_type, typename bare_t<E2>::error_type>;

template <class E1, class E2>
constexpr inline bool same_value_type = std::same_as<typename bare_t<E1>::value_type, typename bare_t<E2>::value_type>;

struct InPlaceExpectedTag;
struct InPlaceUnexpectedTag;

template <class T>
inline constexpr bool is_unexpected_ = false;

template <class E>
inline constexpr bool is_unexpected_<unexpected<E>> = true;

template <class T>
concept IsInplaceExpectedTag = std::same_as<bare_t<T>, InPlaceExpectedTag>;

template <class T>
concept IsInplaceUnexpectedTag = std::same_as<bare_t<T>, InPlaceUnexpectedTag>;

template <class T>
concept ValidUnexpectedType =
    std::is_object_v<T> && !std::is_array_v<T> && !is_unexpected_<bare_t<T>> && !std::is_const_v<T> &&
    !std::is_volatile_v<T> && !IsInplaceExpectedTag<T> && !IsInplaceUnexpectedTag<T> && !std::is_function_v<bare_t<T>>;

template <class T>
concept ValidValueType = requires {
  requires std::destructible<bare_t<T>> || IsVoid<T>;
  requires !std::is_reference_v<T>;
  requires !std::is_array_v<bare_t<T>>;
  requires !is_unexpected_<bare_t<T>>;
  requires !std::is_function_v<bare_t<T>>;
  requires !IsInplaceExpectedTag<T>;
  requires !IsInplaceUnexpectedTag<T>;
};

template <class T>
concept ValidErrorType = requires {
  requires std::destructible<bare_t<T>>;
  requires ValidUnexpectedType<T>;
};

} // namespace detail::expected_

template <class T, class E>
struct [[nodiscard]] expected;

namespace detail::expected_
{

template <class T>
inline constexpr bool is_expected_ = false;

template <class T, class E>
inline constexpr bool is_expected_<expected<T, E>> = true;

template <class T>
concept IsExpected = is_expected_<bare_t<T>>;

template <class T>
concept IsUnexpected = is_unexpected_<bare_t<T>>;

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

template <class E, class U, class G>
concept UnexConstructableFromExpected =
    std::is_constructible_v<unexpected<bare_t<E>>, expected<bare_t<U>, bare_t<G>> &> ||
    std::is_constructible_v<unexpected<bare_t<E>>, expected<bare_t<U>, bare_t<G>>> ||
    std::is_constructible_v<unexpected<bare_t<E>>, const expected<bare_t<U>, bare_t<G>> &> ||
    std::is_constructible_v<unexpected<bare_t<E>>, const expected<bare_t<U>, bare_t<G>>>;

template <class T, class U, class G>
concept ConstructableFromExpected = std::is_constructible_v<T, expected<bare_t<U>, bare_t<G>> &> ||
                                    std::is_constructible_v<T, expected<bare_t<U>, bare_t<G>>> ||
                                    std::is_constructible_v<T, const expected<bare_t<U>, bare_t<G>> &> ||
                                    std::is_constructible_v<T, const expected<bare_t<U>, bare_t<G>>>;

template <class T, class U, class G>
concept ExpectedConvertableTo = std::is_convertible_v<expected<bare_t<U>, bare_t<G>> &, T> ||
                                std::is_convertible_v<expected<bare_t<U>, bare_t<G>>, T> ||
                                std::is_convertible_v<const expected<bare_t<U>, bare_t<G>> &, T> ||
                                std::is_convertible_v<const expected<bare_t<U>, bare_t<G>>, T>;

template <class T, class U>
concept ConstructableFrom = (detail::either_::IsVoid<T> && detail::either_::IsVoid<U>) || std::is_constructible_v<T, U>;

template <class T, class U>
concept ConvertibleTo = (detail::either_::IsVoid<T> && detail::either_::IsVoid<U>) || std::is_convertible_v<U, T>;

template <class T, class E, class U, class G>
concept constructable_from_another_expected_impl = // NOTE: directly copied from the doc. Can be simplified

    ConstructableFrom<T, U> &&

    std::is_constructible_v<E, G> &&

    (std::same_as<T, bool> || !(ConstructableFromExpected<T, U, G> || ExpectedConvertableTo<T, U, G>)) &&

    !UnexConstructableFromExpected<E, U, G>;

template <class T, class E, class U, class G>
concept ConstructableFromAnotherExpectedCref =
    constructable_from_another_expected_impl<T, E, c_l_ref_t<U>, c_l_ref_t<G>>;

template <class T, class E, class U, class G>
concept ConstructableFromAnotherExpected = constructable_from_another_expected_impl<T, E, bare_t<U>, bare_t<G>>;

template <class T, class E, class U, class G>
concept noexcept_from_another_impl =
    (detail::either_::IsVoid<T> || std::is_nothrow_constructible_v<either<T, E>, in_place_left_t, U>) &&
    std::is_nothrow_constructible_v<either<T, E>, in_place_right_t, G>;

template <class T, class E, class U, class G>
concept NoexceptFromAnotherCref = noexcept_from_another_impl<T, E, c_l_ref_t<U>, c_l_ref_t<G>>;

template <class T, class E, class U, class G>
concept NoexceptFromAnother = noexcept_from_another_impl<T, E, bare_t<U>, G>;

template <class T, class E, class U, class G>
concept explicit_from_another_impl = !ConvertibleTo<T, U> || !std::is_convertible_v<G, E>;

template <class T, class E, class U, class G>
concept ExplicitFromAnotherCref = explicit_from_another_impl<T, E, c_l_ref_t<U>, c_l_ref_t<G>>;

template <class T, class E, class U, class G>
concept ExplicitFromAnother = explicit_from_another_impl<T, E, bare_t<U>, bare_t<G>>;

// NOTE: it might be nice to have an appropriate ctor in either for this scenario
template <class T, class E, IsExpected Other>
constexpr auto fromOther(Other &&other) -> either<T, E>
{
  if (!other.has_value()) {
    return {in_place_right, std::forward<Other>(other).error()};
  } else {
    if constexpr (IsVoid<T> && IsVoid<typename bare_t<Other>::value_type>) {
      return {in_place_left};
    } else {
      return {in_place_left, std::forward<Other>(other).value()};
    }
  }
}

template <class T, class E, class U, class G>
concept EqualityComparable = requires(const expected<T, E> &lhs, const expected<U, G> &rhs) {
  { lhs.asEither() == rhs.asEither() };
  { lhs.asEither() != rhs.asEither() };
};

template <class T, class E, class U, class G>
concept ThreeWayComparable = requires(const expected<T, E> &lhs, const expected<U, G> &rhs) {
  { lhs.asEither() <=> rhs.asEither() };
};

template <class T, class E, class U, class G>
concept NothrowEqualityComparable =
    EqualityComparable<T, E, U, G> && requires(const expected<T, E> &lhs, const expected<U, G> &rhs) {
      { lhs.asEither() == rhs.asEither() } noexcept;
      { lhs.asEither() != rhs.asEither() } noexcept;
    };

template <class T, class E, class U, class G>
concept NothrowThreeWayComparable =
    ThreeWayComparable<T, E, U, G> && requires(const expected<T, E> &lhs, const expected<U, G> &rhs) {
      { lhs.asEither() <=> rhs.asEither() } noexcept;
    };

template <class T, class E, class L, class R>
concept CopyConstructibleFromEither =
    std::constructible_from<bare_t<T>, c_l_ref_t<L>> && std::constructible_from<bare_t<E>, c_l_ref_t<R>>;

template <class T, class E, class L, class R>
concept NothrowCopyConstructibleFromEither = std::is_nothrow_constructible_v<bare_t<T>, c_l_ref_t<L>> &&
                                             std::is_nothrow_constructible_v<bare_t<E>, c_l_ref_t<R>>;

template <class T, class E, class L, class R>
concept MoveConstructibleFromEither =
    std::constructible_from<bare_t<T>, r_ref_t<L>> && std::constructible_from<bare_t<E>, r_ref_t<R>>;

template <class T, class E, class L, class R>
concept NothrowMoveConstructibleFromEither =
    std::is_nothrow_constructible_v<bare_t<T>, r_ref_t<L>> && std::is_nothrow_constructible_v<bare_t<E>, r_ref_t<R>>;

template <class T, class E, class L, class R>
concept CopyAssignableFromEither =
    std::assignable_from<l_ref_t<T>, c_l_ref_t<L>> && std::assignable_from<l_ref_t<E>, c_l_ref_t<R>>;

template <class T, class E, class L, class R>
concept NothrowCopyAssignableFromEither =
    std::is_nothrow_assignable_v<l_ref_t<T>, c_l_ref_t<L>> && std::is_nothrow_assignable_v<l_ref_t<E>, c_l_ref_t<R>>;

template <class T, class E, class L, class R>
concept MoveAssignableFromEither =
    std::assignable_from<l_ref_t<T>, r_ref_t<L>> && std::assignable_from<l_ref_t<E>, r_ref_t<R>>;

template <class T, class E, class L, class R>
concept NothrowMoveAssignableFromEither =
    std::is_nothrow_assignable_v<l_ref_t<T>, r_ref_t<L>> && std::is_nothrow_assignable_v<l_ref_t<E>, r_ref_t<R>>;

} // namespace detail::expected_

using in_place_expected_t = detail::expected_::InPlaceExpectedTag;
using in_place_unexpected_t = detail::expected_::InPlaceUnexpectedTag;

inline constexpr detail::expected_::InPlaceExpectedTag in_place_expected{
    detail::expected_::InPlaceExpectedTag::Secret{}};
inline constexpr detail::expected_::InPlaceUnexpectedTag in_place_unexpected{
    detail::expected_::InPlaceUnexpectedTag::Secret{}};

template <class T>
concept IsExpected = detail::expected_::IsExpected<T>;

template <class T>
concept IsUnexpected = detail::expected_::IsUnexpected<T>;

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
 * \brief The class template \c frt::expected<T, E> provides a way to represent either of two values: an expected value
 * of type \p T, or an unexpected value of type \p E. \c frt::expected is never valueless.
 *
 * Further in the documentation, "value" is used as a synonym of the "value of expected type" and "error" as a synonym
 * of the "value of unexpected type".
 *
 * This class is \c std::expected<T, E> with extended monadic operations and some extra methods. It's implemented based
 * on \c frt::either<T, E>.
 *
 * Read the official documentation for more info: https://en.cppreference.com/cpp/utility/expected.
 */
template <class T, class E>
struct [[nodiscard]] expected
{
  static_assert(detail::expected_::ValidValueType<T>,
                "The type must either be void, or meet the Destructible requirements (in particular, array and "
                "reference types are not allowed). It's also must not be a function, frt::in_place_expected_t type, "
                "frt::in_place_unexpected_t type, or frt::unexpected<E> type.");
  static_assert(detail::expected_::ValidErrorType<E>,
                "The type must meet the Destructible requirements, and must be a valid template argument for "
                "frt::unexpected (in particular, arrays, non-object types, and cv-qualified types are not allowed). "
                "It's also must not be a function, frt::in_place_expected_t type, frt::in_place_unexpected_t type, or "
                "frt::unexpected<E> type.");
  static_assert(detail::expected_::DifferentTypes<T, E>, "Value and error types must be different.");

  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <class U>
  using rebind_value_type = expected<U, E>;

  template <class U>
  using rebind_error_type = expected<T, U>;

  //! Create with a default value.
  constexpr expected() noexcept(detail::either_::IsVoid<T> || std::is_nothrow_default_constructible_v<T>)
    requires(detail::either_::IsVoid<T> || std::is_default_constructible_v<T>)
      : data_(in_place_left)
  {
  }

  //! Create from another `expected` by copy.
  constexpr expected(const expected &) noexcept(std::is_nothrow_copy_constructible_v<either<T, E>>)
    requires((detail::either_::IsVoid<T> || std::is_copy_constructible_v<T>) && std::is_copy_constructible_v<E>)
  = default;

  //! Create from another `expected` by move.
  constexpr expected(expected &&) noexcept(std::is_nothrow_move_constructible_v<either<T, E>>)
    requires((detail::either_::IsVoid<T> || std::is_move_constructible_v<T>) && std::is_move_constructible_v<E>)
  = default;

  //! Create from `either` by copy.
  template <class L, class R>
    requires(detail::expected_::CopyConstructibleFromEither<T, E, L, R>)
  constexpr expected(const either<L, R> &inEither) noexcept(
      detail::expected_::NothrowCopyConstructibleFromEither<T, E, L, R>)
      : data_(inEither.is_left() ? either<T, E>{in_place_left, inEither.left()}
                                 : either<T, E>{in_place_right, inEither.right()})
  {
  }

  //! Move from `either`.
  template <class L, class R>
    requires(detail::expected_::MoveConstructibleFromEither<T, E, L, R>)
  constexpr expected(either<L, R> &&inEither) noexcept(
      detail::expected_::NothrowMoveConstructibleFromEither<T, E, L, R>)
      : data_(inEither.is_left() ? either<T, E>{in_place_left, std::move(inEither).left()}
                                 : either<T, E>{in_place_right, std::move(inEither).right()})
  {
  }

  //! Create from another `expected` by copy.
  template <class U, class G>
  constexpr explicit(detail::expected_::ExplicitFromAnotherCref<T, E, U, G>)
      expected(const expected<U, G> &other) noexcept(detail::expected_::NoexceptFromAnotherCref<T, E, U, G>)
    requires(detail::expected_::ConstructableFromAnotherExpectedCref<T, E, U, G>)
      : data_(detail::expected_::fromOther<T, E>(other))
  {
  }

  //! Create from another `expected` by move.
  template <class U, class G>
  constexpr explicit(detail::expected_::ExplicitFromAnother<T, E, U, G>)
      expected(expected<U, G> &&other) noexcept(detail::expected_::NoexceptFromAnother<T, E, U, G>)
    requires(detail::expected_::ConstructableFromAnotherExpected<T, E, U, G>)
      : data_(detail::expected_::fromOther<T, E>(std::move(other)))
  {
  }

  //! Create from a value.
  template <class U = std::remove_cv_t<T>>
    requires(detail::either_::IsNotVoid<T> && !std::same_as<detail::either_::bare_t<U>, in_place_expected_t> &&
             !std::same_as<detail::either_::bare_t<U>, expected> && std::is_constructible_v<T, U> && !IsUnexpected<U> &&
             (!std::same_as<value_type, bool> || !IsExpected<U>))
  constexpr explicit(!std::is_convertible_v<U, T>)
      expected(U &&v) noexcept(std::is_nothrow_constructible_v<either<T, E>, in_place_left_t, U>)
      : data_(in_place_left, std::forward<U>(v))
  {
  }

  //! Create from `unexpected` by copy.
  template <class G = E>
    requires(std::is_constructible_v<E, const G &>)
  constexpr explicit(!std::is_convertible_v<const G &, E>) expected(const unexpected<G> &e) noexcept(
      std::is_nothrow_constructible_v<either<T, E>, in_place_right_t, const G &>)
      : data_(in_place_right, e.error())
  {
  }

  //! Create from `unexpected` by move.
  template <class G = E>
    requires(std::is_constructible_v<E, G>)
  constexpr explicit(!std::is_convertible_v<G, E>)
      expected(unexpected<G> &&e) noexcept(std::is_nothrow_constructible_v<either<T, E>, in_place_right_t, G &&>)
      : data_(in_place_right, std::move(e).error())
  {
  }

  //! Create from the expected value in-place (with arguments).
  template <class... Args>
    requires(detail::either_::IsNotVoid<T> && std::is_constructible_v<T, Args...>)
  constexpr explicit expected(in_place_expected_t, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<either<T, E>, in_place_left_t, Args...>)
      : data_(in_place_left, std::forward<Args>(args)...)
  {
  }

  //! Create from the expected value in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(detail::either_::IsNotVoid<T> && std::is_constructible_v<T, std::initializer_list<U> &, Args...>)
  constexpr explicit expected(in_place_expected_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<either<T, E>, in_place_left_t, std::initializer_list<U> &, Args...>)
      : data_(in_place_left, ilist, std::forward<Args>(args)...)
  {
  }

  //! Create from the expected value in-place (void specialization).
  constexpr explicit expected(in_place_expected_t) noexcept(
      std::is_nothrow_constructible_v<either<T, E>, in_place_left_t>)
    requires(detail::either_::IsVoid<T>)
      : data_(in_place_left)
  {
  }

  //! Create from the unexpected value in-place (with arguments).
  template <class... Args>
    requires(std::is_constructible_v<E, Args...>)
  constexpr explicit expected(in_place_unexpected_t, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<either<T, E>, in_place_right_t, Args...>)
      : data_(in_place_right, std::forward<Args>(args)...)
  {
  }

  //! Create from the unexpected value in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::is_constructible_v<E, std::initializer_list<U> &, Args...>)
  constexpr explicit expected(in_place_unexpected_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<either<T, E>, in_place_right_t, std::initializer_list<U> &, Args...>)
      : data_(in_place_right, ilist, std::forward<Args>(args)...)
  {
  }

  //! Assign from another `expected` by copy.
  expected &operator=(const expected &) noexcept(std::is_nothrow_copy_assignable_v<either<T, E>>)
    requires(std::is_copy_assignable_v<either<T, E>>)
  = default;

  //! Assign from another `expected` by move.
  expected &operator=(expected &&) noexcept(std::is_nothrow_move_assignable_v<either<T, E>>)
    requires(std::is_move_assignable_v<either<T, E>>)
  = default;

  //! Assign from `either` by copy.
  template <class L, class R>
    requires(detail::expected_::CopyAssignableFromEither<T, E, L, R>)
  constexpr expected &
  operator=(const either<L, R> &src) noexcept(detail::expected_::NothrowCopyAssignableFromEither<T, E, L, R>)
  {
    if (src.is_left()) {
      data_.emplaceLeft(src.left());
    } else {
      data_.emplaceRight(src.right());
    }

    return *this;
  }

  //! Assign from another `either` by move.
  template <class L, class R>
    requires(detail::expected_::MoveAssignableFromEither<T, E, L, R>)
  constexpr expected &
  operator=(either<L, R> &&src) noexcept(detail::expected_::NothrowMoveAssignableFromEither<T, E, L, R>)
  {
    if (src.is_left()) {
      data_.emplaceLeft(std::move(src).left());
    } else {
      data_.emplaceRight(std::move(src).right());
    }

    return *this;
  }

  //! Access the contained value through a pointer. Pre-condition: it must contain a value.
  [[nodiscard]] constexpr const T *operator->() const noexcept
    requires(detail::expected_::IsNotVoid<T>)
  {
    FRT_INTERNAL_ASSERT(has_value(), "must contain value");
    return std::addressof(data_.left());
  }

  //! Access the contained value through a pointer. Pre-condition: it must contain a value.
  [[nodiscard]] constexpr T *operator->() noexcept
    requires(detail::expected_::IsNotVoid<T>)
  {
    FRT_INTERNAL_ASSERT(has_value(), "must contain value");
    return std::addressof(data_.left());
  }

  //! Access the contained value. Pre-condition: it must contain a value.
  template <class Self>
    requires(detail::expected_::IsNotVoid<T>)
  [[nodiscard]] constexpr auto &&operator*(this Self &&self) noexcept
  {
    FRT_INTERNAL_ASSERT(self.has_value(), "must contain value");
    return std::forward_like<Self>(self.data_).left();
  }

  //! Access the contained value (void specialization). Pre-condition: it must contain a value.
  constexpr void operator*() const noexcept
    requires(detail::expected_::IsVoid<T>)
  {
    FRT_INTERNAL_ASSERT(has_value(), "must contain value");
  }

  //! Get value. Pre-condition: it must contain a value.
  template <class Self>
    requires(detail::expected_::IsNotVoid<T>)
  [[nodiscard]] constexpr auto &&value(this Self &&self) noexcept
  {
    FRT_INTERNAL_ASSERT(self.has_value(), "must contain value");
    return std::forward_like<Self>(self.data_).left();
  }

  //! Returns the contained value or \p default_value if the object contains an error.
  template <class U = std::remove_cv_t<T>>
    requires(detail::expected_::IsNotVoid<T> && std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>)
  [[nodiscard]] constexpr T value_or(U &&default_value) const & noexcept(std::is_nothrow_copy_constructible_v<T> &&
                                                                         std::is_nothrow_convertible_v<U, T>)
  {
    return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
  }

  //! Returns the contained value or \p default_value if the object contains an error.
  template <class U = std::remove_cv_t<T>>
    requires(detail::expected_::IsNotVoid<T> && std::is_move_constructible_v<T> && std::is_convertible_v<U, T>)
  [[nodiscard]] constexpr T value_or(U &&default_value) && noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                                    std::is_nothrow_convertible_v<U, T>)
  {
    return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
  }

  //! Get error. Pre-condition: it must contain an error.
  template <class Self>
    requires(detail::expected_::IsNotVoid<error_type>)
  [[nodiscard]] constexpr auto &&error(this Self &&self) noexcept
  {
    FRT_INTERNAL_ASSERT(!self.has_value(), "must contain error");
    return std::forward_like<Self>(self.data_).right();
  }

  //! Returns the contained error or \p default_value if the object contains a value.
  template <class G = E>
    requires(std::is_copy_constructible_v<E> && std::is_convertible_v<G, E>)
  [[nodiscard]] constexpr E error_or(G &&default_value) const & noexcept(std::is_nothrow_copy_constructible_v<E> &&
                                                                         std::is_nothrow_convertible_v<G, E>)
  {
    return has_value() ? std::forward<G>(default_value) : error();
  }

  //! Returns the contained error or \p default_value if the object contains a value.
  template <class G = E>
    requires(std::is_move_constructible_v<E> && std::is_convertible_v<G, E>)
  [[nodiscard]] constexpr E error_or(G &&default_value) && noexcept(std::is_nothrow_move_constructible_v<E> &&
                                                                    std::is_nothrow_convertible_v<G, E>)
  {
    return has_value() ? std::forward<G>(default_value) : std::move(error());
  }

  //! Checks whether the object contains an expected value.
  [[nodiscard]] constexpr explicit operator bool() const noexcept { return has_value(); }

  //! Checks whether the object contains an expected value.
  [[nodiscard]] constexpr bool has_value() const noexcept { return data_.is_left(); }

  /*!
   * \brief Applies \p f to the contained value and returns the result.
   *
   * Has no effect if expected contains an error.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(val, a, b, c) -> expected;
   *
   * // Equivalent to f(val, a, b, c)
   * expectedObj.and_then(f, a, b, c);
   *
   * // Equivalent to f(a, val, b, c)
   * expectedObj.and_then(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the contained value. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments and return \c frt::expected<U, E>. The
   * \p U type can be changed, but the \p E type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::expected_::IsNotVoid<value_type>)
  [[nodiscard]] constexpr auto and_then(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::common_::is_nothrow_invocable_with_extra_args<
          F, decltype(std::forward_like<Self>(std::declval<value_type>())), Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<value_type>()));

    static_assert(!detail::common_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain frt::unwrapped.");
    static_assert(detail::common_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one frt::unwrapped.");
    static_assert(detail::common_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value of value_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of value_type shouldn't go first.");

    using ret_t = detail::common_::invoke_result_t<F, fwd_t, Args...>;

    static_assert(IsExpected<ret_t>, "The function must return an expected");
    static_assert(detail::expected_::same_error_type<expected, ret_t>,
                  "The function must return an expected with the same error type");

    if (self.has_value()) {
      return detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).value(),
                                                     std::forward<Args>(args)...);
    } else {
      return std::forward<Self>(self).template forwardError<ret_t>();
    }
  }

  /*!
   * \brief The specialization of \c and_then() when the value is \p void.
   *
   * Has no effect if expected contains an error.
   * Using \p frt::unwrapped as an argument to this function is prohibited.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(a, b, c) -> expected;
   *
   * // Equivalent to f(a, b, c)
   * expectedObj.and_then(f, a, b, c);
   * \endcode
   *
   * The function \p f must be invocable with all passed arguments and return \c frt::expected<U, E>. The \p U type can
   * be changed, but the \p E type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::expected_::IsVoid<value_type>)
  [[nodiscard]] constexpr auto and_then(this Self &&self, F &&f,
                                        Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
  {
    static_assert(!detail::common_::contains_unwrapped<Args...>,
                  "The trailing argument pack must not contain the frt::unwrapped if value_type is void");
    static_assert(std::is_invocable_v<F, Args...>, "The function must be invocable with all extra arguments.");

    using ret_t = std::invoke_result_t<F, Args...>;

    static_assert(IsExpected<ret_t>, "The function must return an expected");
    static_assert(detail::expected_::same_error_type<expected, ret_t>,
                  "The function must return an expected with the same error type");

    if (self.has_value()) {
      return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else {
      return std::forward<Self>(self).template forwardError<ret_t>();
    }
  }

  /*!
   * \brief Applies \p f to the error and returns the result.
   *
   * Has no effect if expected contains a value.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(err, a, b, c) -> expected;
   *
   * // Equivalent to f(err, a, b, c)
   * expectedObj.or_else(f, a, b, c);
   *
   * // Equivalent to f(a, err, b, c)
   * expectedObj.or_else(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the error. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments and return \c frt::expected<T, G>. The
   * \p G type can be changed, but the \p T type must remain the same.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::expected_::IsNotVoid<error_type>)
  [[nodiscard]] constexpr auto or_else(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::common_::is_nothrow_invocable_with_extra_args<
          F, decltype(std::forward_like<Self>(std::declval<error_type>())), Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<error_type>()));

    static_assert(!detail::common_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain frt::unwrapped.");
    static_assert(detail::common_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one frt::unwrapped.");
    static_assert(detail::common_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value of error_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of error_type shouldn't go first.");

    using ret_t = detail::common_::invoke_result_t<F, fwd_t, Args...>;

    static_assert(IsExpected<ret_t>, "The function must return an expected");
    static_assert(detail::expected_::same_value_type<expected, ret_t>,
                  "The function must return an expected with the same value type");

    if (!self.has_value()) {
      return detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).error(),
                                                     std::forward<Args>(args)...);
    } else {
      return std::forward<Self>(self).template forwardValue<ret_t>();
    }
  }

  /*!
   * \brief Applies \p f to the contained value and returns the result.
   *
   * Has no effect if expected contains an error.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(val, a, b, c) -> T;
   *
   * // Equivalent to frt::expected<T, E>{frt::in_place_expected, f(val, a, b, c)}
   * // Note that the type for the value may differ from the original one
   * frt::expected<V, E> expectedObj = ...;
   * frt::expected<T, E> result = expectedObj.transform(f, a, b, c);
   *
   * // Equivalent to f(a, val, b, c)
   * expectedObj.transform(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the contained value. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments. The return value will automatically be
   * wrapped into \c frt::expected<T, E>, where T is the return type of \p f. The type \c T may differ from the original
   * \c V. If the function \p f returns nothing, then the type for the value is \c void.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::expected_::IsNotVoid<value_type>)
  [[nodiscard]] constexpr auto transform(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::common_::is_nothrow_invocable_with_extra_args<
          F, decltype(std::forward_like<Self>(std::declval<value_type>())), Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<value_type>()));

    static_assert(!detail::common_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain frt::unwrapped.");
    static_assert(detail::common_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one frt::unwrapped.");
    static_assert(detail::common_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value of value_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of value_type shouldn't go first.");

    using func_ret_t = detail::common_::invoke_result_t<F, fwd_t, Args...>;
    using ret_t = expected<detail::common_::bare_t<func_ret_t>, error_type>;

    if (self.has_value()) {
      if constexpr (detail::expected_::IsVoid<func_ret_t>) {
        detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).value(),
                                                std::forward<Args>(args)...);
        return ret_t{in_place_expected};
      } else {
        return ret_t{in_place_expected,
                     detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).value(),
                                                             std::forward<Args>(args)...)};
      }
    } else {
      return std::forward<Self>(self).template forwardError<ret_t>();
    }
  }

  /*!
   * \brief The specialization of \c transform() when the value is \p void.
   *
   * Has no effect if expected contains an error.
   * Using \p frt::unwrapped as an argument to this function is prohibited.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(a, b, c) -> T;
   *
   * // Equivalent to frt::expected<T, E>{frt::in_place_expected, f(a, b, c)}
   * // Note that the type for the value may differ from the original one
   * frt::expected<void, E> expectedObj = ...;
   * frt::expected<T, E> result = expectedObj.transform(f, a, b, c);
   * \endcode
   *
   * The function \p f must be invocable with the final combination of arguments. The return value will automatically be
   * wrapped into \c frt::expected<T, E>, where T is the return type of \p f. The type \c T may differ from the original
   * \c void. If the function \p f returns nothing, then the type for the value is \c void.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::expected_::IsVoid<value_type>)
  [[nodiscard]] constexpr auto transform(this Self &&self, F &&f,
                                         Args &&...args) noexcept(std::is_nothrow_invocable_v<F, Args...>)
  {
    static_assert(!detail::common_::contains_unwrapped<Args...>,
                  "The trailing argument pack must not contain the frt::unwrapped if value_type is void");
    static_assert(std::is_invocable_v<F, Args...>, "The function must be invocable with all extra arguments.");

    using func_ret_t = std::invoke_result_t<F, Args...>;
    using ret_t = expected<detail::common_::bare_t<func_ret_t>, error_type>;

    if (self.has_value()) {
      if constexpr (detail::expected_::IsVoid<func_ret_t>) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        return ret_t{in_place_expected};
      } else {
        return ret_t{in_place_expected, std::invoke(std::forward<F>(f), std::forward<Args>(args)...)};
      }
    } else {
      return std::forward<Self>(self).template forwardError<ret_t>();
    }
  }

  /*!
   * \brief Applies \p f to the error and returns the result.
   *
   * Has no effect if expected contains a value.
   *
   * This is possible to supply a pack of arguments to the function \p f. All arguments are not stored and simply
   * forwarded to the function \p f keeping the same value category and constness.
   *
   * \code{.cpp}
   * auto f(err, a, b, c) -> G;
   *
   * // Equivalent to frt::expected<T, G>{frt::in_place_unexpected, f(err, a, b, c)}
   * // Note that the type for the error may differ from the original one
   * frt::expected<T, E> expectedObj = ...;
   * frt::expected<T, G> result = expectedObj.transform_error(f, a, b, c);
   *
   * // Equivalent to f(a, err, b, c)
   * expectedObj.transform_error(f, a, frt::unwrapped, b, c);
   * \endcode
   *
   * The \p frt::unwrapped will be simply replaced with the error. The pack must contain zero or one
   * frt::unwrapped object. If it's a single-argument pack, then this argument must not be \p frt::unwrapped.
   *
   * The function \p f must be invocable with the final combination of arguments. The return value will automatically be
   * wrapped into \c frt::expected<T, G>, where G is the return type of \p f. The type \c G may differ from the original
   * \c E.
   *
   * \param f is a function to apply.
   *
   * \param args is an optional pack of arguments to supply to the function \p f.
   */
  template <class Self, class F, class... Args>
    requires(detail::expected_::IsNotVoid<error_type>)
  [[nodiscard]] constexpr auto transform_error(this Self &&self, F &&f, Args &&...args) noexcept(
      detail::common_::is_nothrow_invocable_with_extra_args<
          F, decltype(std::forward_like<Self>(std::declval<error_type>())), Args...>)
  {
    using fwd_t = decltype(std::forward_like<Self>(std::declval<error_type>()));

    static_assert(!detail::common_::single_arg_is_unwrapped<Args...>,
                  "The trailing argument pack of a single argument must not contain frt::unwrapped.");
    static_assert(detail::common_::contains_zero_or_one_unwrapped<Args...>,
                  "The trailing argument pack must contain zero or one frt::unwrapped.");
    static_assert(detail::common_::is_invocable_with_extra_args<F, fwd_t, Args...>,
                  "The function must be invocable with a value of error_type and all extra arguments. "
                  "Consider using frt::unwrapped if an argument of error_type shouldn't go first.");

    using func_ret_t = detail::common_::invoke_result_t<F, fwd_t, Args...>;
    using ret_t = expected<value_type, detail::common_::bare_t<func_ret_t>>;

    if (!self.has_value()) {
      return ret_t{in_place_unexpected,
                   detail::common_::invoke_with_extra_args(std::forward<F>(f), std::forward<Self>(self).error(),
                                                           std::forward<Args>(args)...)};
    } else {
      return std::forward<Self>(self).template forwardValue<ret_t>();
    }
  }

  /*!
   * \brief Invoke \p vf for the value or \p ef for the error and return the result.
   *
   * This function works similarly to pattern matching. It takes into account the value category and constness of the
   * `expected` object. It means, for example, that both \p vf and \p ef can modify contained values and accept them by
   * rvalue reference.
   *
   * The \p vf function must be invocable for the value. If the value has type \c void, then \p vf
   * must accept no arguments.
   * The \p ef function must be invocable for the error.
   *
   * Both functions \p vf and \p ef must have the same return type.
   *
   * \param vf is a function to apply to the value.
   * \param ef is a function to apply to the error.
   */
  template <class Self, class ValueFunc, class ErrorFunc>
    requires(!detail::common_::IsVoid<
             detail::common_::InvocableResultT<ValueFunc, detail::common_::ForwardAlternativeT<Self, value_type>>>)
  [[nodiscard]] constexpr decltype(auto) fold(this Self &&self, ValueFunc vf, ErrorFunc ef) noexcept(
      detail::common_::NothrowInvocableV<ValueFunc, detail::common_::ForwardAlternativeT<Self, value_type>> &&
      detail::common_::NothrowInvocableV<ErrorFunc, detail::common_::ForwardAlternativeT<Self, error_type>>)
  {
    using fwd_val_t = detail::common_::ForwardAlternativeT<Self, value_type>;
    using fwd_err_t = detail::common_::ForwardAlternativeT<Self, error_type>;

    static_assert(detail::common_::InvocableV<ValueFunc, fwd_val_t>,
                  "The value function must be invocable for the value. If the value has type void, "
                  "then the value function must take no arguments.");
    static_assert(detail::common_::InvocableV<ErrorFunc, fwd_err_t>,
                  "The error function must be invocable for the error.");
    static_assert(detail::common_::SameReturnType<ValueFunc, fwd_val_t, ErrorFunc, fwd_err_t>,
                  "Both value and error functions must have the same return type.");

    if (self.has_value()) {
      if constexpr (detail::expected_::IsVoid<value_type>) {
        return std::invoke(vf);
      } else {
        return std::invoke(vf, std::forward<Self>(self).value());
      }
    } else {
      return std::invoke(ef, std::forward<Self>(self).error());
    }
  }

  /*!
   * \brief Invoke \p vf for the value or \p ef for the error.
   *
   * This function works similarly to pattern matching. It takes into account the value category and constness of the
   * `expected` object. It means, for example, that both \p vf and \p ef can modify contained values and accept them by
   * rvalue reference.
   *
   * The \p vf function must be invocable for the value. If the value has type \c void, then \p vf
   * must accept no arguments.
   * The \p ef function must be invocable for the error.
   *
   * Both functions \p vf and \p ef must not return.
   *
   * \param vf is a function to apply to the value.
   * \param ef is a function to apply to the error.
   */
  template <class Self, class ValueFunc, class ErrorFunc>
    requires(detail::common_::IsVoid<
             detail::common_::InvocableResultT<ValueFunc, detail::common_::ForwardAlternativeT<Self, value_type>>>)
  constexpr void fold(this Self &&self, ValueFunc vf, ErrorFunc ef) noexcept(
      detail::common_::NothrowInvocableV<ValueFunc, detail::common_::ForwardAlternativeT<Self, value_type>> &&
      detail::common_::NothrowInvocableV<ErrorFunc, detail::common_::ForwardAlternativeT<Self, error_type>>)
  {
    using fwd_val_t = detail::common_::ForwardAlternativeT<Self, value_type>;
    using fwd_err_t = detail::common_::ForwardAlternativeT<Self, error_type>;

    static_assert(detail::common_::InvocableV<ValueFunc, fwd_val_t>,
                  "The value function must be invocable for the value. If the value has type void, "
                  "then the value function must take no arguments.");
    static_assert(detail::common_::InvocableV<ErrorFunc, fwd_err_t>,
                  "The error function must be invocable for the error.");
    static_assert(detail::common_::SameReturnType<ValueFunc, fwd_val_t, ErrorFunc, fwd_err_t>,
                  "Both value and error functions must not return.");

    if (self.has_value()) {
      if constexpr (detail::expected_::IsVoid<value_type>) {
        std::invoke(vf);
      } else {
        std::invoke(vf, std::forward<Self>(self).value());
      }
    } else {
      return std::invoke(ef, std::forward<Self>(self).error());
    }
  }

  /*!
   * \brief Invoke \p f for the value or for the error and return the result.
   *
   * The function \p f must be invocable for both value and error. This function is implemented in terms of a fold that
   * accepts two functions, but in this case, the function is the same, i.e. fold(f, f).
   *
   * This function has the same contract as fold that accepts two functions. See its description for more details.
   *
   * This function is useful when it's required to access the same interface of two distinct objects. E.g.
   * \code{.cpp}
   * auto str = expectedObj.fold([](const std::formattable<char> auto& obj) { return std::format("{}", obj); }
   * \endcode
   */
  template <class Self, class Func>
    requires(!detail::common_::IsVoid<
             detail::common_::InvocableResultT<Func, detail::common_::ForwardAlternativeT<Self, value_type>>>)
  [[nodiscard]] constexpr decltype(auto) fold(this Self &&self, Func f) noexcept(
      detail::common_::NothrowInvocableV<Func, detail::common_::ForwardAlternativeT<Self, value_type>> &&
      detail::common_::NothrowInvocableV<Func, detail::common_::ForwardAlternativeT<Self, error_type>>)
  {
    return std::forward<Self>(self).fold(f, f);
  }

  /*!
   * \brief Invoke \p f for the value or for the error.
   *
   * The function \p f must be invocable for both value and error and must not return. This function is implemented in
   * terms of a fold that accepts two functions, but in this case, the function is the same, i.e. fold(f, f).
   *
   * This function has the same contract as fold that accepts two functions. See its description for more details.
   *
   * This function is useful when it's required to access the same interface of two distinct objects. E.g.
   * \code{.cpp}
   * expectedObj.fold([](const std::formattable<char> auto& obj) { std::println("{}", obj); }
   * \endcode
   */
  template <class Self, class Func>
    requires(detail::common_::IsVoid<
             detail::common_::InvocableResultT<Func, detail::common_::ForwardAlternativeT<Self, value_type>>>)
  constexpr void fold(this Self &&self, Func f) noexcept(
      detail::common_::NothrowInvocableV<Func, detail::common_::ForwardAlternativeT<Self, value_type>> &&
      detail::common_::NothrowInvocableV<Func, detail::common_::ForwardAlternativeT<Self, error_type>>)
  {
    std::forward<Self>(self).fold(f, f);
  }

  /*!
   * \brief Emplace value in-place (with arguments).
   *
   * Destroys any currently contained value or error and in-place constructs a new value of type \p T from \p args.
   */
  template <class... Args>
    requires(detail::common_::IsNotVoid<T> && std::constructible_from<T, Args...>)
  constexpr decltype(auto) emplace(Args &&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  {
    return data_.emplaceLeft(std::forward<Args>(args)...);
  }

  /*!
   * \brief Emplace value in-place (with an initializer list and arguments).
   *
   * Destroys any currently contained value or error and in-place constructs a new value of type \p T from \p il and \p
   * args.
   */
  template <class U, class... Args>
    requires(detail::common_::IsNotVoid<T> && std::constructible_from<T, std::initializer_list<U> &, Args...>)
  constexpr decltype(auto)
  emplace(std::initializer_list<U> il,
          Args &&...args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args...>)
  {
    return data_.emplaceLeft(il, std::forward<Args>(args)...);
  }

  /*!
   * \brief Emplace value in-place (void specialization).
   *
   * Destroys any currently contained error and makes the object contain a value.
   */
  constexpr void emplace() noexcept
    requires(detail::common_::IsVoid<T>)
  {
    data_.emplaceLeft();
  }

  /*!
   * \brief Swap contents with \p other.
   *
   * Swaps the contents with \p other expected.
   */
  constexpr void swap(expected &other) noexcept(std::is_nothrow_swappable_v<T> && std::is_nothrow_swappable_v<E>)
  {
    data_.swap(other.data_);
  }

  /*!
   * \brief Swap contents with \p other (void specialization).
   *
   * Swaps the contents with \p other expected.
   */
  constexpr void swap(expected &other) noexcept(std::is_nothrow_swappable_v<E>)
    requires(detail::common_::IsVoid<T>)
  {
    data_.swap(other.data_);
  }

  //! Swap contents of \p lhs and \p rhs.
  friend constexpr void swap(expected &lhs, expected &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  //! Compare with another expected.
  template <class U, class G>
    requires(detail::expected_::EqualityComparable<T, E, U, G>)
  [[nodiscard]] friend constexpr bool
  operator==(const expected &lhs,
             const expected<U, G> &rhs) noexcept(detail::expected_::NothrowEqualityComparable<T, E, U, G>)
  {
    return lhs.asEither() == rhs.asEither();
  }

  //! Three-way compare with another expected.
  template <class U, class G>
    requires(detail::expected_::ThreeWayComparable<T, E, U, G>)
  [[nodiscard]] friend constexpr std::partial_ordering
  operator<=>(const expected &lhs,
              const expected<U, G> &rhs) noexcept(detail::expected_::NothrowThreeWayComparable<T, E, U, G>)
  {
    return lhs.asEither() <=> rhs.asEither();
  }

  template <class Self>
  [[nodiscard]] constexpr auto &&asEither(this Self &&self) noexcept
  {
    return std::forward_like<Self>(self.data_);
  }

private:
  // Forward the value of \p self as \p Ret
  template <class Ret, class Self>
  constexpr auto forwardValue(this Self &&self) noexcept(
      detail::expected_::IsVoid<value_type> ||
      std::is_nothrow_constructible_v<Ret, in_place_expected_t, detail::common_::ForwardAlternativeT<Self, value_type>>)
  {
    if constexpr (detail::expected_::IsVoid<value_type>) {
      return Ret{in_place_expected};
    } else {
      return Ret{in_place_expected, std::forward<Self>(self).value()};
    }
  }

  // Forward the error of \p self as \p Ret
  template <class Ret, class Self>
  constexpr auto forwardError(this Self &&self) noexcept(
      std::is_nothrow_constructible_v<Ret, in_place_expected_t, detail::common_::ForwardAlternativeT<Self, error_type>>)
  {
    return Ret{in_place_unexpected, std::forward<Self>(self).error()};
  }

  either<T, E> data_;
};

} // namespace frt
