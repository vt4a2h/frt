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
concept ValidType = requires {
  requires std::destructible<bare_t<T>> || std::is_void_v<bare_t<T>>;
  requires !std::is_reference_v<T>;
  requires !std::is_array_v<bare_t<T>>;
};

struct VoidTag
{
};

} // namespace detail::either_

template <detail::either_::ValidType Left, detail::either_::ValidType Right>
class either;

namespace detail::either_
{
template <class T>
inline constexpr bool is_either_ = false;

template <ValidType L, ValidType R>
inline constexpr bool is_either_<either<L, R>> = true;

template <class T>
concept IsEither = is_either_<bare_t<T>>;

template <class T>
concept IsNotVoid = !std::is_void_v<bare_t<T>>;

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

template <IsNotVoid Left, IsNotVoid Right>
struct Data;

template <class T>
constexpr bool is_data_ = false;

template <class L, class R>
constexpr bool is_data_<Data<L, R>> = true;

template <class T>
concept IsData = is_data_<bare_t<T>>;

template <class T>
concept IsNotData = !is_data_<bare_t<T>>;

template <class T>
concept IsHashable = requires(T v) {
  { std::hash<T>{}(v) } -> std::same_as<std::size_t>;
};

template <class T>
concept SameLeftAndRight = requires {
  requires IsData<T>;
  requires std::same_as<typename bare_t<T>::left_t, typename bare_t<T>::right_t>;
};

template <class Val, class T>
concept EqualityComparableSame = requires {
  requires IsData<T>;
  requires IsNotData<Val>;
  requires SameLeftAndRight<T>;
  requires std::equality_comparable_with<typename bare_t<T>::left_t, Val>;
};

template <class Val, class T>
concept EqualityComparableLeft = requires {
  requires IsData<T>;
  requires IsNotData<Val>;
  requires !SameLeftAndRight<T>;
  requires std::equality_comparable_with<typename bare_t<T>::left_t, bare_t<Val>>;
};

template <class Val, class T>
concept EqualityComparableRight = requires {
  requires IsData<T>;
  requires IsNotData<Val>;
  requires !SameLeftAndRight<T>;
  requires std::equality_comparable_with<typename bare_t<T>::right_t, bare_t<Val>>;
};

template <class Val, class T>
concept ThreeWayComparableSame = requires {
  requires IsData<T>;
  requires IsNotData<Val>;
  requires SameLeftAndRight<T>;
  requires std::three_way_comparable_with<typename bare_t<T>::left_t, Val>;
};

template <class Val, class T>
concept ThreeWayComparableLeft = requires {
  requires IsData<T>;
  requires IsNotData<Val>;
  requires !SameLeftAndRight<T>;
  requires std::three_way_comparable_with<typename bare_t<T>::left_t, bare_t<Val>>;
};

template <class Val, class T>
concept ThreeWayComparableRight = requires {
  requires IsData<T>;
  requires IsNotData<Val>;
  requires !SameLeftAndRight<T>;
  requires std::three_way_comparable_with<typename bare_t<T>::right_t, bare_t<Val>>;
};

template <IsNotVoid Left, IsNotVoid Right>
struct Data
{
  using left_t = bare_t<Left>;
  using right_t = bare_t<Right>;

  Data() noexcept = delete;

  constexpr Data(const Data &data) noexcept(std::is_nothrow_copy_constructible_v<Left> &&
                                            std::is_nothrow_copy_constructible_v<Right>)
  {
    initFrom(data);
  }

  constexpr Data(Data &&data) noexcept(std::is_nothrow_move_constructible_v<Left> &&
                                       std::is_nothrow_move_constructible_v<Right>)
  {
    initFrom(std::move(data));
  }

  constexpr Data &operator=(const Data &data) noexcept(std::is_nothrow_copy_assignable_v<Left> &&
                                                       std::is_nothrow_copy_assignable_v<Right>)
  {
    destroy();
    initFrom(data);

    return *this;
  }

  constexpr Data &operator=(Data &&data) noexcept(std::is_nothrow_move_assignable_v<Left> &&
                                                  std::is_nothrow_move_assignable_v<Right>)
  {
    destroy();
    initFrom(std::move(data));

    return *this;
  }

  template <ValidConstructableFrom<Left> U>
  constexpr Data(CreateLeftTag, U &&u) noexcept(std::is_nothrow_constructible_v<Left, U>)
  {
    std::construct_at(std::addressof(left_), std::forward<U>(u));
    is_left_ = true;
  }

