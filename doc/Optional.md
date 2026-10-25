# Optional

The class template `frt::optional<T>` manages an optional contained value, representing either the presence of a value
of type `T` or its absence. An `frt::optional` object is never valueless and does not allocate dynamic memory for its
contained state.

While [Either](Either.md) is a sum type representing two alternative states and [Expected](Expected.md) represents
either a successful value or a distinct error payload, `Optional` specializes in cases where an operation simply either
yields a result or produces nothing (`nullopt`).

```c++
struct Component;

using OptionalComponent = frt::optional<Component>;

// Operation can yield a component or nothing
OptionalComponent findComponent( /* ... */ );

// ...

auto opt = findComponent( /* ... */ );
if (opt.has_value()) {
    auto& component = opt.value();
    // Do something with the component
} else {
    // Handle absence of component
}
```

## Monadic operations

The type comes with monadic operations conforming to modern C++ standards and functional programming idioms. Assume you
have `frt::optional<T>`, where `T` is the type of the contained value. The type `U` indicates that an operation can
change the type of the value.

| Operation                   | `frt` name  | Also known as        |
|-----------------------------|-------------|----------------------|
| `f: T -> frt::optional<U>`  | `and_then`  | bind / flatMap       |
| `f: () -> frt::optional<T>` | `or_else`   | orElse / recoverWith |
| `f: T -> U`                 | `transform` | map / fmap           |

```c++
struct Context;
struct Path;
struct MetaData;
struct Component;
struct Widget;

frt::optional<Context> extractContext();
frt::optional<Component> createComponent(Context, Path, MetaData);
Widget toWidget(const Component& component);
frt::optional<Widget> fallbackWidget();

Path path{ /* ... */ };
MetaData metaData{ /* ... */ };

auto result = extractContext()
    .and_then(&createComponent, path, metaData)
    .transform(&toWidget)
    .or_else(&fallbackWidget);
```

All monadic operations support extended syntax, allowing you to pass trailing argument packs and customize the position
of the unwrapped value using `frt::unwrapped`. You can find more information in
the [Monadic Operations](MonadicOperations.md) documentation.

In addition, all monadic operations can be used as free functions and support pipeline syntax (`|`). You can find more
information in the [Monadic Operations as Free Functions](MonadicOperationsAsFreeFunctions.md) documentation.

## Range support

`frt::optional<T>` natively models a `std::ranges::contiguous_range` containing either zero elements (when empty) or
one element (when containing a value). This adheres to the C++ standard proposal P3168R2, allowing an optional object
to be used seamlessly with range-based `for` loops, standard range algorithms, and range adaptors:

```c++
auto opt = findComponent( /* ... */ );

// Range-based for loop executes 0 or 1 time
for (const auto& component : opt) {
    render(component);
}

// Seamless composition with std::views
auto names = opt
    | std::views::transform(&Component::name);
```

## Match

There is another operation called `match` that allows you to handle both present and empty states in a single call. This
operation works similarly to pattern matching: it takes into account the value category and constness of the `optional`
object, can mutate the contained value in place, and preserves value categories.

`match` provides two overloads:

1. Accepts two callables: the first invoked with the value if present, and the second invoked without arguments if
   empty.
2. Accepts a single callable that can be invoked with the value or without arguments.

```c++
auto opt = findComponent( /* ... */ );

opt.match(
    [](const Component& component) {
        // Do something with the component
        // You can mutate the component in place if the parameter is `Component&`
        // You can return something from the function
    },
    []() {
        // Handle absence of value
        // You can return something from the function
    }
);

opt.match([](const auto&... args) {
    // This callable is invoked with `component` when present, or with 0 arguments when empty
});
```

---

## Key differences from `std::optional`

While `frt::optional<T>` serves a similar role to `std::optional<T>`, it incorporates several key design enhancements,
extended monadic ergonomics, and full integration with the FRT library ecosystem:

1. **Extended monadic argument forwarding & `frt::unwrapped`**:
    - In `std::optional`, member functions `and_then`, `or_else`, and `transform` accept only unary callables `f` (or
      nullary for `or_else`).
    - In `frt::optional`, all monadic member functions accept additional trailing arguments (`args...`) that are
      forwarded directly to `f`. Furthermore, `and_then` and `transform` support `frt::unwrapped` to place the unwrapped
      value at any position within the argument list.

2. **Native range and iterator support (P3168R2)**:
    - In C++17–C++23, `std::optional` is not a range and provides no `begin()` or `end()` members.
    - `frt::optional` natively models a `std::ranges::contiguous_range`, providing `begin()` and `end()` iterators (both
      non-const and const). This allows direct use with range-based `for` loops, `std::ranges` algorithms, and
      `std::views`.

