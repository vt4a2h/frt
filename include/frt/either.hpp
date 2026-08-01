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

#include <type_traits>
#include <utility>

#include <frt/expects.hpp>

namespace frt
{

namespace detail::either_
{

template <class T>
using bare_t = std::remove_cvref_t<T>;

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
concept IsEither = is_either_<bare_t<T>>;

template <class T>
concept IsNotEither = !is_either_<bare_t<T>>;

struct InPlaceLeftTag
{
  InPlaceLeftTag() = default;
};

struct InPlaceRightTag
{
  InPlaceRightTag() = default;
};

struct CreateLeftTag
{
  CreateLeftTag() = default;
};

struct CreateRightTag
{
  CreateRightTag() = default;
};

template <class T>
concept is_tag_ = std::same_as<T, InPlaceLeftTag> || std::same_as<T, InPlaceRightTag> ||
                  std::same_as<T, CreateLeftTag> || std::same_as<T, CreateRightTag>;

template <class U, class T>
concept ValidConstructableFrom = requires {
  requires std::constructible_from<T, U>;
  requires !is_either_<bare_t<U>>;
  requires !is_tag_<bare_t<U>>;
};

template <class T>
concept IsHashable = requires(T v) {
  { std::hash<T>{}(v) } -> std::same_as<std::size_t>;
} || IsVoid<T>;

template <class L1, class R1, class L2, class R2>
concept EqualityComparable = (std::equality_comparable_with<L1, L2> || (IsVoid<L1> && IsVoid<L2>)) &&
                             (std::equality_comparable_with<R1, R2> || (IsVoid<R1> && IsVoid<R2>));

template <class L1, class R1, class L2, class R2>
concept ThreeWayComparable = (std::three_way_comparable_with<L1, L2> || (IsVoid<L1> && IsVoid<L2>)) &&
                             (std::three_way_comparable_with<R1, R2> || (IsVoid<R1> && IsVoid<R2>));

inline constexpr std::size_t void_hash{};

} // namespace detail::either_

using in_place_left_t = detail::either_::InPlaceLeftTag;
using in_place_right_t = detail::either_::InPlaceRightTag;

inline constexpr in_place_left_t in_place_left = {};
inline constexpr in_place_right_t in_place_right = {};

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
struct either
{
  using left_t = detail::either_::bare_t<Left>;
  using right_t = detail::either_::bare_t<Right>;

  //! The object must be created of either Left or Right. The uninitialized state is not permitted.
  either() = delete;

  //! Create from another `either` by copy.
  constexpr either(const either &src) noexcept(std::is_nothrow_copy_constructible_v<left_t> &&
                                               std::is_nothrow_copy_constructible_v<right_t>)
  {
    initFrom(src);
  }

  //! Create from another `either` by move.
  constexpr either(either &&src) noexcept(std::is_nothrow_move_constructible_v<left_t> &&
                                          std::is_nothrow_move_constructible_v<right_t>)
  {
    initFrom(std::move(src));
  }

  //! Assign from another `either` by copy.
  constexpr either &operator=(const either &src) noexcept(std::is_nothrow_copy_assignable_v<left_t> &&
                                                          std::is_nothrow_copy_assignable_v<right_t>)
  {
    destroy();
    initFrom(src);

    return *this;
  }

  //! Assign from another `either` by move.
  constexpr either &operator=(either &&src) noexcept(std::is_nothrow_move_assignable_v<left_t> &&
                                                     std::is_nothrow_move_assignable_v<right_t>)
  {
    destroy();
    initFrom(std::move(src));

    return *this;
  }

  //! Create from the left alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<left_t, Args...>)
  constexpr either(in_place_left_t, Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, Args...>)
  {
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = true;
  }

  //! Create from the left alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<left_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_left_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<left_t, std::initializer_list<U> &, Args...>)
  {
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = true;
  }

  //! Create from the right alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<right_t, Args...>)
  constexpr either(in_place_right_t, Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, Args...>)
  {
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;
  }

  //! Create from the right alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<right_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_right_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<right_t, std::initializer_list<U> &, Args...>)
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
  [[nodiscard]] constexpr auto &&left(this Self &&self)
  {
    expects(self.is_left());
    return std::forward_like<Self>(self.left_);
  }

  //! Returns the left alternative or \p default value if the object contains the right alternative.
  template <class U = left_t>
    requires(std::is_copy_constructible_v<left_t> && std::is_convertible_v<U, left_t>)
  [[nodiscard]] constexpr left_t left_or(U &&default_value) const &
  {
    return is_left() ? left_ : static_cast<left_t>(std::forward<U>(default_value));
  }

