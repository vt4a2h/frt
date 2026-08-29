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

} // namespace detail::optional_

using in_place_t = detail::optional_::InPlaceTag;
using nullopt_t = detail::optional_::NullOptTag;

inline constexpr in_place_t in_place_opt{in_place_t::Secret{}};
inline constexpr nullopt_t nullopt{nullopt_t::Secret{}};

namespace detail::optional_
{

template <class T>
concept ValidType = requires {
  requires !std::is_reference_v<T>;
  requires !std::same_as<T, void>;
  requires !is_tag_<T>;
  requires !std::is_array_v<bare_t<T>>;
  requires std::destructible<bare_t<T>>;
};

} // namespace detail::optional_

/*!
 * \brief The class template frt::optional manages an optional contained value,
 *        i.e., a value that may or may not be present.
 *
 * This is a fully constexpr implementation with all features up to and including C++26.
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

  constexpr optional() noexcept : data_(in_place_left) {}

  constexpr explicit(false) optional(nullopt_t) noexcept : data_(in_place_left) {}

  constexpr optional(const optional &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
    requires(std::is_copy_constructible_v<T>)
      : data_(other.data_)
  {
  }

  constexpr optional(optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
    requires(std::is_move_constructible_v<T>)
      : data_(std::move(other.data_))
  {
  }

  constexpr optional &operator=(const optional &other) noexcept(std::is_nothrow_copy_assignable_v<T>)
    requires(std::is_copy_assignable_v<T>)
  {
    data_ = other.data_;
    return *this;
  }

  constexpr optional &operator=(optional &&other) noexcept(std::is_nothrow_move_assignable_v<T>)
    requires(std::is_move_assignable_v<T>)
  {
    data_ = std::move(other.data_);
    return *this;
  }

private:
  either<void, T> data_;
};

} // namespace frt