3. **Pattern matching with `match`**:
    - `std::optional` has no pattern matching member function.
    - `frt::optional` provides `match(vf, nf)` and `match(f)`, supporting value-category preservation, in-place
      mutation,
      and uniform return types across both branches.

4. **Contract-based assertions and `noexcept` accessors**:
    - In `std::optional`, calling `.value()` on an empty instance throws `std::bad_optional_access`.
    - In `frt::optional`, `.value()` and `operator*()` are marked `noexcept` and enforce the precondition
      (`has_value()`)
      via internal assertions (`FRT_INTERNAL_ASSERT`), avoiding exception overhead in performance-sensitive code.

5. **Interoperability with `frt::either` and algorithms**:
    - `frt::optional<T>` is implemented on top of `frt::either<void, T>`.
    - It provides direct converting constructors and assignment operators to and from `frt::either<void, U>`, as well as
      the `.asEither()` accessor.
    - It seamlessly participates in generic FRT algorithms (`frt::traverse`, `frt::sequence`, `frt::match`) and free
      monadic pipelines via `frt::meta_either`.

6. **Deducing `this` (C++23 explicit object parameter)**:
    - `frt::optional` uses C++23 explicit object parameters (`this Self&& self`) across observers and monadic
      operations,
      ensuring exact cv- and ref-qualification forwarding without redundant overloads.

7. **Dedicated in-place construction tag**:
    - Uses `frt::in_place_opt` (`frt::in_place_opt_t`) instead of `std::in_place`.
    - Disambiguation tag `frt::nullopt` (`frt::nullopt_t`) is provided under namespace `frt`.

8. **Free function monadic algorithms and pipe syntax**:
    - All monadic operations can be invoked as free functions supporting partial application and the pipe operator
      (`opt | frt::and_then(...) | frt::transform(...)`).

---

## API reference

```cpp
#include <frt/optional.hpp>

namespace frt {

template <class T>
struct [[nodiscard]] optional;

}
```

### Template parameters

- `T`: Type of the contained value. Must be a destructible, non-reference, non-array object type. Must not be `void` or
  a tag type (`in_place_opt_t`, `nullopt_t`).

---

### Member types

| Type             | Description                                                                                         |
|------------------|-----------------------------------------------------------------------------------------------------|
| `value_type`     | Type of the contained value (`T`).                                                                  |
| `iterator`       | Contiguous iterator type pointing to `T` (`detail::optional_::contiguous_iterator<T>`).             |
| `const_iterator` | Contiguous iterator type pointing to `const T` (`detail::optional_::contiguous_iterator<const T>`). |

---

### Constructors and assignment

```cpp
constexpr optional() noexcept = default;
```

- **Effects**: Constructs an empty `optional`.
- **Postconditions**: `has_value() == false`.

```cpp
constexpr explicit(false) optional(nullopt_t) noexcept;
```

- **Effects**: Constructs an empty `optional`.
- **Postconditions**: `has_value() == false`.

```cpp
constexpr optional(const optional &other) noexcept(/* ... */);
constexpr optional(optional &&other) noexcept(/* ... */);

template <class U>
constexpr explicit(/* ... */) optional(const optional<U> &other) noexcept(/* ... */);

template <class U>
constexpr explicit(/* ... */) optional(optional<U> &&other) noexcept(/* ... */);
```

- **Postconditions**: `has_value() == other.has_value()`. If `other.has_value() == true`, the contained value is
  initialized from `other.value()`.

```cpp
template <class U>
constexpr explicit optional(const either<void, U> &inEither) noexcept(/* ... */);

template <class U>
constexpr explicit optional(either<void, U> &&inEither) noexcept(/* ... */);
```

- **Effects**: If `inEither.is_right() == true`, constructs the contained value from `inEither.right()`; otherwise,
  constructs an empty optional.
- **Postconditions**: `has_value() == inEither.is_right()`.

```cpp
template <class U = std::remove_cvref_t<T>>
constexpr explicit(!std::constructible_from<T, U>) optional(U &&value) noexcept(/* ... */);
```

- **Effects**: Constructs the contained value with `std::forward<U>(value)`.
- **Postconditions**: `has_value() == true`.

```cpp
template <class... Args>
constexpr explicit optional(in_place_opt_t, Args &&...args) noexcept(/* ... */);

template <class U, class... Args>
constexpr explicit optional(in_place_opt_t, std::initializer_list<U> il, Args &&...args) noexcept(/* ... */);
```

- **Effects**: In-place constructs the contained value using `args...` (or `il, args...`).
- **Postconditions**: `has_value() == true`.