  //! Returns the left alternative or \p default value if the object contains the right alternative.
  template <class U = left_t>
    requires(std::is_move_constructible_v<left_t> && std::is_convertible_v<U, left_t>)
  [[nodiscard]] constexpr left_t left_or(U &&default_value) &&
  {
    return is_left() ? std::move(left_) : static_cast<left_t>(std::forward<U>(default_value));
  }

  //! Get the right alternative. Pre-condition: it must contain the right alternative.
  template <class Self>
  [[nodiscard]] constexpr auto &&right(this Self &&self)
  {
    expects(self.is_right());
    return std::forward_like<Self>(self.right_);
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = right_t>
    requires(std::is_copy_constructible_v<right_t> && std::is_convertible_v<U, right_t>)
  [[nodiscard]] constexpr right_t right_or(U &&default_value) const &
  {
    return is_right() ? right_ : static_cast<right_t>(std::forward<U>(default_value));
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = right_t>
    requires(std::is_move_constructible_v<right_t> && std::is_convertible_v<U, right_t>)
  [[nodiscard]] constexpr right_t right_or(U &&default_value) &&
  {
    return is_right() ? std::move(right_) : static_cast<right_t>(std::forward<U>(default_value));
  }

  //! Compare with another either
  template <class L, class R>
    requires(detail::either_::EqualityComparable<left_t, right_t, L, R>)
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
    requires(detail::either_::ThreeWayComparable<left_t, right_t, L, R>)
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
  constexpr void swap(either &rhs) noexcept(std::is_nothrow_swappable_v<left_t> && std::is_nothrow_swappable_v<right_t>)
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
    requires(std::constructible_from<left_t, Args...>)
  constexpr left_t &emplaceLeft(Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = true;

    return left_;
  }

  //! Emplace left (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<left_t, std::initializer_list<U> &, Args...>)
  constexpr left_t &
  emplaceLeft(std::initializer_list<U> ilist,
              Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = true;

    return left_;
  }

  //! Emplace right (from args)
  template <class... Args>
    requires(std::constructible_from<right_t, Args...>)
  constexpr right_t &emplaceRight(Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }

  //! Emplace right (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<right_t, std::initializer_list<U> &, Args...>)
  constexpr right_t &
  emplaceRight(std::initializer_list<U> ilist,
               Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), ilist, std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
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
    Left left_;
    Right right_;
  };
  bool is_left_;
};

template <detail::either_::ValidType Right>
struct either<void, Right>
{
  using left_t = void;
  using right_t = detail::either_::bare_t<Right>;

  either() = default;

  //! Create from another `either` by copy.
  constexpr either(const either &src) noexcept(std::is_nothrow_copy_constructible_v<right_t>) { initFrom(src); }

  //! Create from another `either` by move.
  constexpr either(either &&src) noexcept(std::is_nothrow_move_constructible_v<right_t>) { initFrom(std::move(src)); }

  //! Assign from another `either` by copy.
  constexpr either &operator=(const either &src) noexcept(std::is_nothrow_copy_assignable_v<right_t>)
  {
    destroy();
    initFrom(src);

    return *this;
  }

  //! Assign from another `either` by move.
  constexpr either &operator=(either &&src) noexcept(std::is_nothrow_move_assignable_v<right_t>)
  {
    destroy();
    initFrom(std::move(src));

    return *this;
  }

  //! Create from the right alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<right_t, Args...>)
  constexpr either(in_place_right_t, Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, Args...>)
  {
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;
  }

  //! Create from the right alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<right_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_right_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<right_t, std::initializer_list<U> &, Args...>)
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

