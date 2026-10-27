# Match

`match` provides exhaustive pattern matching for result types ([`frt::either`](Either.md),
[`frt::expected`](Expected.md), [`frt::optional`](Optional.md), and any custom type adapted via
[`frt::meta_either`](MonadicOperationsAsFreeFunctions.md#customizing-monadic-operations-with-frtmeta_either)).

While [monadic operations](MonadicOperations.md) (`and_then*`, `transform*`, `or_else`) transform or chain a single
active branch and keep the computation inside the same result type family, `match` handles **both** alternatives in a
single call and can unwrap the result into any common return type (or `void` for side effects).

Key properties of `match`:

- **Exhaustive**: You must provide handlers capable of being invoked for both the left and right alternatives.
- **Unrestricted return type**: The handlers can return any type (including references or `void`), provided both
  branches return the exact same type.
- **Value-category and constness preservation**: The contained alternative is forwarded with the exact value category
  and constness of the result object (`&`, `const&`, `&&`, `const&&`). Handlers can mutate the contained value in place
  or move it out without copies.
- **`void` alternative support**: If an alternative has type `void` (or `frt::void_t`, such as the empty state of an
  optional), the corresponding handler is invoked with zero arguments (`()`).

---

## Overloads

`match` provides two overloads (available as a member function, a free function `frt::match`, and a pipeable adaptor):

1. **Two callables (`match(lf, rf)`)**:
    - Invokes the first callable if the first alternative is active, or the second callable if the second alternative is
      active.
    - Both callables must have the exact same return type (or both return `void`).

2. **Single callable (`match(f)`)**:
    - Equivalent to calling `match(f, f)`.
    - The single callable `f` must be invocable with both alternatives and return the same type in both cases.
    - Particularly useful with generic lambdas (`[](const auto& value) { ... }`) when both alternatives share a common
      interface (for example, `std::formattable` or a common GUI method) or with overloaded visitor structs.

---

## Usage with `frt::either`

Assume the following GUI types and helper functions:

```c++
#include <frt/either.hpp>
#include <format>
#include <print>
#include <string>

struct Widget {
    std::string name;
    void resize(int width, int height);
};

struct Window {
    std::string title;
    void resize(int width, int height);
};

using WidgetOrWindow = frt::either<Widget, Window>;

WidgetOrWindow loadGuiElement(/* ... */);
```

### 1. Built-in member function (`.match(...)`)

`frt::either<L, R>::match` can be called directly on an `either` instance:

```c++
WidgetOrWindow element = loadGuiElement();

// 1. Two callables: return a common type (std::string) from either branch
std::string label = element.match(
    [](const Widget& widget) {
        return std::format("Widget: {}", widget.name);
    },
    [](const Window& window) {
        return std::format("Window: {}", window.title);
    }
);

// 2. Two callables: mutate the active alternative in-place (returns void)
element.match(
    [](Widget& widget) { widget.resize(400, 300); },
    [](Window& window) { window.resize(1280, 720); }
);

// 3. Single callable: invoke a shared interface on whichever alternative is active
element.match([](auto& item) {
    item.resize(800, 600);
});
```

Because `.match` forwards the value category of `*this`, calling `.match` on an rvalue `either` passes rvalue
references (`Widget&&` / `Window&&`) to the handlers, allowing move-only extraction:

```c++
std::string extractedName = std::move(element).match(
    [](Widget&& widget) { return std::move(widget.name); },
    [](Window&& window) { return std::move(window.title); }
);
```

### 2. Free function (`frt::match(result, ...)`)

`frt::match` in `<frt/algorithm/match.hpp>` (also included by `<frt/either.hpp>`) can be invoked as a free function with
the result object as its first argument:

```c++
#include <frt/algorithm/match.hpp>

WidgetOrWindow element = loadGuiElement();

// Two callables
std::string label = frt::match(
    element,
    [](const Widget& widget) { return widget.name; },
    [](const Window& window) { return window.title; }
);

// Single callable
frt::match(element, [](auto& item) {
    item.resize(1024, 768);
});
```

### 3. Pipe (`|`) syntax (`result | frt::match(...)`)

When called without a leading result object, `frt::match(lf, rf)` and `frt::match(f)` return a partially applied closure
object that can be used with the pipe operator (`|`). This is especially natural at the end of a monadic pipeline:

```c++
struct Context;
struct Component;
struct Error;

frt::either<Error, Context> extractContext();
frt::either<Error, Component> createComponent(Context);
Widget toWidget(Component);

// Chain monadic operations and unwrap the final outcome with `| frt::match(...)`
std::string statusMessage = extractContext()
    | frt::and_then_right(&createComponent)
    | frt::transform_right(&toWidget)
    | frt::match(
        [](const Error& error)  { return std::format("Failed to build widget: {}", error); },
        [](const Widget& widget) { return std::format("Created widget: {}", widget.name); }
    );

// Single callable with pipe syntax
loadGuiElement() | frt::match([](auto& item) {
    item.resize(800, 600);
});
```

A partially applied `frt::match` closure can also be stored in a variable and reused across multiple result objects:

```c++
const auto getElementName = frt::match(
    [](const Widget& widget) { return widget.name; },
    [](const Window& window) { return window.title; }
);

std::string first  = loadGuiElement() | getElementName;
std::string second = loadGuiElement() | getElementName;
```

> [!NOTE]
> Just like the [pipe syntax for monadic operations](MonadicOperationsAsFreeFunctions.md#hidden-caveats-of-the--syntax),
> `frt::match(lf, rf)` and `frt::match(f)` capture the callables **by value** (`std::decay_t<F>`) into an intermediate
> closure object. The decayed callable types must be move-constructible. If a callable holds expensive state or needs to
> mutate its own state across calls, wrap it in `std::ref` or `std::cref`.

---

## Handling `void` Alternatives

When the left alternative, the right alternative, or both alternatives are `void` (or `frt::void_t`, such as the empty
branch of an optional), the corresponding handler must be invocable with **no arguments** (`()`):

```c++
using ErrorOrReady = frt::either<Error, void>;

ErrorOrReady status = initializeWindowSystem();

// Right alternative is `void`, so the right callable takes no arguments
bool ready = status | frt::match(
    [](const Error& err) {
        logError(err);
        return false;
    },
    []() {
        return true;
    }
);
```

When using the single-callable overload with a `void` alternative, you can pass an overloaded callable or a variadic
generic lambda that accepts either 0 or 1 argument:

```c++
struct Visitor {
    std::string operator()(const Error& err) const { return "Error occurred"; }
    std::string operator()() const                 { return "Initialized"; }
};

std::string message = status | frt::match(Visitor{});
```

---

## Customizing `frt::match` for Other Types

`frt::match` works out of the box with `frt::either`, `frt::expected`, `frt::optional`, as well as `std::expected` and
`std::optional` (when `<frt/meta/std/meta_either_std_expected.hpp>` or `<frt/meta/std/meta_either_std_optional.hpp>` is
included).

To enable `frt::match` (both direct and `|` pipe syntax) for a custom sum type `Result`, specialize:

1. `frt::is_result_type<Result>` (set to `true`).
2. `frt::meta_either<Result>`.

### Required `frt::meta_either` interface for `frt::match`

Unlike `and_then_*` and `transform_*`, `frt::match` only inspects and extracts from the input result object and never
constructs or rebinds a result type. Consequently, `frt::meta_either<Result>` only needs to provide **five** members:

| Member                                | Requirement                    | Description                                                                                                                                  |
|---------------------------------------|--------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------|
| `left_t`                              | Type alias                     | Type of the left alternative (use `void` or `frt::void_t` if the left state carries no value).                                               |
| `right_t`                             | Type alias                     | Type of the right alternative (use `void` or `frt::void_t` if the right state carries no value).                                             |
| `is_left(result)`                     | `(const Result&) -> bool`      | Returns `true` if `result` holds the left alternative, `false` if it holds the right alternative.                                            |
| `left(std::forward<Result>(result))`  | `(Result&&) -> decltype(auto)` | Returns the contained left value forwarded with the value category of `result` (or `frt::void_val` if `left_t` is `void` / `frt::void_t`).   |
| `right(std::forward<Result>(result))` | `(Result&&) -> decltype(auto)` | Returns the contained right value forwarded with the value category of `result` (or `frt::void_val` if `right_t` is `void` / `frt::void_t`). |

> [!TIP]
> `tag_t`, `rebind_left`, `rebind_right`, `is_right`, `make_left`, and `make_right` are **not** required if you only use
> `frt::match` with your type. For full compatibility with all free monadic operations, see
> [Monadic Operations as Free Functions](MonadicOperationsAsFreeFunctions.md#customizing-monadic-operations-with-frtmeta_either).

### Custom type example

Suppose you have a custom `GuiEventResult` sum type that holds either an `Error` or a `Widget`:

```c++
#include <frt/algorithm/match.hpp>
#include <frt/concept/concept.hpp>
#include <frt/meta/meta_either.hpp>

#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>

struct Error { std::string reason; };
struct Widget { std::string name; };

struct GuiEventResult {
    std::variant<Error, Widget> storage;
};

namespace frt {

template <>
inline constexpr bool is_result_type<GuiEventResult> = true;

template <>
struct meta_either<GuiEventResult> {
    using left_t  = Error;
    using right_t = Widget;

    static constexpr auto is_left = [](const GuiEventResult& res) noexcept {
        return res.storage.index() == 0;
    };

    static constexpr auto left = []<class S>(S&& res) -> decltype(auto)
        requires(std::same_as<std::remove_cvref_t<S>, GuiEventResult>)
    {
        return std::get<0>(std::forward<S>(res).storage);
    };

    static constexpr auto right = []<class S>(S&& res) -> decltype(auto)
        requires(std::same_as<std::remove_cvref_t<S>, GuiEventResult>)
    {
        return std::get<1>(std::forward<S>(res).storage);
    };
};

} // namespace frt
```

Now `GuiEventResult` can be used with both `frt::match` and the `|` operator:

```c++
GuiEventResult event = pollGuiEvent();

std::string description = event | frt::match(
    [](const Error& err) { return "Error: " + err.reason; },
    [](const Widget& w)  { return "Widget: " + w.name; }
);
```