```cpp
constexpr optional &operator=(const optional &other) noexcept(/* ... */);
constexpr optional &operator=(optional &&other) noexcept(/* ... */);

template <class U>
constexpr optional &operator=(const optional<U> &other) noexcept(/* ... */);

template <class U>
constexpr optional &operator=(optional<U> &&other) noexcept(/* ... */);
```

- **Effects**: Assigns the state of `other` to `*this`.
- **Postconditions**: `has_value() == other.has_value()`.
- **Returns**: `*this`.

```cpp
constexpr optional &operator=(nullopt_t) noexcept;
```

- **Effects**: Resets `*this` to the empty state.
- **Postconditions**: `has_value() == false`.
- **Returns**: `*this`.

```cpp
template <class U = std::remove_cvref_t<T>>
constexpr optional &operator=(U &&value) noexcept(/* ... */);
```

- **Effects**: Assigns or constructs `value` in-place.
- **Postconditions**: `has_value() == true`.
- **Returns**: `*this`.

```cpp
template <class U>
constexpr optional &operator=(const either<void, U> &other) noexcept(/* ... */);

template <class U>
constexpr optional &operator=(either<void, U> &&other) noexcept(/* ... */);
```

- **Effects**: If `other.is_right() == true`, assigns or constructs the value from `other.right()`; otherwise, resets
  the optional.
- **Postconditions**: `has_value() == other.is_right()`.
- **Returns**: `*this`.

---

### Observers

```cpp
constexpr explicit operator bool() const noexcept;
[[nodiscard]] constexpr bool has_value() const noexcept;
```

- **Returns**: `true` if `*this` contains a value; otherwise `false`.

```cpp
constexpr const T *operator->() const noexcept;
constexpr T *operator->() noexcept;
```

- **Preconditions**: `has_value() == true`.
- **Returns**: A pointer to the contained value.

```cpp
template <class Self>
[[nodiscard]] constexpr auto &&operator*(this Self &&self) noexcept;
```

- **Preconditions**: `self.has_value() == true`.
- **Returns**: A reference to the contained value, forwarded with the value category and constness of `self`.

```cpp
template <class Self>
[[nodiscard]] constexpr auto &&value(this Self &&self) noexcept;
```

- **Preconditions**: `self.has_value() == true`.
- **Returns**: A reference to the contained value, forwarded with the value category and constness of `self`.

```cpp
template <std::convertible_to<T> U = std::remove_cv_t<T>>
constexpr T value_or(U &&default_value) const & noexcept(/* ... */);

template <std::convertible_to<T> U = std::remove_cv_t<T>>
constexpr T value_or(U &&default_value) && noexcept(/* ... */);
```

- **Returns**: The contained value if `has_value() == true`; otherwise `static_cast<T>(std::forward<U>(default_value))`.

```cpp
template <class Self>
[[nodiscard]] constexpr auto &&asEither(this Self &&self) noexcept;
```

- **Returns**: A reference to the underlying `frt::either<void, T>`, forwarded with the value category and constness
  of `self`.

---

### Range and iterator access

```cpp
[[nodiscard]] constexpr iterator begin() noexcept;
[[nodiscard]] constexpr const_iterator begin() const noexcept;
```

- **Returns**: An iterator to the contained value if `has_value() == true`; otherwise equal to `end()`.

```cpp
[[nodiscard]] constexpr iterator end() noexcept;
[[nodiscard]] constexpr const_iterator end() const noexcept;
```

- **Returns**: An iterator to the element following the contained value if `has_value() == true`; otherwise equal to
  `begin()`.

---

### Modifiers

```cpp
constexpr void reset() noexcept;
```

- **Effects**: Destroys the contained value if present.
- **Postconditions**: `has_value() == false`.

```cpp
template <class... Args>
constexpr T &emplace(Args &&...args) noexcept(/* ... */);

template <class U, class... Args>
constexpr T &emplace(std::initializer_list<U> il, Args &&...args) noexcept(/* ... */);
```

- **Effects**: Destroys any contained value and constructs a new value of type `T` in-place using `args...` (or
  `il, args...`).
- **Postconditions**: `has_value() == true`.
- **Returns**: A reference to the newly constructed value.

```cpp
constexpr void swap(optional &other) noexcept(/* ... */);
```

- **Effects**: Exchanges the contents of `*this` and `other`.
- **Postconditions**: `*this` contains the value/state previously in `other`, and `other` contains the value/state
  previously in `*this`.

---

### Monadic operations

