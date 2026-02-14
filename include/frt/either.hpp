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

template <IsNotVoid Left, IsNotVoid Right>
struct Data
{
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
  constexpr Data(CreateLeftTag, U &&u) noexcept(std::is_nothrow_constructible_v<Left, U &&>)
  {
    std::construct_at(std::addressof(left_), std::forward<U>(u));
    is_left_ = true;
  }

  template <ValidConstructableFrom<Right> U>
  constexpr Data(CreateRightTag, U &&u) noexcept(std::is_nothrow_constructible_v<Right, U &&>)
  {
    std::construct_at(std::addressof(right_), std::forward<U>(u));
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
    requires(std::equality_comparable<Left> && std::equality_comparable<Right>)
  {
    return lhs.is_left_ == rhs.is_left_ && (lhs.is_left_ ? lhs.left_ == rhs.left_ : lhs.right_ == rhs.right_);
  }

  friend constexpr std::partial_ordering operator<=>(const Data &lhs, const Data &rhs) noexcept
    requires std::three_way_comparable<Left> && std::three_way_comparable<Right>
  {
    if (lhs.is_left_ == rhs.is_left_) {
      return lhs.is_left_ ? lhs.left_ <=> rhs.left_ : lhs.right_ <=> rhs.right_;
    } else {
      return std::partial_ordering::unordered;
    }
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

template <detail::either_::ValidType Left, detail::either_::ValidType Right>
class either
{
public:
  using left_t = detail::either_::bare_t<Left>;
  using right_t = detail::either_::bare_t<Right>;

  either() = delete;

  constexpr either(const either &src) noexcept(std::is_nothrow_copy_constructible_v<left_t> &&
                                               std::is_nothrow_copy_constructible_v<right_t>)
      : data_(src.data_)
  {
  }

  constexpr either(either &&src) noexcept(std::is_nothrow_move_constructible_v<left_t> &&
                                          std::is_nothrow_move_constructible_v<right_t>)
      : data_(std::move(src.data_))
  {
  }

  constexpr either &operator=(const either &src) noexcept(std::is_nothrow_copy_assignable_v<left_t> &&
                                                          std::is_nothrow_copy_assignable_v<right_t>)
  {
    data_ = src.data_;
    return *this;
  }

  constexpr either &operator=(either &&src) noexcept(std::is_nothrow_move_assignable_v<left_t> &&
                                                     std::is_nothrow_move_assignable_v<right_t>)
  {
    data_ = std::move(src.data_);
    return *this;
  }

  template <detail::either_::ValidConstructableFrom<left_t> U>
  constexpr either(create_left_t t, U &&u) noexcept(std::is_nothrow_constructible_v<left_t, U>)
      : data_(t, std::forward<U>(u))
  {
  }

  template <detail::either_::ValidConstructableFrom<right_t> U>
  constexpr either(create_right_t t, U &&u) noexcept(std::is_nothrow_constructible_v<right_t, U>)
      : data_(t, std::forward<U>(u))
  {
  }

  constexpr ~either() = default;

  [[nodiscard]] constexpr bool is_left() const noexcept { return data_.is_left_; }
  [[nodiscard]] constexpr bool is_right() const noexcept { return !is_left(); }

  template <class Self>
  [[nodiscard]] constexpr auto &&left(this Self &&self)
  {
    expects(self.is_left());
    return std::forward_like<Self>(self.data_.left_);
  }

  template <class Self>
  [[nodiscard]] constexpr auto &&right(this Self &&self)
  {
    expects(self.is_right());
    return std::forward_like<Self>(self.data_.right_);
  }

  friend constexpr bool operator==(const either &lhs, const either &rhs) noexcept = default;
  friend constexpr auto operator<=>(const either &, const either &) noexcept = default;

private:
  detail::either_::Data<left_t, right_t> data_;
};

} // namespace frt
