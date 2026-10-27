# Monadic Operations as Free Functions

In addition to member functions on [`frt::either`](Either.md), [`frt::expected`](Expected.md), and
[`frt::optional`](Optional.md), `frt` provides all monadic operations as free functions (function objects) in the `frt`
namespace:

- `frt::and_then_left`
- `frt::and_then_right`
- `frt::transform_left`
- `frt::transform_right`

These operations work uniformly with any result type that models a two-alternative sum type via `frt::meta_either` and
`frt::is_result_type`. By convention:

- The **left** alternative represents the first alternative of `either`, the unexpected error `E` of `expected`, or the
  empty state (`nullopt`) of `optional`.
- The **right** alternative represents the second alternative of `either`, the expected value `T` of `expected`, or the
  contained value `T` of `optional`.

| Free function          | Callable signature  | Description                                                                                                        |
|------------------------|---------------------|--------------------------------------------------------------------------------------------------------------------|
| `frt::and_then_left`   | `L -> Result<U, R>` | Chains a computation returning a result type on the left alternative (like `or_else` on `expected`/`optional`).    |
| `frt::and_then_right`  | `R -> Result<L, U>` | Chains a computation returning a result type on the right alternative (like `and_then` on `expected`/`optional`).  |
| `frt::transform_left`  | `L -> U`            | Maps the left alternative to a new value wrapped in the left branch (like `transform_error` on `expected`).        |
| `frt::transform_right` | `R -> U`            | Maps the right alternative to a new value wrapped in the right branch (like `transform` on `expected`/`optional`). |