All monadic operations forward additional arguments `args...` to the callable `f`. In `and_then` and `transform`, if
`args...` contains `frt::unwrapped`, the contained value replaces the `frt::unwrapped` placeholder in the invocation;
otherwise, the contained value is passed as the first argument.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto and_then(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Callable `f` must return `frt::optional<U>`. `args...` must contain at most one `frt::unwrapped`
  marker (and not as the sole argument).
- **Effects**: If `self.has_value() == true`, invokes `f` with the forwarded value and returns the resulting optional.
  If `self.has_value() == false`, returns an empty `frt::optional<U>`.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto or_else(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Callable `f` must return an optional of type `frt::optional<T>`. `args...` must contain zero
  `frt::unwrapped` markers.
- **Effects**: If `self.has_value() == false`, invokes `f` with `args...` and returns the resulting optional. If
  `self.has_value() == true`, returns an `optional<T>` containing the forwarded value.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto transform(this Self &&self, F &&f, Args &&...args) noexcept(/* ... */);
```

- **Preconditions**: Same argument forwarding rules as `and_then`. Callable `f` must not return `void`.
- **Effects**: If `self.has_value() == true`, invokes `f` and wraps the result in an `optional<U>` where `U` is the
  return type of `f`. If `self.has_value() == false`, returns an empty `optional<U>`.

---

### Pattern matching

```cpp
template <class Self, class ValueFunc, class NullOptFunc>
[[nodiscard]] constexpr decltype(auto) match(this Self &&self, ValueFunc vf, NullOptFunc nf) noexcept(/* ... */);
```

- **Preconditions**: `vf` must be invocable with the value. `nf` must be invocable without arguments. Both callables
  must have identical return types.
- **Effects**: If `self.has_value() == true`, invokes `vf` with the forwarded value. Otherwise, invokes `nf()`.
- **Returns**: The value returned by `vf` or `nf`.

```cpp
template <class Self, class Func>
[[nodiscard]] constexpr decltype(auto) match(this Self &&self, Func f) noexcept(/* ... */);
```

- **Preconditions**: Callable `f` must be invocable with the value and without arguments, returning the same type in
  both cases.
- **Effects**: Equivalent to calling `self.match(f, f)`.

---

### Comparisons

```cpp
template <class U>
[[nodiscard]] friend constexpr bool
operator==(const optional &lhs, const optional<U> &rhs) noexcept(/* ... */);
```

- **Returns**: `true` if both `lhs` and `rhs` are empty, or if both have values and `lhs.value() == rhs.value()`.
  Returns `false` otherwise.

```cpp
friend constexpr bool operator==(const optional &opt, nullopt_t) noexcept;
```

- **Returns**: `!opt.has_value()`.

```cpp
template <class U>
friend constexpr bool operator==(const optional &opt, const U &value) noexcept(/* ... */);
```

- **Returns**: `opt.has_value() && opt.value() == value`.

```cpp
template <class U>
[[nodiscard]] friend constexpr auto
operator<=>(const optional &lhs, const optional<U> &rhs) noexcept(/* ... */);
```

- **Returns**: `lhs.value() <=> rhs.value()` if both hold values; `false <=> true` if `lhs` is empty and `rhs` has a
  value; `true <=> false` if `lhs` has a value and `rhs` is empty; `std::strong_ordering::equal` if both are empty.

```cpp
friend constexpr std::strong_ordering operator<=>(const optional &opt, nullopt_t) noexcept;
```

- **Returns**: `opt.has_value() <=> false`.

```cpp
template <class U>
friend constexpr std::strong_ordering
operator<=>(const optional &opt, const U &value) noexcept(/* ... */);
```

- **Returns**: `opt.has_value() ? opt.value() <=> value : std::strong_ordering::less`.

---

### Non-member utilities, tags, and helpers

```cpp
friend constexpr void swap(optional &lhs, optional &rhs) noexcept(/* ... */);
```

- **Effects**: Equivalent to `lhs.swap(rhs)`.

```cpp
template <class T>
optional(T) -> optional<T>;
```

- Deduction guide to deduce `optional<T>` from a value argument.

#### Tags

```cpp
struct in_place_opt_t { /* ... */ };
inline constexpr in_place_opt_t in_place_opt{ /* ... */ };

struct nullopt_t { /* ... */ };
inline constexpr nullopt_t nullopt{ /* ... */ };

struct unwrapped_t { /* ... */ };
inline constexpr unwrapped_t unwrapped{ /* ... */ };
```

- Disambiguation tag for in-place construction (`in_place_opt`), empty state representation (`nullopt`), and monadic
  placeholder (`unwrapped`).

#### Concepts

```cpp
template <class T>
concept IsOptional;
```

- Satisfied if `std::remove_cvref_t<T>` is a specialization of `frt::optional`.

#### Standard library integration

- `<frt/hash/std_hash_optional.hpp>` provides a specialization of `std::hash<frt::optional<T>>`.
- `<frt/format/std_fmt_optional.hpp>` provides a specialization of `std::formatter<frt::optional<T>, char>`.
