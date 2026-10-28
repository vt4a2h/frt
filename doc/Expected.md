# Expected

The class template `frt::expected<T, E>` provides a way to represent either of two values: an expected value of type `T`
(referred to as the "value"), or an unexpected value of type `E` (referred to as the "error"). An `frt::expected` object
is never valueless.

While [Either](Either.md) is a symmetric sum type where neither alternative carries a preconceived meaning, `Expected`
is an asymmetric result type designed specifically for computations that can succeed or fail. The value alternative `T`
represents a successful result, while the error alternative `E` represents a failure reason.

```c++
struct Component;
struct Error;

using ComponentResult = frt::expected<Component, Error>;

// Operation can fail. The result is either a component or an error
ComponentResult loadComponent( /* ... */ );

// ...

auto result = loadComponent( /* ... */ );
if (result.has_value()) {
    auto& component = result.value();
    // Do something with the component
} else {
    auto& error = result.error();
    // Handle the error
}
```

## Table of contents

- [Monadic operations](#monadic-operations)
- [Match](#match)
- [Key differences from `std::expected`](#key-differences-from-stdexpected)
- [API reference](#api-reference)
    - [Template parameters](#template-parameters)
    - [Member types](#member-types)
    - [Constructors and assignment](#constructors-and-assignment)
    - [Observers](#observers)
    - [Modifiers](#modifiers)
    - [Monadic operations](#monadic-operations-1)
    - [Pattern matching](#pattern-matching)
    - [Comparisons](#comparisons)
    - [Non-member utilities, tags, and helpers](#non-member-utilities-tags-and-helpers)
        - [`frt::unexpected`](#frtunexpected)
        - [Tags](#tags)
        - [Concepts](#concepts)
        - [Standard library integration](#standard-library-integration)

---

## Monadic operations

The type comes with monadic operations named to conform to both standard C++ conventions and functional programming
practices. Assume you have `frt::expected<T, E>`, where `T` is the type of the expected value and `E` is the type of
the unexpected error. The types `U` and `G` indicate that operations can change the type of the value or error
respectively.

| Operation                     | `frt` name        | Also known as        |
|-------------------------------|-------------------|----------------------|
| `f: T -> frt::expected<U, E>` | `and_then`        | bind / flatMap       |
| `f: E -> frt::expected<T, G>` | `or_else`         | recoverWith / orElse |
| `f: T -> U`                   | `transform`       | map / fmap           |
| `f: E -> G`                   | `transform_error` | leftMap / mapError   |

```c++
struct Context;
struct Path;
struct MetaData;
struct Component;
struct Widget;
struct Error;

frt::expected<Context, Error> extractContext();
frt::expected<Component, Error> createComponent(Context, Path, MetaData);
Widget toWidget(const Component& component);
frt::expected<Widget, Error> fallbackWidget(const Error& error);

Path path{ /* ... */ };
MetaData metaData{ /* ... */ };

auto result = extractContext()
    .and_then(&createComponent, path, metaData)
    .transform(&toWidget)
    .or_else(&fallbackWidget);
```

All monadic operations support extended syntax, allowing you to pass trailing argument packs and customize the position
of the unwrapped value or error using `frt::unwrapped`. You can find more information in
the [Monadic Operations](MonadicOperations.md) documentation.

In addition, all monadic operations can be used as free functions and support pipeline syntax (`|`). You can find more
information in the [Monadic Operations as Free Functions](MonadicOperationsAsFreeFunctions.md) documentation.

## Match

There is another operation called `match` that allows you to handle both value and error states in a single call. This
operation works similarly to pattern matching: it takes into account the value category and constness of the `expected`
object, can mutate the contained value or error in place, and supports generic visitors.

`match` provides two overloads:

1. Accepts two callables: the first for the value alternative and the second for the error alternative.
2. Accepts a single callable that can be invoked for either alternative.

```c++
auto result = loadComponent( /* ... */ );

result.match(
    [](const Component& component) {
        // Do something with the component
        // You can mutate the component in place if the parameter is `Component&`
        // You can return something from the function
    },
    [](const Error& error) {
        // Handle the error
        // You can mutate the error in place if the parameter is `Error&`
        // You can return something from the function
    }
);

result.match([](const auto& value) {
    // This function will be invoked for both component and error
    // For example, if both are formattable:
    std::println("Outcome: {}", value);
});
```

---

## Key differences from `std::expected`

While `frt::expected<T, E>` shares the same core role as `std::expected<T, E>` introduced in C++23, it incorporates
several key design improvements, extended monadic capabilities, and tighter integration with the rest of the library:

1. **Extended monadic argument forwarding & `frt::unwrapped`**:
    - In `std::expected`, member functions `and_then`, `or_else`, `transform`, and `transform_error` accept only a unary
      callable (or a nullary callable if `T` is `void`).
    - In `frt::expected`, all monadic member functions accept additional trailing arguments (`args...`) that are
      forwarded directly to the callable. Furthermore, by using the placeholder marker `frt::unwrapped`, the unwrapped
      value or error can be positioned at any arbitrary argument index instead of strictly being the first argument.

2. **Pattern matching with `match`**:
    - `std::expected` does not provide pattern matching member functions.
    - `frt::expected` provides `match(vf, ef)` and `match(f)` member functions. `match` forwards the value category and
      constness of the `expected` instance, allowing in-place mutation or moving of the active alternative.

3. **`error_or` fallback observer**:
    - `std::expected` provides `value_or(default_value)` to obtain a fallback when an error is present, but provides no
      equivalent method for errors.
    - `frt::expected` provides `error_or(default_value)` (in both `const &` and `&&` overloads), returning the held
      error
      or the supplied default if a value is present.

4. **Contract-based assertions and `noexcept` accessors**:
    - In `std::expected`, invoking `.value()` when `has_value() == false` throws `std::bad_expected_access<E>`.
    - In `frt::expected`, calling `.value()`, `operator*`, or `.error()` with unsatisfied preconditions triggers
      internal
      assertions (`FRT_INTERNAL_ASSERT`) rather than throwing exceptions, and these accessors are marked `noexcept`.

5. **Void value representation**:
    - In `std::expected<void, E>`, `.value()` and `operator*()` return `void`.
    - In `frt::expected<void, E>`, `.value()` and `operator*()` return `frt::void_t` (`frt::void_val`). This enables
      uniform generic programming where the return value of an accessor can be handled as an expression without
      requiring separate template specializations for `void`.

6. **Interoperability with `frt::either` and algorithms**:
    - `frt::expected<T, E>` is implemented directly on top of `frt::either<E, T>`.
    - It provides implicit/explicit constructors and assignment operators from `frt::either`, and provides the
      `.asEither()` observer for direct access to the underlying sum type.
    - It seamlessly participates in `frt` algorithms such as `frt::traverse`, `frt::sequence`, `frt::match`, and free
      monadic operators via `frt::meta_either`.

7. **Disallowance of identical value and error types**:
    - `std::expected<T, E>` allows `T` and `E` to be the exact same type (e.g. `std::expected<int, int>`).
    - `frt::expected<T, E>` explicitly requires `T` and `E` to be distinct types (`!std::same_as<bare_t<T>, bare_t<E>>`)
      at compile time. This prevents ambiguity and subtle bugs in value-category conversions and generic handling.

8. **Deducing `this` (C++23 explicit object parameter)**:
    - `frt::expected` uses C++23 explicit object parameters (`this Self&& self`) throughout its member functions,
      reducing overload boilerplate and ensuring exact cv- and ref-qualification forwarding.

9. **Dedicated in-place construction tags**:
    - Instead of standard library tags `std::in_place` and `std::unexpect`, `frt::expected` uses
      `frt::in_place_expected` (`frt::in_place_expected_t`) and `frt::in_place_unexpected`
      (`frt::in_place_unexpected_t`).

10. **Three-way comparison semantics**:
    - In `std::expected`, an instance holding an error is ordered before an instance holding a value.
    - In `frt::expected`, comparing instances holding different alternatives yields `std::partial_ordering::unordered`,
      consistent with `frt::either`.

---

## API reference

```cpp
#include <frt/expected.hpp>

namespace frt {

template <class E>
struct unexpected;

template <class T, class E>
struct [[nodiscard]] expected;

}
```

### Template parameters

- `T`: Type of the expected value. Must be `void` or a destructible, non-reference, non-array object type. Must not be
  a function type, `frt::unexpected<E>`, or an in-place tag type (`in_place_expected_t`, `in_place_unexpected_t`).
- `E`: Type of the unexpected value (error). Must be a destructible, non-array, non-reference object type without
  `const` or `volatile` qualifiers. Must not be a function type, `frt::unexpected<E>`, or an in-place tag type.

The types `T` and `E` (stripped of cv-qualifiers and references) must be different types.

---

### Member types

| Type                   | Description                         |
|------------------------|-------------------------------------|
| `value_type`           | Type of the expected value (`T`).   |
| `error_type`           | Type of the unexpected error (`E`). |
| `unexpected_type`      | Alias for `frt::unexpected<E>`.     |
| `rebind_value_type<U>` | Alias for `expected<U, E>`.         |
| `rebind_error_type<U>` | Alias for `expected<T, U>`.         |

---

### Constructors and assignment

```cpp
constexpr expected() noexcept(/* ... */)
  requires(IsVoid<T> || std::is_default_constructible_v<T>);
```

- **Effects**: Value-initializes the expected value. If `T` is `void`, initializes to an expected state.
- **Postconditions**: `has_value() == true`.

```cpp
constexpr expected(const expected &src);
constexpr expected(expected &&src);

template <class U, class G>
constexpr explicit(/* ... */) expected(const expected<U, G> &other);

template <class U, class G>
constexpr explicit(/* ... */) expected(expected<U, G> &&other);
```

- **Postconditions**: `has_value() == other.has_value()`. The active alternative is initialized from `other`'s active
  alternative.

```cpp
template <class L, class R>
constexpr expected(const either<L, R> &inEither) noexcept(/* ... */);

template <class L, class R>
constexpr expected(either<L, R> &&inEither) noexcept(/* ... */);
```

- **Effects**: If `inEither.is_left() == true`, initializes the expected value from `inEither.left()`; otherwise,
  initializes the unexpected error from `inEither.right()`.
- **Postconditions**: `has_value() == inEither.is_left()`.

```cpp
template <class U = std::remove_cv_t<T>>
constexpr explicit(!std::is_convertible_v<U, T>) expected(U &&v) noexcept(/* ... */);
```

- **Effects**: Constructs the expected value with `std::forward<U>(v)`.
- **Postconditions**: `has_value() == true`.

```cpp
template <class G = E>
constexpr explicit(!std::is_convertible_v<const G &, E>) expected(const unexpected<G> &e) noexcept(/* ... */);

template <class G = E>
constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G> &&e) noexcept(/* ... */);
```

- **Effects**: Constructs the unexpected error with `e.error()` (or `std::move(e).error()`).
- **Postconditions**: `has_value() == false`.

```cpp
template <class... Args>
constexpr explicit expected(in_place_expected_t, Args &&...args) noexcept(/* ... */);

template <class U, class... Args>
constexpr explicit expected(in_place_expected_t, std::initializer_list<U> ilist, Args &&...args) noexcept(/* ... */);
```

- **Effects**: In-place constructs the expected value using `args...` (or `ilist, args...`). If `T` is `void`, no
  arguments are passed.
- **Postconditions**: `has_value() == true`.

```cpp
template <class... Args>
constexpr explicit expected(in_place_unexpected_t, Args &&...args) noexcept(/* ... */);

template <class U, class... Args>
constexpr explicit expected(in_place_unexpected_t, std::initializer_list<U> ilist, Args &&...args) noexcept(/* ... */);
```

- **Effects**: In-place constructs the unexpected error using `args...` (or `ilist, args...`).
- **Postconditions**: `has_value() == false`.

```cpp
expected &operator=(const expected &src);
expected &operator=(expected &&src);

template <class L, class R>
constexpr expected &operator=(const either<L, R> &src) noexcept(/* ... */);

template <class L, class R>
constexpr expected &operator=(either<L, R> &&src) noexcept(/* ... */);
```

- **Effects**: Destroys the currently held alternative and re-initializes from `src`. For assignment from `either`,
  left initializes value and right initializes error.
- **Postconditions**: `has_value() == src.has_value()` (or `has_value() == src.is_left()`).
- **Returns**: `*this`.

---

### Observers

```cpp
[[nodiscard]] constexpr explicit operator bool() const noexcept;
[[nodiscard]] constexpr bool has_value() const noexcept;
```

- **Returns**: `true` if `*this` contains an expected value; `false` if it contains an unexpected error.

```cpp
[[nodiscard]] constexpr const T *operator->() const noexcept
  requires(detail::expected_::IsNotVoid<T>);

[[nodiscard]] constexpr T *operator->() noexcept
  requires(detail::expected_::IsNotVoid<T>);
```

- **Preconditions**: `has_value() == true`.
- **Returns**: A pointer to the contained value.

```cpp
template <class Self>
  requires(detail::expected_::IsNotVoid<T>)
[[nodiscard]] constexpr auto &&operator*(this Self &&self) noexcept;

constexpr void_t operator*() const noexcept
  requires(detail::expected_::IsVoid<T>);
```

- **Preconditions**: `self.has_value() == true`.
- **Returns**: A reference to the contained value, forwarded with the value category and constness of `self`. If `T`
  is `void`, returns `frt::void_val`.

```cpp
template <class Self>
  requires(detail::expected_::IsNotVoid<T>)
[[nodiscard]] constexpr auto &&value(this Self &&self) noexcept;

constexpr void_t value() const noexcept
  requires(detail::expected_::IsVoid<T>);
```

- **Preconditions**: `self.has_value() == true`.
- **Returns**: A reference to the contained value, forwarded with the value category and constness of `self`. If `T`
  is `void`, returns `frt::void_val`.

```cpp
template <class U = std::remove_cv_t<T>>
[[nodiscard]] constexpr T value_or(U &&default_value) const & noexcept(/* ... */);

template <class U = std::remove_cv_t<T>>
[[nodiscard]] constexpr T value_or(U &&default_value) && noexcept(/* ... */);
```

- **Returns**: The contained value if `has_value() == true`; otherwise `static_cast<T>(std::forward<U>(default_value))`.

```cpp
template <class Self>
  requires(detail::expected_::IsNotVoid<error_type>)
[[nodiscard]] constexpr auto &&error(this Self &&self) noexcept;
```

- **Preconditions**: `self.has_value() == false`.
- **Returns**: A reference to the contained unexpected error, forwarded with the value category and constness of `self`.

```cpp
template <class G = E>
[[nodiscard]] constexpr E error_or(G &&default_value) const & noexcept(/* ... */);

template <class G = E>
[[nodiscard]] constexpr E error_or(G &&default_value) && noexcept(/* ... */);
```

- **Returns**: The contained error if `has_value() == false`; otherwise `std::forward<G>(default_value)`.

```cpp
template <class Self>
[[nodiscard]] constexpr auto &&asEither(this Self &&self) noexcept;
```

- **Returns**: A reference to the underlying `frt::either<E, T>`, forwarded with the value category and constness of
  `self`.

---

### Modifiers

```cpp
template <class... Args>
constexpr decltype(auto) emplace(Args &&...args) noexcept(/* ... */);

template <class U, class... Args>
constexpr decltype(auto) emplace(std::initializer_list<U> ilist, Args &&...args) noexcept(/* ... */);

constexpr void emplace() noexcept
  requires(detail::common_::IsVoid<T>);
```

- **Effects**: Destroys the currently held alternative and constructs the expected value in-place.
- **Postconditions**: `has_value() == true`.
- **Returns**: A reference to the newly constructed value (or `void` if `T` is `void`).

```cpp
constexpr void swap(expected &other) noexcept(/* ... */);
```

- **Effects**: Exchanges the state and contained alternatives of `*this` and `other`.
- **Postconditions**: `*this` contains the alternative previously in `other`, and `other` contains the alternative
  previously in `*this`.

---

### Monadic operations

All monadic operations forward additional arguments `args...` to the callable `f`. If `args...` contains
`frt::unwrapped`, the alternative value or error replaces the `frt::unwrapped` placeholder in the invocation; otherwise,
the alternative value or error is passed as the first argument. When the alternative is `void`, `f` is invoked with
`args...` only, and `frt::unwrapped` is prohibited.

```cpp
template <class Self, class F, class... Args>
  requires(detail::expected_::IsNotVoid<value_type>)
[[nodiscard]] constexpr auto and_then(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);

template <class Self, class F, class... Args>
  requires(detail::expected_::IsVoid<value_type>)
[[nodiscard]] constexpr auto and_then(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Callable `f` must return `frt::expected<U, error_type>`. If `value_type` is not `void`, `args...`
  must contain at most one `frt::unwrapped` marker (and not as the sole argument). If `value_type` is `void`,
  `frt::unwrapped` is prohibited.
- **Effects**: If `self.has_value() == true`, invokes `f` with the forwarded value (or `args...` if `void`) and returns
  the resulting expected object. If `self.has_value() == false`, returns an `expected<U, error_type>` containing the
  forwarded error.

```cpp
template <class Self, class F, class... Args>
  requires(detail::expected_::IsNotVoid<error_type>)
[[nodiscard]] constexpr auto or_else(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Callable `f` must return `frt::expected<value_type, G>`. `args...` must contain at most one
  `frt::unwrapped` marker (and not as the sole argument).
- **Effects**: If `self.has_value() == false`, invokes `f` with the forwarded error and returns the resulting expected
  object. If `self.has_value() == true`, returns an `expected<value_type, G>` containing the forwarded value.

```cpp
template <class Self, class F, class... Args>
  requires(detail::expected_::IsNotVoid<value_type>)
[[nodiscard]] constexpr auto transform(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);

template <class Self, class F, class... Args>
  requires(detail::expected_::IsVoid<value_type>)
[[nodiscard]] constexpr auto transform(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Same argument forwarding rules as `and_then`.
- **Effects**: If `self.has_value() == true`, invokes `f` and wraps the result in an `expected<U, error_type>` as the
  expected value (or `expected<void, error_type>` if `f` returns `void`). If `self.has_value() == false`, returns an
  `expected<U, error_type>` containing the forwarded error.

```cpp
template <class Self, class F, class... Args>
  requires(detail::expected_::IsNotVoid<error_type>)
[[nodiscard]] constexpr auto transform_error(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Same argument forwarding rules as `or_else`.
- **Effects**: If `self.has_value() == false`, invokes `f` and wraps the result in an `expected<value_type, G>` as the
  unexpected error. If `self.has_value() == true`, returns an `expected<value_type, G>` containing the forwarded value.

---

### Pattern matching

```cpp
template <class Self, class ValueFunc, class ErrorFunc>
[[nodiscard]] constexpr decltype(auto) match(this Self &&self, ValueFunc vf, ErrorFunc ef) noexcept(/* ... */);

template <class Self, class ValueFunc, class ErrorFunc>
constexpr void match(this Self &&self, ValueFunc vf, ErrorFunc ef) noexcept(/* ... */);
```

- **Preconditions**: `vf` must be invocable with the value (or with no arguments if `value_type` is `void`). `ef` must
  be invocable with the error. Both callables must have identical return types (or both return `void`).
- **Effects**: If `self.has_value() == true`, invokes `vf`. Otherwise, invokes `ef`.
- **Returns**: The value returned by `vf` or `ef` (or `void`).

```cpp
template <class Self, class Func>
[[nodiscard]] constexpr decltype(auto) match(this Self &&self, Func f) noexcept(/* ... */);

template <class Self, class Func>
constexpr void match(this Self &&self, Func f) noexcept(/* ... */);
```

- **Preconditions**: Callable `f` must be invocable with both value and error alternatives (or with no arguments if
  `value_type` is `void`) and return the same type in both cases.
- **Effects**: Equivalent to calling `self.match(f, f)`.

---

### Comparisons

```cpp
template <class U, class G>
[[nodiscard]] friend constexpr bool
operator==(const expected &lhs, const expected<U, G> &rhs) noexcept(/* ... */);
```

- **Returns**: `true` if `lhs` and `rhs` both contain values and `lhs.value() == rhs.value()`, or both contain errors
  and `lhs.error() == rhs.error()`. Returns `false` otherwise.

```cpp
template <class U, class G>
[[nodiscard]] friend constexpr std::partial_ordering
operator<=>(const expected &lhs, const expected<U, G> &rhs) noexcept(/* ... */);
```

- **Returns**: `lhs.value() <=> rhs.value()` if both hold values; `lhs.error() <=> rhs.error()` if both hold errors;
  `std::partial_ordering::unordered` if one holds a value and the other holds an error.

---

### Non-member utilities, tags, and helpers

#### `frt::unexpected`

Represents an unexpected value stored in `frt::expected`.

```cpp
template <class E>
struct unexpected {
    constexpr explicit unexpected(Err &&e);
    template <class... Args>
    constexpr explicit unexpected(in_place_unexpected_t, Args &&...args);
    template <class U, class... Args>
    constexpr explicit unexpected(in_place_unexpected_t, std::initializer_list<U> il, Args &&...args);

    template <class Self>
    [[nodiscard]] constexpr auto &&error(this Self &&self);

    constexpr void swap(unexpected &other) noexcept(/* ... */);
};

template <class E>
unexpected(E) -> unexpected<E>;
```

#### Tags

```cpp
struct in_place_expected_t { /* ... */ };
inline constexpr in_place_expected_t in_place_expected{ /* ... */ };

struct in_place_unexpected_t { /* ... */ };
inline constexpr in_place_unexpected_t in_place_unexpected{ /* ... */ };

struct unwrapped_t { /* ... */ };
inline constexpr unwrapped_t unwrapped{ /* ... */ };
```

- Tags for in-place construction of expected value, unexpected error, and the placeholder for monadic argument packs.

#### Concepts

```cpp
template <class T>
concept IsExpected;

template <class T>
concept IsUnexpected;
```

- `IsExpected`: satisfied if `std::remove_cvref_t<T>` is a specialization of `frt::expected`.
- `IsUnexpected`: satisfied if `std::remove_cvref_t<T>` is a specialization of `frt::unexpected`.

#### Standard library integration

- `<frt/hash/std_hash_expected.hpp>` provides a specialization of `std::hash<frt::expected<T, E>>`.
- `<frt/format/std_fmt_expected.hpp>` provides a specialization of `std::formatter<frt::expected<T, E>, char>`.