All free functions support the same [Inline Partial Application](MonadicOperations.md#inline-partial-application)
capabilities as member functions, including trailing argument packs and the `frt::unwrapped` positional placeholder.

---

## Invocation Syntaxes

Every free monadic operation supports two invocation styles:

1. **Direct free-function call**: `frt::op(result, f, args...)`
2. **Pipe (`|`) syntax (partial application)**: `result | frt::op(f, args...)`

### 1. Direct free-function syntax

When the first argument satisfies `frt::IsResultType`, the operation is executed immediately. Extra arguments `args...`
are forwarded directly to the callable `f` without creating any intermediate closure objects:

```c++
#include <frt/either.hpp>

struct Error;
struct Context;
struct Path;
struct MetaData;
struct Component;
struct Widget;

frt::either<Error, Context> extractContext();
frt::either<Error, Component> createComponent(Path, Context, MetaData);
Widget toWidget(Component, const MetaData&);
frt::either<Error, Widget> fallbackWidget(Error);

Path path{/* ... */};
MetaData metaData{/* ... */};

auto contextOrError = extractContext();

auto componentOrError = frt::and_then_right(
    std::move(contextOrError),
    &createComponent,
    path, frt::unwrapped, metaData);

auto widgetOrError = frt::transform_right(
    std::move(componentOrError),
    &toWidget,
    metaData);

auto finalResult = frt::and_then_left(
    std::move(widgetOrError),
    &fallbackWidget);
```

### 2. Pipe (`|`) syntax

When a free monadic operation is called **without** a leading result object — `frt::op(f, args...)` — it returns a
partially applied closure object that can be chained with `operator|`:

```c++
Path path{/* ... */};
MetaData metaData{/* ... */};

auto finalResult = extractContext()
    | frt::and_then_right(&createComponent, path, frt::unwrapped, metaData)
    | frt::transform_right(&toWidget, metaData)
    | frt::and_then_left(&fallbackWidget);
```

---

## Hidden Caveats of the `|` Syntax

While the pipe syntax (`|`) is concise and expressive, it has important semantic and performance differences compared
to direct invocation (`result.and_then_right(...)` or `frt::and_then_right(result, ...)`).

### 1. Eager capture and unconditional copies/moves

In a direct call such as `frt::and_then_right(result, f, a, b)`, the extra arguments `a` and `b` are simply forwarded by
reference and are **never touched** if `result` holds the left alternative.

In contrast, in a pipe expression:

```c++
auto res = result | frt::and_then_right(f, a, b);
```

the subexpression `frt::and_then_right(f, a, b)` is evaluated **before** `operator|` inspects `result`. To return a
standalone closure object, it must decay and capture `f`, `a`, and `b` **by value** (`std::decay_t<F>` and
`std::decay_t<Args>...`):

- **Work is performed even when short-circuiting**: Copying or moving `f` and `args...` into the closure happens
  unconditionally, even if `result` holds the inactive alternative and `f` is never called.
- **Pass expensive objects with `std::ref` / `std::cref`**: To avoid copying large objects (such as a `Window` hierarchy
  or `Context`) into the closure, or when the callable mutates an outer variable by reference, wrap those arguments in
  `std::ref` or `std::cref`:

```c++
struct Layout;
struct WindowManager {
    frt::either<Error, Window> embed(Layout& layout, Component component) const;
};

WindowManager manager{/* ... */};
Layout layout{/* ... */};

// Avoids copying `manager` and passes `layout` by mutable reference
auto windowOrError = createComponent(path, std::move(ctx), metaData)
    | frt::and_then_right(&WindowManager::embed,
                          std::cref(manager),
                          std::ref(layout),
                          frt::unwrapped);
```

### 2. Move-constructibility requirements on `F` and `Args...`

Because the pipe closure stores decayed copies of the callable and extra arguments, every type `T` in `F` and `Args...`
must satisfy the same requirements as `std::bind_front` and `std::bind_back`:

- `std::is_constructible_v<std::decay_t<T>, T>`
- `std::is_move_constructible_v<std::decay_t<T>>`

If an extra argument is non-movable (for example, a synchronization primitive or a pinned GUI context), passing it by
value to `frt::and_then_right(f, nonMovable)` will fail to compile unless wrapped in `std::ref` or `std::cref`. Direct
invocation (`frt::and_then_right(result, f, nonMovable)`) has no such restriction because no closure is constructed.

### 3. Value category of stored closures

The value category of the closure object itself determines how its captured callable and arguments are forwarded when
`operator|` is evaluated:

- **Rvalue closure (`result | frt::and_then_right(...)` or `result | std::move(closure)`)**: The captured callable and
  arguments are forwarded as rvalues. This is required when capturing move-only types (such as
  `std::unique_ptr<Widget>`), and means a closure holding move-only state can only be consumed once.
- **Lvalue closure (`result | closure`)**: The captured callable and arguments are forwarded as lvalues (or `const`
  lvalues if the closure is `const`), allowing the closure to be safely reused across multiple pipe expressions without
  moving from its stored state.

---

## Customizing Monadic Operations with `frt::meta_either`

All free monadic operations (as well as [Match](Match.md), [Traverse](Traverse.md), and
[Sequence](Sequence.md) algorithms) operate on result types through two customization points in namespace `frt`:

1. **`frt::is_result_type<T>`**: A `bool` variable template that must be specialized to `true` for the result type `T`
   (which satisfies the `frt::IsResultType` concept).
2. **`frt::meta_either<T>`**: A class template specialization that defines how to inspect, extract, rebind, and
   construct the left and right alternatives of `T`.

```c++
#include <frt/concept/concept.hpp>
#include <frt/meta/meta_either.hpp>
```

### `frt::meta_either` interface

A complete specialization of `frt::meta_either<Self>` can provide the following members:

```c++
namespace frt {

template <>
struct meta_either<MyResult> {
    // Type of the left alternative (or frt::void_t / void if unvalued)
    using left_t = /* ... */;

    // Type of the right alternative (or frt::void_t / void if unvalued)
    using right_t = /* ... */;

    // Result type with the left alternative changed to U (used by transform_left)
    template <class U>
    using rebind_left = /* ... */;

    // Result type with the right alternative changed to U (used by transform_right)
    template <class U>
    using rebind_right = /* ... */;

    // Unique non-template tag type identifying the result type family (used by and_then_left / and_then_right)
    using tag_t = /* ... */;

    // Observers checking which alternative is active: (const Self&) -> bool
    static constexpr auto is_left  = [](const Self& self) -> bool { /* ... */ };
    static constexpr auto is_right = [](const Self& self) -> bool { /* ... */ };

    // Accessors forwarding the active alternative: (S&&) -> decltype(auto)
    static constexpr auto left  = []<class S>(S&& self) -> decltype(auto) { /* ... */ };
    static constexpr auto right = []<class S>(S&& self) -> decltype(auto) { /* ... */ };

    // Factory callables constructing Self from an alternative: (U&&) -> Self
    static constexpr auto make_left  = []<class U>(U&& val) -> Self { /* ... */ };
    static constexpr auto make_right = []<class U>(U&& val) -> Self { /* ... */ };
};

} // namespace frt
```

| Member                    | Purpose                                                                                                                                  | Required by                                                                                                                              |
|---------------------------|------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| `left_t`, `right_t`       | Alternative types of the result object. Use `void` or `frt::void_t` when a branch carries no value.                                      | All operations                                                                                                                           |
| `tag_t`                   | A unique, non-template type identifying the result family (e.g., ensuring `and_then_right` on `either` returns an `either`).             | `and_then_left`, `and_then_right`                                                                                                        |
| `rebind_left<U>`          | Alias for the result type with its left alternative replaced by `U`.                                                                     | `transform_left`                                                                                                                         |
| `rebind_right<U>`         | Alias for the result type with its right alternative replaced by `U`.                                                                    | `transform_right`                                                                                                                        |
| `is_left`, `is_right`     | Predicates returning `true` when the respective alternative is active.                                                                   | `is_left`: `and_then_left`, `transform_left`<br>`is_right`: `and_then_right`, `transform_right`                                          |
| `left`, `right`           | Extractors forwarding the contained left or right value (or returning `frt::void_val` when the alternative is `void` / empty).           | All operations                                                                                                                           |
| `make_left`, `make_right` | Factories constructing a result instance holding the left or right alternative (accepting `frt::void_t` when the alternative is `void`). | `make_left`: `and_then_right`, `transform_left`, `transform_right`<br>`make_right`: `and_then_left`, `transform_left`, `transform_right` |

---

## Standard Library Examples: `std::expected` and `std::optional`

The `frt` library ships with built-in `frt::meta_either` specializations for `std::expected` and `std::optional`:

- `<frt/meta/std/meta_either_std_expected.hpp>`
- `<frt/meta/std/meta_either_std_optional.hpp>`

These headers serve as reference examples of how to adapt external types to `frt`'s monadic operations.

### Example 1: `frt::meta_either` for `std::expected`

In `<frt/meta/std/meta_either_std_expected.hpp>`, `std::expected<T, E>` is mapped so that `left_t` is `error_type` (`E`)
and `right_t` is `value_type` (`T`). When `T` is `void`, `right` returns `frt::void_val` (of type `frt::void_t`), and
`make_right` constructs `std::expected<void, E>{std::in_place}`:

```c++
#include <frt/concept/concept.hpp>
#include <frt/meta/meta_either.hpp>
#include <frt/util/void.hpp>

#include <expected>
#include <type_traits>
#include <utility>

namespace frt {

template <class T, class E>
inline constexpr bool is_result_type<std::expected<T, E>> = true;

template <class T, class E>
struct meta_either<std::expected<T, E>> {
    using Self = std::expected<T, E>;

    using right_t = Self::value_type;
    using left_t  = Self::error_type;

    template <class U>
    using rebind_right = Self::template rebind<U>;

    template <class U>
    using rebind_left = std::expected<T, U>;

    static constexpr auto left = []<class S>(S&& self) -> decltype(auto)
        requires(std::same_as<std::remove_cvref_t<S>, Self>)
    {
        return std::forward<S>(self).error();
    };

    static constexpr auto right = []<class S>(S&& self) -> decltype(auto)
        requires(std::same_as<std::remove_cvref_t<S>, Self>)
    {
        if constexpr (std::is_void_v<std::remove_cvref_t<right_t>>) {
            return void_val;
        } else {
            return std::forward<S>(self).value();
        }
    };

    static constexpr auto is_left  = [](const Self& self) { return !self.has_value(); };
    static constexpr auto is_right = [](const Self& self) { return self.has_value(); };

    static constexpr auto make_left = []<class U>(U&& val) {
        return Self{std::unexpect, std::forward<U>(val)};
    };

    static constexpr auto make_right = []<class U>(U&& val) {
        if constexpr (std::same_as<void_t, std::remove_cvref_t<U>>) {
            (void)val;
            return Self{std::in_place};
        } else {
            return Self{std::in_place, std::forward<U>(val)};
        }
    };
};

} // namespace frt
```

### Example 2: `frt::meta_either` for `std::optional`

In `<frt/meta/std/meta_either_std_optional.hpp>`, `std::optional<T>` has only one payload type (`T`), while its empty
state (`std::nullopt`) carries no data. This is naturally modeled by setting `right_t` to `T` and `left_t` to
`frt::void_t`:

```c++
#include <frt/concept/concept.hpp>
#include <frt/meta/meta_either.hpp>
#include <frt/util/void.hpp>

#include <optional>
#include <type_traits>
#include <utility>

namespace frt {

template <class T>
inline constexpr bool is_result_type<std::optional<T>> = true;

template <class L>
struct meta_either<std::optional<L>> {
    using Self = std::optional<L>;

    using right_t = Self::value_type;
    using left_t  = void_t;

    template <class U>
    using rebind_right = std::optional<U>;

    template <class>
    using rebind_left = std::optional<L>;

    static constexpr auto left = []<class S>(S&&) -> decltype(auto)
        requires(std::same_as<std::remove_cvref_t<S>, Self>)
    {
        return void_val;
    };

    static constexpr auto right = []<class S>(S&& self) -> decltype(auto)
        requires(std::same_as<std::remove_cvref_t<S>, Self>)
    {
        return std::forward<S>(self).value();
    };

    static constexpr auto is_left  = [](const Self& self) { return !self.has_value(); };
    static constexpr auto is_right = [](const Self& self) { return self.has_value(); };

    static constexpr auto make_left  = []<class U>(U&&) { return Self{}; };
    static constexpr auto make_right = []<class U>(U&& val) {
        return Self{std::in_place, std::forward<U>(val)};
    };
};

} // namespace frt
```

By including `<frt/meta/std/meta_either_std_expected.hpp>` or `<frt/meta/std/meta_either_std_optional.hpp>`,
`std::expected` and `std::optional` immediately gain support for `frt`'s free monadic operations and inline partial
application:

```c++
#include <frt/algorithm/transform_right.hpp>
#include <frt/meta/std/meta_either_std_optional.hpp>

struct Context;
struct Path;
struct Component;

std::optional<Context> findActiveContext();
Component buildComponent(const Path&, Context);

Path path{/* ... */};

std::optional<Component> component = findActiveContext()
    | frt::transform_right(&buildComponent, path, frt::unwrapped);
```

---

## Which Types Should Specialize `frt::meta_either`?

### Well-suited: Sum types of two alternatives

The `frt::meta_either` customization mechanism works cleanly for **value-semantic sum types of two alternatives** that
are naturally representable as an `Either` monad:

- Tagged unions with two states (such as custom `Result<T, E>`, `Outcome<T, Error>`, or `Either<L, R>` types).
- Optional-like value wrappers where one state holds a value `T` and the other state represents absence (`void_t`).
- Two-element `std::variant<L, R>` wrappers with well-defined left and right roles.

### Not recommended: Pointer-like types (including smart pointers)

Although a nullable pointer (`T*`, `std::unique_ptr<T>`, or `std::shared_ptr<T>`) superficially resembles an optional
value (either non-null or `nullptr`), customizing `frt::meta_either` for pointer-like types is **not a good idea**:

1. **Ownership, lifetime, and hidden allocations**:
    - For raw pointers (`T*`), `make_right` cannot safely construct a new `T` without an owning allocation or taking the
      address of a temporary (which immediately dangles).
    - For smart pointers (`std::unique_ptr<T>`, `std::shared_ptr<T>`), every `transform_right` step would be forced to
      perform a dynamic heap allocation (`std::make_unique` / `std::make_shared`) to wrap the transformed result.
2. **Shallow `const` and reference semantics**:
    - Sum types propagate their own constness and value category (`&`, `const&`, `&&`) to the contained value.
    - Pointers have shallow constness (`const std::unique_ptr<Widget>&` still dereferences to a mutable `Widget&`),
      which
      breaks the value-category forwarding guarantees of monadic operations.
3. **Incompatible `void` rebinding**:
    - Monadic transformations allow functions that return `void` (producing `rebind_right<void>`). Neither `void*` nor
      `std::unique_ptr<void>` can represent a valid dereferenceable `void` value distinct from `nullptr`.

If a function returns a raw or smart pointer and you want to use monadic operations, convert the pointer check into an
explicit `frt::optional` (for example, `frt::optional< std::reference_wrapper<Widget> >` for non-owning access) or
`frt::expected` at the boundary rather than specializing `frt::meta_either` for pointer types.
