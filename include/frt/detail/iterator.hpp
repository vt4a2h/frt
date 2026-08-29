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

#include <iterator>

#include <frt/expects.hpp>

namespace frt::detail::optional_
{

template <class T>
/*!
 * \brief A contiguous iterator for frt::optional.
 *
 * As it's said in P3168R2, an optional object can be treated as a range of
 * either zero elements (for an empty optional) or a single element (for a non-empty optional).
 *
 * For more information, see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3168r2.html.
 */
class contiguous_iterator
{
public:
  using iterator_concept = std::contiguous_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

  constexpr contiguous_iterator() noexcept = default;

  constexpr explicit contiguous_iterator(pointer value_ptr) noexcept : value_ptr_(value_ptr) {}

  constexpr reference operator*() const noexcept
  {
    FRT_INTERNAL_ASSERT(value_ptr_);
    return *value_ptr_;
  }

  constexpr pointer operator->() const noexcept
  {
    FRT_INTERNAL_ASSERT(value_ptr_);
    return value_ptr_;
  }

  constexpr contiguous_iterator &operator++() noexcept
  {
    ++value_ptr_;
    return *this;
  }

  constexpr contiguous_iterator operator++(int) noexcept
  {
    contiguous_iterator tmp = *this;
    ++*this;
    return tmp;
  }

  constexpr contiguous_iterator &operator--() noexcept
  {
    --value_ptr_;
    return *this;
  }

  constexpr contiguous_iterator operator--(int) noexcept
  {
    contiguous_iterator tmp = *this;
    --*this;
    return tmp;
  }

  constexpr contiguous_iterator &operator+=(difference_type n) noexcept
  {
    value_ptr_ += n;
    return *this;
  }

  constexpr contiguous_iterator &operator-=(difference_type n) noexcept
  {
    value_ptr_ -= n;
    return *this;
  }

  constexpr reference operator[](difference_type n) const noexcept { return *(*this + n); }

  friend constexpr contiguous_iterator operator+(contiguous_iterator it, difference_type n) noexcept { return it += n; }

  friend constexpr contiguous_iterator operator+(difference_type n, contiguous_iterator it) noexcept { return it += n; }

  friend constexpr contiguous_iterator operator-(contiguous_iterator it, difference_type n) noexcept { return it -= n; }

  friend constexpr difference_type operator-(const contiguous_iterator &a, const contiguous_iterator &b) noexcept
  {
    return a.value_ptr_ - b.value_ptr_;
  }

  friend constexpr auto operator<=>(const contiguous_iterator &, const contiguous_iterator &) noexcept = default;
  friend constexpr bool operator==(const contiguous_iterator &, const contiguous_iterator &) noexcept = default;

private:
  pointer value_ptr_{nullptr};
};

} // namespace frt::detail::optional_