  template <ValidConstructableFrom<Right> U>
  constexpr Data(CreateRightTag, U &&u) noexcept(std::is_nothrow_constructible_v<Right, U>)
  {
    std::construct_at(std::addressof(right_), std::forward<U>(u));
    is_left_ = false;
  }

  template <class... Args>
  constexpr Data(InPlaceLeftTag, Args &&...args) noexcept(std::is_nothrow_constructible_v<Left, Args...>)
  {
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = true;
  }

  template <class U, class... Args>
  constexpr Data(InPlaceLeftTag, std::initializer_list<U> ilist,
                 Args &&...args) noexcept(std::is_nothrow_constructible_v<Left, std::initializer_list<U> &, Args...>)
  {
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = true;
  }

  template <class... Args>
  constexpr Data(InPlaceRightTag, Args &&...args) noexcept(std::is_nothrow_constructible_v<Right, Args...>)
  {
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;
  }

  template <class U, class... Args>
  constexpr Data(InPlaceRightTag, std::initializer_list<U> ilist,
                 Args &&...args) noexcept(std::is_nothrow_constructible_v<Right, std::initializer_list<U> &, Args...>)
  {
    std::construct_at(std::addressof(right_), ilist, std::forward<Args>(args)...);
    is_left_ = false;
  }

  constexpr ~Data() { destroy(); }

  constexpr void destroy()
  {
    if (is_left_) {
      std::destroy_at(std::addressof(left_));
    } else {
      std::destroy_at(std::addressof(right_));
    }
  }

  template <IsData D>
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

  friend constexpr bool operator==(const Data &lhs, const Data &rhs) noexcept
  {
    return lhs.is_left_ == rhs.is_left_ && (lhs.is_left_ ? lhs.left_ == rhs.left_ : lhs.right_ == rhs.right_);
  }

  friend constexpr std::partial_ordering operator<=>(const Data &lhs, const Data &rhs) noexcept
  {
    if (lhs.is_left_ == rhs.is_left_) {
      return lhs.is_left_ ? lhs.left_ <=> rhs.left_ : lhs.right_ <=> rhs.right_;
    } else {
      return std::partial_ordering::unordered;
    }
  }

  template <EqualityComparableSame<Data> Val>
  friend constexpr bool operator==(const Data &lhs, const Val &val) noexcept
  {
    return lhs.is_left_ ? lhs.left_ == val : lhs.right_ == val;
  }

  template <EqualityComparableLeft<Data> Val>
  friend constexpr bool operator==(const Data &lhs, const Val &val) noexcept
  {
    return lhs.is_left_ && lhs.left_ == val;
  }

  template <EqualityComparableRight<Data> Val>
  friend constexpr bool operator==(const Data &lhs, const Val &val) noexcept
  {
    return !lhs.is_left_ && lhs.right_ == val;
  }

  template <ThreeWayComparableSame<Data> Val>
  friend constexpr std::partial_ordering operator<=>(const Data &lhs, const Val &val) noexcept
  {
    return lhs.is_left_ ? lhs.left_ <=> val : lhs.right_ <=> val;
  }

  template <ThreeWayComparableLeft<Data> Val>
  friend constexpr std::partial_ordering operator<=>(const Data &lhs, const Val &val) noexcept
  {
    if (lhs.is_left_) {
      return lhs.left_ <=> val;
    } else {
      return std::partial_ordering::unordered;
    }
  }

  template <ThreeWayComparableRight<Data> Val>
  friend constexpr std::partial_ordering operator<=>(const Data &lhs, const Val &val) noexcept
  {
    if (!lhs.is_left_) {
      return lhs.right_ <=> val;
    } else {
      return std::partial_ordering::unordered;
    }
  }

  constexpr void swap(Data &rhs) noexcept(std::is_nothrow_swappable_v<Left> && std::is_nothrow_swappable_v<Right>)
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

  friend constexpr void swap(Data &lhs, Data &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  template <class... Args>
    requires(std::constructible_from<Left, Args...>)
  constexpr Left &emplaceLeft(Args &&...args) noexcept(std::is_nothrow_constructible_v<Left, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), std::forward<Args>(args)...);
    is_left_ = true;

    return left_;
  }

