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

namespace frt
{

namespace detail::meta_container_
{
template <class C, class U>
concept HasPushBack = requires(C c, U u) { c.push_back(u); };

template <class C, class U>
concept HasEmplaceBack = requires(C c, U u) { c.emplace_back(u); };

template <class C, class U>
concept HasInsert = requires(C c, U u) { c.insert(u); };

template <class C, class U>
concept HasEmplace = requires(C c, U u) { c.emplace(u); };

template <class C>
concept HasReserve = requires(C c, std::size_t n) { c.reserve(n); };

template <class C, class E>
  requires(HasPushBack<C, E> && !HasEmplaceBack<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.push_back(std::forward<E>(e));
}

// Prefer emplace_back over push_back for the sequential containers
template <class C, class E>
  requires(HasEmplaceBack<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.emplace_back(std::forward<E>(e));
}

// Prefer emplace over insert for the associative containers
template <class C, class E>
  requires(HasInsert<C, E> && !HasEmplace<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.insert(std::forward<E>(e));
}

template <class C, class E>
  requires(HasEmplace<C, E>)
void add_elem_impl(C &c, E &&e)
{
  c.emplace(std::forward<E>(e));
}

template <class C, class E>
void add_elem_impl(C &, E &&)
{
  static_assert(
      false, "The result container must support push_back, or emplace_back, or insert interface for adding elements. "
             "To support alternative interface, provide a frt::meta_container specialization for your type, please.");
}

// TODO: add a customization point to provide a custom function for adding an element to a container

template <HasReserve C>
void reserve_impl(C &c, std::size_t n)
{
  c.reserve(n);
}

template <class C>
void reserve_impl(C &, std::size_t)
{
  // Do nothing if we cannot reserve
}

// TODO: add a customization point to provide a custom function for reserving space in a container
} // namespace detail::meta_container_

template <class C>
struct meta_container
{
  using E = std::ranges::range_value_t<C>;

  static constexpr auto add_element = []<std::convertible_to<E> ElemT>(C &c, ElemT &&e) {
    detail::meta_container_::add_elem_impl(c, std::forward<ElemT>(e));
  };

  static constexpr auto reserve = [](C &c, std::size_t n) { detail::meta_container_::reserve_impl(c, n); };
};

namespace detail::meta_container_
{

template <class C>
concept MetaContainerHasAddElement =
    requires(meta_container<C> mc, C c, std::ranges::range_value_t<C> e) { mc.add_element(c, e); };

template <class C>
concept MetaContainerHasReserve = requires(meta_container<C> mc, C c, std::size_t n) { mc.reserve(c, n); };

template <template <class...> class, class>
struct select_container
{
  static_assert(false, "Cannot select a container for the given type.");
};

// A sequential container of any elements (possibly pairs)
template <template <class...> class Container, class ElemT>
  requires(requires { typename Container<ElemT>; })
struct select_container<Container, ElemT>
{
  using type = Container<ElemT>;
};

// An associative container of any keys and values. The element type must be a pair-like (not a tuple-like).
template <template <class...> class Container, class ElemT>
  requires(requires {
    typename ElemT::first_type;
    typename ElemT::second_type;
    typename Container<typename ElemT::first_type, typename ElemT::second_type>;
  })
struct select_container<Container, ElemT>
{
  using type = Container<typename ElemT::first_type, typename ElemT::second_type>;
};

template <template <class...> class Container, class ElemT>
using select_container_t = select_container<Container, ElemT>::type;

} // namespace detail::meta_container_

} // namespace frt