  //! Get the right alternative. Pre-condition: it must contain the right alternative.
  template <class Self>
  [[nodiscard]] constexpr auto &&right(this Self &&self)
  {
    expects(self.is_right());
    return std::forward_like<Self>(self.right_);
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = right_t>
    requires(std::is_copy_constructible_v<right_t> && std::is_convertible_v<U, right_t>)
  [[nodiscard]] constexpr right_t right_or(U &&default_value) const &
  {
    return is_right() ? right_ : static_cast<right_t>(std::forward<U>(default_value));
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = right_t>
    requires(std::is_move_constructible_v<right_t> && std::is_convertible_v<U, right_t>)
  [[nodiscard]] constexpr right_t right_or(U &&default_value) &&
  {
    return is_right() ? std::move(right_) : static_cast<right_t>(std::forward<U>(default_value));
  }

  //! Compare with another either
  template <class L, class R>
    requires(detail::either_::EqualityComparable<left_t, right_t, L, R>)
  [[nodiscard]] friend constexpr bool
  operator==(const either &lhs, const either<L, R> &rhs) noexcept(noexcept(static_cast<bool>(lhs.right_ == rhs.right_)))
  {
    if (lhs.is_left_ != rhs.is_left_) {
      return false;
    }

    if (lhs.is_left_) {
      return true;
    } else {
      return lhs.right_ == rhs.right_;
    }
  }

  //! Three-way compare with another either
  template <class L, class R>
    requires(detail::either_::ThreeWayComparable<left_t, right_t, L, R>)
  [[nodiscard]] friend constexpr std::partial_ordering
  operator<=>(const either &lhs,
              const either<L, R> &rhs) noexcept(noexcept(static_cast<std::partial_ordering>(lhs.right_ <=> rhs.right_)))
  {
    if (lhs.is_left_ == rhs.is_left_) {
      return lhs.is_left_ ? true : lhs.right_ <=> rhs.right_;
    } else {
      return std::partial_ordering::unordered;
    }
  }

  //! Swap value with \p rhs.
  constexpr void swap(either &rhs) noexcept(std::is_nothrow_swappable_v<right_t>)
  {
    using std::swap;
    if (is_left_ != rhs.is_left_) {
      auto tmp = *this;

      this->destroy();
      this->initFrom(rhs);

      rhs.destroy();
      rhs.initFrom(tmp);
    } else if (!is_left_) {
      swap(right_, rhs.right_);
    }
  }

  //! Swap \p lhs and \p rhs.
  friend constexpr void swap(either &lhs, either &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  //! Emplace right (from args)
  template <class... Args>
    requires(std::constructible_from<right_t, Args...>)
  constexpr right_t &emplaceRight(Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }

  //! Emplace right (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<right_t, std::initializer_list<U> &, Args...>)
  constexpr right_t &
  emplaceRight(std::initializer_list<U> ilist,
               Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), ilist, std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }

private:
  constexpr void destroy()
  {
    if (!is_left_) {
      std::destroy_at(std::addressof(right_));
    }
  }

  template <class D>
  constexpr void initFrom(D &&data)
  {
    if (!data.is_left_) {
      std::construct_at(std::addressof(right_), std::forward_like<D>(data.right_));
    }
    is_left_ = data.is_left_;
  }

  union
  {
    Right right_;
  };
  bool is_left_{};
};

template <detail::either_::ValidType Left>
struct either<Left, void>
{
  using left_t = detail::either_::bare_t<Left>;
  using right_t = void;

  either() = default;

  //! Create from another `either` by copy.
  constexpr either(const either &src) noexcept(std::is_nothrow_copy_constructible_v<left_t>) { initFrom(src); }

  //! Create from another `either` by move.
  constexpr either(either &&src) noexcept(std::is_nothrow_move_constructible_v<left_t>) { initFrom(std::move(src)); }

  //! Assign from another `either` by copy.
  constexpr either &operator=(const either &src) noexcept(std::is_nothrow_copy_assignable_v<left_t>)
  {
    destroy();
    initFrom(src);

    return *this;
  }

  //! Assign from another `either` by move.
  constexpr either &operator=(either &&src) noexcept(std::is_nothrow_move_assignable_v<left_t>)
  {
    destroy();
    initFrom(std::move(src));

    return *this;
  }

  //! Create from the right alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<left_t, Args...>)
  constexpr either(in_place_left_t, Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, Args...>)
  {
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = false;
  }

  //! Create from the right alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<left_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_left_t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<left_t, std::initializer_list<U> &, Args...>)
  {
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = false;
  }

  //! Default destructor.
  constexpr ~either() { destroy(); }

  //! Check for the left alternative.
  [[nodiscard]] constexpr bool is_left() const noexcept { return is_left_; }

  //! Check for the right alternative.
  [[nodiscard]] constexpr bool is_right() const noexcept { return !is_left(); }

  //! Get the right alternative. Pre-condition: it must contain the right alternative.
  template <class Self>
  [[nodiscard]] constexpr auto &&right(this Self &&self)
  {
    expects(self.is_right());
    return std::forward_like<Self>(self.left_);
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = left_t>
    requires(std::is_copy_constructible_v<left_t> && std::is_convertible_v<U, left_t>)
  [[nodiscard]] constexpr left_t left_or(U &&default_value) const &
  {
    return is_right() ? left_ : static_cast<left_t>(std::forward<U>(default_value));
  }

  //! Returns the right alternative or \p default value if the object contains the left alternative.
  template <class U = left_t>
    requires(std::is_move_constructible_v<left_t> && std::is_convertible_v<U, left_t>)
  [[nodiscard]] constexpr left_t left_or(U &&default_value) &&
  {
    return is_right() ? std::move(left_) : static_cast<left_t>(std::forward<U>(default_value));
  }

  //! Compare with another either
  template <class L, class R>
    requires(detail::either_::EqualityComparable<left_t, left_t, L, R>)
  [[nodiscard]] friend constexpr bool
  operator==(const either &lhs, const either<L, R> &rhs) noexcept(noexcept(static_cast<bool>(lhs.left_ == rhs.left_)))
  {
    if (lhs.is_left_ != rhs.is_left_) {
      return false;
    }

    if (lhs.is_left_) {
      return true;
    } else {
      return lhs.left_ == rhs.left_;
    }
  }

  //! Three-way compare with another either
  template <class L, class R>
    requires(detail::either_::ThreeWayComparable<left_t, left_t, L, R>)
  [[nodiscard]] friend constexpr std::partial_ordering
  operator<=>(const either &lhs,
              const either<L, R> &rhs) noexcept(noexcept(static_cast<std::partial_ordering>(lhs.left_ <=> rhs.left_)))
  {
    if (lhs.is_left_ == rhs.is_left_) {
      return lhs.is_left_ ? true : lhs.left_ <=> rhs.left_;
    } else {
      return std::partial_ordering::unordered;
    }
  }

  //! Swap value with \p rhs.
  constexpr void swap(either &rhs) noexcept(std::is_nothrow_swappable_v<left_t>)
  {
    using std::swap;
    if (is_left_ != rhs.is_left_) {
      auto tmp = *this;

      this->destroy();
      this->initFrom(rhs);

      rhs.destroy();
      rhs.initFrom(tmp);
    } else if (!is_left_) {
      swap(left_, rhs.left_);
    }
  }

  //! Swap \p lhs and \p rhs.
  friend constexpr void swap(either &lhs, either &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  //! Emplace right (from args)
  template <class... Args>
    requires(std::constructible_from<left_t, Args...>)
  constexpr left_t &emplaceRight(Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = false;

    return left_;
  }

  //! Emplace right (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<left_t, std::initializer_list<U> &, Args...>)
  constexpr left_t &
  emplaceRight(std::initializer_list<U> ilist,
               Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = false;

    return left_;
  }

private:
  constexpr void destroy()
  {
    if (!is_left_) {
      std::destroy_at(std::addressof(left_));
    }
  }

  template <class D>
  constexpr void initFrom(D &&data)
  {
    if (!data.is_left_) {
      std::construct_at(std::addressof(left_), std::forward_like<D>(data.left_));
    }
    is_left_ = data.is_left_;
  }

  union
  {
    Left left_;
  };
  bool is_left_{};
};

//! Weird, but why not? Technically it's possible to have an `either<void, void>`, but practically useless.
template <>
struct either<void, void>
{
  using left_t = void;
  using right_t = void;

  //! The object must be created of either Left or Right. The uninitialized state is not permitted.
  either() = delete;

  //! Create from another `either` by copy.
  constexpr either(const either &src) noexcept = default;

  //! Create from another `either` by move.
  constexpr either(either &&src) noexcept = default;

  //! Assign from another `either` by copy.
  constexpr either &operator=(const either &src) noexcept = default;

  //! Assign from another `either` by move.
  constexpr either &operator=(either &&src) noexcept = default;

  //! Create from the left alternative in-place (with arguments).
  constexpr either(in_place_left_t) noexcept : is_left_(true) {}

  //! Create from the right alternative in-place (with an initializer list and arguments).
  constexpr either(in_place_right_t) noexcept : is_left_(false) {}

  //! Default destructor.
  constexpr ~either() = default;

  //! Check for the left alternative.
  [[nodiscard]] constexpr bool is_left() const noexcept { return is_left_; }

  //! Check for the right alternative.
  [[nodiscard]] constexpr bool is_right() const noexcept { return !is_left(); }

  //! Compare with another either
  [[nodiscard]] friend constexpr bool operator==(const either &lhs, const either &rhs) noexcept = default;

  //! Three-way compare with another either
  [[nodiscard]] friend constexpr std::partial_ordering operator<=>(const either &lhs,
                                                                   const either &rhs) noexcept = default;

  //! Swap value with \p rhs.
  constexpr void swap(either &rhs) noexcept
  {
    using std::swap;
    swap(is_left_, rhs.is_left_);
  }

  //! Swap \p lhs and \p rhs.
  friend constexpr void swap(either &lhs, either &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

private:
  bool is_left_;
};

} // namespace frt

//! Hasher for the `frt::either<L, R>` alternative. The result is either hash of the left or of the right alternative.
template <class Left, class Right>
  requires(frt::detail::either_::IsHashable<Left> && frt::detail::either_::IsHashable<Right>)
struct std::hash<frt::either<Left, Right>>
{
  constexpr std::size_t operator()(const frt::either<Left, Right> &e) const noexcept
  {
    if (e.is_left()) {
      if constexpr (frt::detail::either_::IsVoid<Left>) {
        return frt::detail::either_::void_hash;
      } else {
        return std::hash<frt::detail::either_::bare_t<Left>>{}(e.left());
      }
    } else {
      if constexpr (frt::detail::either_::IsVoid<Right>) {
        return frt::detail::either_::void_hash;
      } else {
        return std::hash<frt::detail::either_::bare_t<Right>>{}(e.right());
      }
    }
  }
};