  template <class U, class... Args>
    requires(std::constructible_from<Left, std::initializer_list<U> &, Args...>)
  constexpr Left &
  emplaceLeft(std::initializer_list<U> ilist,
              Args &&...args) noexcept(std::is_nothrow_constructible_v<Left, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(left_), ilist, std::forward<Args>(args)...);
    is_left_ = true;

    return left_;
  }

  template <class... Args>
    requires(std::constructible_from<Right, Args...>)
  constexpr Right &emplaceRight(Args &&...args) noexcept(std::is_nothrow_constructible_v<Right, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }

  template <class U, class... Args>
    requires(std::constructible_from<Right, std::initializer_list<U> &, Args...>)
  constexpr Right &
  emplaceRight(std::initializer_list<U> ilist,
               Args &&...args) noexcept(std::is_nothrow_constructible_v<Right, std::initializer_list<U> &, Args...>)
  {
    destroy();
    std::construct_at(std::addressof(right_), ilist, std::forward<Args>(args)...);
    is_left_ = false;

    return right_;
  }
};
} // namespace detail::either_

using in_place_left_t = detail::either_::InPlaceLeftTag;
using in_place_right_t = detail::either_::InPlaceRightTag;

inline constexpr in_place_left_t in_place_left = {};
inline constexpr in_place_right_t in_place_right = {};

using create_left_t = detail::either_::CreateLeftTag;
using create_right_t = detail::either_::CreateRightTag;

inline constexpr create_left_t create_left = {};
inline constexpr create_right_t create_right = {};

using void_t = detail::either_::VoidTag;
inline constexpr detail::either_::VoidTag void_val = {};

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
class either
{
public:
  using left_t = detail::either_::bare_t<Left>;
  using right_t = detail::either_::bare_t<Right>;

  //! The object must be created of either Left or Right. The uninitialized state is not permitted.
  either() = delete;

  //! Create from another `either` by copy.
  constexpr either(const either &src) noexcept(std::is_nothrow_copy_constructible_v<left_t> &&
                                               std::is_nothrow_copy_constructible_v<right_t>)
      : data_(src.data_)
  {
  }

  //! Create from another `either` by move.
  constexpr either(either &&src) noexcept(std::is_nothrow_move_constructible_v<left_t> &&
                                          std::is_nothrow_move_constructible_v<right_t>)
      : data_(std::move(src.data_))
  {
  }

  //! Assign from another `either` by copy.
  constexpr either &operator=(const either &src) noexcept(std::is_nothrow_copy_assignable_v<left_t> &&
                                                          std::is_nothrow_copy_assignable_v<right_t>)
  {
    data_ = src.data_;
    return *this;
  }

  //! Assign from another `either` by move.
  constexpr either &operator=(either &&src) noexcept(std::is_nothrow_move_assignable_v<left_t> &&
                                                     std::is_nothrow_move_assignable_v<right_t>)
  {
    data_ = std::move(src.data_);
    return *this;
  }

  //! Create from the left alternative.
  template <detail::either_::ValidConstructableFrom<left_t> U>
  constexpr either(create_left_t t, U &&u) noexcept(std::is_nothrow_constructible_v<left_t, U>)
      : data_(t, std::forward<U>(u))
  {
  }

  //! Create from the right alternative.
  template <detail::either_::ValidConstructableFrom<right_t> U>
  constexpr either(create_right_t t, U &&u) noexcept(std::is_nothrow_constructible_v<right_t, U>)
      : data_(t, std::forward<U>(u))
  {
  }

  //! Create from the left alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<left_t, Args...>)
  constexpr either(in_place_left_t t, Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, Args...>)
      : data_(t, std::forward<Args>(args)...)
  {
  }

  //! Create from the left alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<left_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_left_t t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<left_t, std::initializer_list<U> &, Args...>)
      : data_(t, ilist, std::forward<Args>(args)...)
  {
  }

  //! Create from the right alternative in-place (with arguments).
  template <class... Args>
    requires(std::constructible_from<right_t, Args...>)
  constexpr either(in_place_right_t t, Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, Args...>)
      : data_(t, std::forward<Args>(args)...)
  {
  }

  //! Create from the right alternative in-place (with an initializer list and arguments).
  template <class U, class... Args>
    requires(std::constructible_from<right_t, std::initializer_list<U> &, Args...>)
  constexpr either(in_place_right_t t, std::initializer_list<U> ilist, Args &&...args) noexcept(
      std::is_nothrow_constructible_v<right_t, std::initializer_list<U> &, Args...>)
      : data_(t, ilist, std::forward<Args>(args)...)
  {
  }

