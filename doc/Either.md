# Either

This is a basic sum type that can contain either a left value or a right value. Neither the left nor the right
alternative has a special meaning. Historically, in many functional languages, the left alternative is considered a
"negative" outcome of a computation and the right alternative a "positive" outcome. If you need to represent such
semantics, there is the more specialized type [Expected](Expected.md).

```c++
struct Widget;
struct Window;

using WidgetOrWindow = frt::either<Widget, Window>;

// The operation cannot fail. The result is always either a widget or a window
WidgetOrWindow load( /* ... */ );

// ...

auto widgetOrWindow = load( /* ... */ );
if (widgetOrWindow.is_left()) {
    auto widget = widgetOrWindow.left();
    // Do something with the widget
} else {
    auto window = widgetOrWindow.right(); 
    // Do something with the window
}
```

## Table of contents

- [Monadic operations](#monadic-operations)
- [Match](#match)
- [API reference](#api-reference)
  - [Template parameters](#template-parameters)
  - [Member types](#member-types)
  - [Constructors and assignment](#constructors-and-assignment)
  - [Observers](#observers)
  - [Modifiers](#modifiers)
  - [Monadic operations](#monadic-operations-1)
  - [Pattern matching](#pattern-matching)
  - [Comparisons](#comparisons)
  - [Non-member utilities and tags](#non-member-utilities-and-tags)

---

## Monadic operations

The type comes with monadic operations named to conform to both standard terminology and the nature of the type. Assume
you have `frt::either<L, R>`, where `L` is the type of the left alternative and `R` is the type of the right
alternative. The type `U` indicates that an operation can change the type of an alternative.

| Operation                   | `frt` name        | Also known as        |
|-----------------------------|-------------------|----------------------|
| `f: L -> frt::either<U, R>` | `and_then_left`   | orElse / recoverWith |
| `f: R -> frt::either<L, U>` | `and_then_right`  | bind / flatMap       |
| `f: L -> U`                 | `transform_left`  | leftMap              |
| `f: R -> U`                 | `transform_right` | map / fmap           |

```c++
struct Component;

auto widgetOrWindow = load( /* ... */ );

auto result = widgetOrWindow
    .and_then_left([](const Widget& widget) -> frt::either<Component, Window> {
        // Convert the widget to component
    })
    .and_then_right([](const Window& window) -> frt::either<Component, Component> {
        // Convert the window to component
    })
    .transform_left([](const Component& leftComponent) -> Component {
        // Do something with the left component (former Widget)
    })
    .transform_right([](const Component& rightComponent) -> Component {
        // Do something with the right component (former Window)
    });
```

All monadic operations support extended syntax. You can find more information in
the [Monadic Operations](MonadicOperations.md) documentation.

## Match

There is another operation called `match` that allows you to handle both alternatives in a single call. This operation
differs from regular monadic operations because it does not impose strict requirements on the return type and can mutate
the value in place. `match` provides two overloads: the first accepts two functions for the left and right alternatives,
and the second accepts a single function that can be invoked for both alternatives. In fact, it works similarly to
pattern matching.

```c++
auto widgetOrWindow = load( /* ... */ );

widgetOrWindow.match(
    [](const Widget& widget) {
        // Do something with the widget
        // You can mutate the widget in place if the type is `Widget&`
        // You can return something from the function
    },
    [](const Window& window) {
        // Do something with the window
        // You can mutate the window in place if the type is `Window&`
        // You can return something from the function
    }
);

widgetOrWindow.match([](const auto& value) {
    // This function will be invoked for widget and window
    // For example, if both are printable it can be:
    std::println("Component: {}", value);    
});
```

## API reference

```cpp
#include <frt/either.hpp>

namespace frt {

template <class Left, class Right>
struct [[nodiscard]] either;

}
```

### Template parameters

- `Left`: Type of the left alternative. Must be `void` or a destructible, non-reference, non-array object type.
- `Right`: Type of the right alternative. Must be `void` or a destructible, non-reference, non-array object type.

Neither type may be a tag type (`in_place_left_t`, `in_place_right_t`, `unwrapped_t`).

---

### Member types

| Type                         | Description                                                         |
|------------------------------|---------------------------------------------------------------------|
| `left_t`                     | Type of the left alternative (`Left`, stripped of cv-qualifiers).   |
| `right_t`                    | Type of the right alternative (`Right`, stripped of cv-qualifiers). |
| `rebind_left<AnotherLeft>`   | Alias for `either<AnotherLeft, right_t>`.                           |
| `rebind_right<AnotherRight>` | Alias for `either<left_t, AnotherRight>`.                           |

---

### Constructors and assignment

```cpp
either() = delete;
```

- **Description**: Default constructor is deleted. An uninitialized state is not permitted.

```cpp
constexpr either(const either &src);
constexpr either(either &&src);

template <class AnotherLeft, class AnotherRight>
constexpr either(const either<AnotherLeft, AnotherRight> &src);

template <class AnotherLeft, class AnotherRight>
constexpr either(either<AnotherLeft, AnotherRight> &&src);
```

- **Postconditions**: `is_left() == src.is_left()`. The active alternative is initialized from `src`'s active
  alternative.

```cpp
template <class... Args>
constexpr explicit either(in_place_left_t, Args &&...args);

template <class U, class... Args>
constexpr explicit either(in_place_left_t, std::initializer_list<U> ilist, Args &&...args);
```

- **Postconditions**: `is_left() == true`. The left alternative is constructed in-place with `args...` (or
  `ilist, args...`).

```cpp
template <class... Args>
constexpr explicit either(in_place_right_t, Args &&...args);

template <class U, class... Args>
constexpr explicit either(in_place_right_t, std::initializer_list<U> ilist, Args &&...args);
```

- **Postconditions**: `is_right() == true`. The right alternative is constructed in-place with `args...` (or
  `ilist, args...`).

```cpp
constexpr either &operator=(const either &src);
constexpr either &operator=(either &&src);

template <class AnotherLeft, class AnotherRight>
constexpr either &operator=(const either<AnotherLeft, AnotherRight> &src);

template <class AnotherLeft, class AnotherRight>
constexpr either &operator=(either<AnotherLeft, AnotherRight> &&src);
```

- **Effects**: Destroys the currently held alternative and re-initializes from `src`.
- **Postconditions**: `is_left() == src.is_left()`.
- **Returns**: `*this`.

---

### Observers

```cpp
[[nodiscard]] constexpr bool is_left() const noexcept;
```

- **Returns**: `true` if `*this` contains the left alternative; otherwise `false`.

```cpp
[[nodiscard]] constexpr bool is_right() const noexcept;
```

- **Returns**: `true` if `*this` contains the right alternative; otherwise `false`.

```cpp
template <class Self>
[[nodiscard]] constexpr auto &&left(this Self &&self);
```

- **Preconditions**: `self.is_left() == true`.
- **Returns**: A reference to the contained left value, forwarded with the value category and constness of `self`.

```cpp
template <class Self>
[[nodiscard]] constexpr auto &&right(this Self &&self);
```

- **Preconditions**: `self.is_right() == true`.
- **Returns**: A reference to the contained right value, forwarded with the value category and constness of `self`.

```cpp
template <class U = left_t>
[[nodiscard]] constexpr auto left_or(U &&default_value) const &;

template <class U = left_t>
[[nodiscard]] constexpr auto left_or(U &&default_value) &&;
```

- **Returns**: The contained left value if `is_left() == true`; otherwise
  `static_cast<left_t>(std::forward<U>(default_value))`.

```cpp
template <class U = right_t>
[[nodiscard]] constexpr auto right_or(U &&default_value) const &;

template <class U = right_t>
[[nodiscard]] constexpr auto right_or(U &&default_value) &&;
```

- **Returns**: The contained right value if `is_right() == true`; otherwise
  `static_cast<right_t>(std::forward<U>(default_value))`.

---

### Modifiers

```cpp
template <class... Args>
constexpr auto &emplaceLeft(Args &&...args);

template <class U, class... Args>
constexpr auto &emplaceLeft(std::initializer_list<U> ilist, Args &&...args);
```

- **Effects**: Destroys the current alternative and constructs the left alternative in-place.
- **Postconditions**: `is_left() == true`.
- **Returns**: Reference to the newly constructed left alternative.

```cpp
template <class... Args>
constexpr auto &emplaceRight(Args &&...args);

template <class U, class... Args>
constexpr auto &emplaceRight(std::initializer_list<U> ilist, Args &&...args);
```

- **Effects**: Destroys the current alternative and constructs the right alternative in-place.
- **Postconditions**: `is_right() == true`.
- **Returns**: Reference to the newly constructed right alternative.

```cpp
constexpr void swap(either &rhs) noexcept(/* ... */);
```

- **Effects**: Exchanges the state and contained alternatives of `*this` and `rhs`.
- **Postconditions**: `*this` contains the alternative previously in `rhs`, and `rhs` contains the alternative
  previously in `*this`.

---

### Monadic operations

All monadic operations forward additional arguments `args...` to the callable `f`. If `args...` contains
`frt::unwrapped`, the alternative value replaces the `frt::unwrapped` placeholder in the invocation; otherwise, the
alternative value is passed as the first argument. When the alternative is `void`, `f` is invoked with `args...` only,
and `frt::unwrapped` is prohibited.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto and_then_left(this Self &&self, F &&f, Args &&...args);
```

- **Preconditions**: Callable `f` must return `frt::either<NewLeft, right_t>`. If `left_t` is not `void`, `args...` must
  contain at most one `frt::unwrapped` marker (and not as the sole argument).
- **Effects**: If `self.is_left() == true`, invokes `f` and returns the resulting either. If `self.is_right() == true`,
  returns an `either<NewLeft, right_t>` containing the forwarded right alternative.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto and_then_right(this Self &&self, F &&f, Args &&...args);
```

- **Preconditions**: Callable `f` must return `frt::either<left_t, NewRight>`. If `right_t` is not `void`, `args...`
  must contain at most one `frt::unwrapped` marker (and not as the sole argument).
- **Effects**: If `self.is_right() == true`, invokes `f` and returns the resulting either. If `self.is_left() == true`,
  returns an `either<left_t, NewRight>` containing the forwarded left alternative.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto transform_left(this Self &&self, F &&f, Args &&...args);
```

- **Preconditions**: Same argument forwarding rules as `and_then_left`.
- **Effects**: If `self.is_left() == true`, invokes `f` and wraps the result in an `either<NewLeft, right_t>` as the
  left alternative (or `void` if `f` returns `void`). If `self.is_right() == true`, returns an
  `either<NewLeft, right_t>` containing the forwarded right alternative.

```cpp
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto transform_right(this Self &&self, F &&f, Args &&...args);
```

- **Preconditions**: Same argument forwarding rules as `and_then_right`.
- **Effects**: If `self.is_right() == true`, invokes `f` and wraps the result in an `either<left_t, NewRight>` as the
  right alternative (or `void` if `f` returns `void`). If `self.is_left() == true`, returns an
  `either<left_t, NewRight>` containing the forwarded left alternative.

---

### Pattern matching

```cpp
template <class Self, class LeftFunc, class RightFunc>
constexpr decltype(auto) match(this Self &&self, LeftFunc &&lf, RightFunc &&rf);
```

- **Preconditions**: `lf` must be invocable with the left alternative (or with no arguments if `left_t` is `void`). `rf`
  must be invocable with the right alternative (or with no arguments if `right_t` is `void`). Both callables must have
  identical return types (or both return `void`).
- **Effects**: If `self.is_left() == true`, invokes `lf`. Otherwise, invokes `rf`.
- **Returns**: The value returned by `lf` or `rf` (or `void`).

```cpp
template <class Self, class Func>
constexpr decltype(auto) match(this Self &&self, Func &&f);
```

- **Preconditions**: Callable `f` must be invocable with either alternative (or with no arguments if an alternative is
  `void`) and return the same type in both cases.
- **Effects**: Equivalent to calling `match(std::forward<Func>(f), std::forward<Func>(f))`.

---

### Comparisons

```cpp
template <class L, class R>
[[nodiscard]] friend constexpr bool
operator==(const either &lhs, const either<L, R> &rhs);
```

- **Returns**: `true` if `lhs` and `rhs` hold the same alternative and their contained values compare equal
  (`lhs.left() == rhs.left()` or `lhs.right() == rhs.right()`). Returns `false` otherwise.

```cpp
template <class L, class R>
[[nodiscard]] friend constexpr std::partial_ordering
operator<=>(const either &lhs, const either<L, R> &rhs);
```

- **Returns**: `lhs.left() <=> rhs.left()` if both hold the left alternative; `lhs.right() <=> rhs.right()` if both hold
  the right alternative; `std::partial_ordering::unordered` if they hold different alternatives.

---

### Non-member utilities and tags

```cpp
friend constexpr void swap(either &lhs, either &rhs) noexcept(/* ... */);
```

- **Effects**: Equivalent to `lhs.swap(rhs)`.

```cpp
struct in_place_left_t { /* ... */ };
inline constexpr in_place_left_t in_place_left{ /* ... */ };

struct in_place_right_t { /* ... */ };
inline constexpr in_place_right_t in_place_right{ /* ... */ };
```

- In-place construction disambiguation tags for left and right alternatives.

```cpp
struct unwrapped_t { /* ... */ };
inline constexpr unwrapped_t unwrapped{ /* ... */ };
```

- Placeholder tag for specifying the position of the unwrapped alternative in monadic argument packs.

```cpp
template <class T>
concept IsEither;
```

- Concept satisfied if `std::remove_cvref_t<T>` is a specialization of `frt::either`.