  //! Default destructor.
  constexpr ~either() = default;

  //! Check for the left alternative.
  [[nodiscard]] constexpr bool is_left() const noexcept { return data_.is_left_; }

  //! Check for the right alternative.
  [[nodiscard]] constexpr bool is_right() const noexcept { return !is_left(); }

  //! Get the left alternative. Pre-condition: must contain the left alternative.
  template <class Self>
  [[nodiscard]] constexpr auto &&left(this Self &&self)
  {
    expects(self.is_left());
    return std::forward_like<Self>(self.data_.left_);
  }

  //! Get the right alternative. Pre-condition: must contain the right alternative.
  template <class Self>
  [[nodiscard]] constexpr auto &&right(this Self &&self)
  {
    expects(self.is_right());
    return std::forward_like<Self>(self.data_.right_);
  }

  //! Compare with another comparable object (not necessary of the `either` type)
  template <class Other>
  friend constexpr bool operator==(const either &lhs, const Other &other) noexcept
  {
    return lhs.data_ == other;
  }

  //! Three-way compare with another comparable object (not necessary of the `either` type)
  template <class Other>
  friend constexpr auto operator<=>(const either &lhs, const Other &other) noexcept
  {
    return lhs.data_ <=> other;
  }

  //! Swap value with \p rhs.
  constexpr void swap(either &rhs) noexcept(std::is_nothrow_swappable_v<left_t> && std::is_nothrow_swappable_v<right_t>)
  {
    data_.swap(rhs.data_);
  }

  //! Swap \p lhs and \p rhs.
  friend constexpr void swap(either &lhs, either &rhs) noexcept(noexcept(lhs.swap(rhs))) { lhs.swap(rhs); }

  //! Emplace left (from args)
  template <class... Args>
    requires(std::constructible_from<left_t, Args...>)
  constexpr left_t &emplaceLeft(Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, Args...>)
  {
    return data_.emplaceLeft(std::forward<Args>(args)...);
  }

  //! Emplace left (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<left_t, std::initializer_list<U> &, Args...>)
  constexpr left_t &
  emplaceLeft(std::initializer_list<U> ilist,
              Args &&...args) noexcept(std::is_nothrow_constructible_v<left_t, std::initializer_list<U> &, Args...>)
  {
    return data_.emplaceLeft(ilist, std::forward<Args>(args)...);
  }

  //! Emplace right (from args)
  template <class... Args>
    requires(std::constructible_from<right_t, Args...>)
  constexpr right_t &emplaceRight(Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, Args...>)
  {
    return data_.emplaceRight(std::forward<Args>(args)...);
  }

  //! Emplace right (from an initializer list and args)
  template <class U, class... Args>
    requires(std::constructible_from<right_t, std::initializer_list<U> &, Args...>)
  constexpr right_t &
  emplaceRight(std::initializer_list<U> ilist,
               Args &&...args) noexcept(std::is_nothrow_constructible_v<right_t, std::initializer_list<U> &, Args...>)
  {
    return data_.emplaceRight(ilist, std::forward<Args>(args)...);
  }

private:
  detail::either_::Data<left_t, right_t> data_;
};

} // namespace frt

//! Hasher for the void alternative. The hash is always 0.
template <>
struct std::hash<frt::void_t>
{
  constexpr std::size_t operator()(const frt::void_t &) const noexcept { return 0; }
};

//! Hasher for the `frt::either<L, R>` alternative. The result is either hash of the left or of the right alternative.
template <frt::detail::either_::ValidType Left, frt::detail::either_::ValidType Right>
  requires(frt::detail::either_::IsHashable<Left> && frt::detail::either_::IsHashable<Right>)
struct std::hash<frt::either<Left, Right>>
{
  constexpr std::size_t operator()(const frt::either<Left, Right> &e) const noexcept
  {
    if (e.is_left()) {
      if constexpr (std::same_as<frt::detail::either_::bare_t<Left>, frt::void_t>) {
        return std::hash<frt::void_t>{}(frt::void_val);
      } else {
        return std::hash<frt::detail::either_::bare_t<Left>>{}(e.left());
      }
    } else {
      if constexpr (std::same_as<frt::detail::either_::bare_t<Right>, frt::void_t>) {
        return std::hash<frt::void_t>{}(frt::void_val);
      } else {
        return std::hash<frt::detail::either_::bare_t<Right>>{}(e.right());
      }
    }
  }
};
