# Monadic Operations

Monadic operations provide a declarative, composable, and **safe alternative to regular `if-else` branching** when
working with sum and result types ([`frt::either`](Either.md), [`frt::expected`](Expected.md), and
[`frt::optional`](Optional.md)).

When handling multi-step computations with manual `if-else` statements, code quickly becomes deeply nested and
error-prone:

- It is easy to accidentally access an inactive alternative (e.g., calling `.value()` or `.right()` on the wrong
  branch).
- Every intermediate step requires boilerplate state checks and manual error or fallback propagation.
- Preserving value categories (remembering `std::move` when forwarding temporaries) must be managed manually at each
  step.

Monadic operations eliminate these hazards by encapsulating the state check and value extraction: a callable is invoked
**only** when the target alternative is active, and the inactive alternative is automatically propagated unchanged.

```c++
struct Context;
struct Component;
struct Widget;
struct Error;

frt::expected<Context, Error> extractContext();
frt::expected<Component, Error> createComponent(Context);
Widget toWidget(Component);
frt::expected<Widget, Error> fallbackWidget(Error);
```

<table>
<thead>
<tr>
<th>With <code>if-else</code> branching</th>
<th>With monadic operations</th>
</tr>
</thead>
<tbody>
<tr>
<td>

```c++
frt::expected<Widget, Error> buildWidget() {
    auto ctxResult = extractContext();
    if (ctxResult.has_value()) {
        auto compResult = createComponent(
            std::move(ctxResult).value());
        if (compResult.has_value()) {
            return toWidget(
                std::move(compResult).value());
        } else {
            return fallbackWidget(
                std::move(compResult).error());
        }
    } else {
        return fallbackWidget(
            std::move(ctxResult).error());
    }
}
```

</td>
<td>

```c++
frt::expected<Widget, Error> buildWidget() {
    return extractContext()
        .and_then(&createComponent)
        .transform(&toWidget)
        .or_else(&fallbackWidget);
}
```

</td>
</tr>
</tbody>
</table>

In addition to being available as member functions on `frt::either`, `frt::expected`, and `frt::optional`, **monadic
operations can be customized for other sum types** (including `std::expected`, `std::optional`, and user-defined result
types) by specializing `frt::meta_either` and `frt::is_result_type`. See
[Monadic Operations as Free Functions](MonadicOperationsAsFreeFunctions.md) for details on free-function invocation,
pipeline syntax (`|`), and custom type integration.

---

## Summary of Operations

The table below lists all monadic operations provided by [`frt::either<L, R>`](Either.md),
[`frt::expected<T, E>`](Expected.md), and [`frt::optional<T>`](Optional.md), along with their free-function counterparts
and functional programming equivalents. The types `U` and `G` indicate that an operation can change the corresponding
alternative type.

| Type                  | Member function   | Signature of `f`           | Also known as            |
|-----------------------|-------------------|----------------------------|--------------------------|
| `frt::either<L, R>`   | `and_then_left`   | `L -> frt::either<U, R>`   | `orElse` / `recoverWith` |
| `frt::either<L, R>`   | `and_then_right`  | `R -> frt::either<L, U>`   | `bind` / `flatMap`       |
| `frt::either<L, R>`   | `transform_left`  | `L -> U`                   | `leftMap`                |
| `frt::either<L, R>`   | `transform_right` | `R -> U`                   | `map` / `fmap`           |
|                       |                   |                            |                          |
| `frt::expected<T, E>` | `and_then`        | `T -> frt::expected<U, E>` | `bind` / `flatMap`       |
| `frt::expected<T, E>` | `or_else`         | `E -> frt::expected<T, G>` | `orElse` / `recoverWith` |
| `frt::expected<T, E>` | `transform`       | `T -> U`                   | `map` / `fmap`           |
| `frt::expected<T, E>` | `transform_error` | `E -> G`                   | `leftMap` / `mapError`   |
|                       |                   |                            |                          |
| `frt::optional<T>`    | `and_then`        | `T -> frt::optional<U>`    | `bind` / `flatMap`       |
| `frt::optional<T>`    | `or_else`         | `() -> frt::optional<T>`   | `orElse` / `recoverWith` |
| `frt::optional<T>`    | `transform`       | `T -> U` (non-`void`)      | `map` / `fmap`           |

### Semantics

- **Chaining (`and_then`, `and_then_right`, `and_then_left`, `or_else`)**:
  Used when the next step in a computation itself returns a result type (`either`, `expected`, or `optional`). If the
  target alternative is active, the callable `f` is invoked and its result is returned directly without double-wrapping.
  Otherwise, `f` is not called and the inactive alternative is forwarded into the return type.
- **Mapping (`transform`, `transform_right`, `transform_left`, `transform_error`)**:
  Used when a function transforms the contained value into a plain value `U` (or `void` where supported). If the target
  alternative is active, `f` is invoked and its return value is automatically wrapped into the corresponding alternative
  of the result type. Otherwise, `f` is not called and the inactive alternative is forwarded unchanged.

---

## Inline Partial Application

In standard C++ (`std::expected` and `std::optional`), monadic operations only accept unary callables (or nullary
callables for `void` / `optional::or_else`). Whenever a step in a pipeline requires additional variables from the
enclosing scope, or invokes a member function on an existing object, you are forced to write a wrapper lambda or use
`std::bind_front` / `std::bind_back`:

- **Lambda boilerplate**: Explicit capture lists, parameter type declarations, and manual `std::move` calls add visual
  noise and can conflict with project style rules (such as prohibiting implicit `[&]` captures or single-line lambdas).
- **Value category pitfalls**: Forgetting `std::move` inside a wrapper lambda silently copies movable values.
- **`std::bind_front` / `std::bind_back` overhead**: Creates intermediate wrapper objects, requires move-constructible
  decayed arguments, and evaluates argument copies/moves eagerly even when the branch is inactive.

The `frt` library solves this by supporting **Inline Partial Application (IPA)** across all monadic operations. Every
monadic operation accepts an optional pack of extra arguments (`Args&&... args`) that are forwarded directly to the
callable only when the target branch is active, with **zero overhead** and **no intermediate objects or tuples**.

Below, we illustrate how inline partial application works using `frt::either::and_then_right`, which has the signature:

```c++
template <class Self, class F, class... Args>
[[nodiscard]] constexpr auto and_then_right(this Self&& self, F&& f, Args&&... args);
```

> [!NOTE]
> Everything shown for `either::and_then_right` applies identically to `and_then_left`, `transform_left`, and
> `transform_right` on `frt::either`, as well as `and_then`, `or_else`, `transform`, and `transform_error` on
> `frt::expected` and `frt::optional`.

---

### 1. Single-argument invocations

#### Passing only the contained value (no extra arguments)

When no extra arguments are supplied, `and_then_right` passes the contained right alternative as the single argument to
the callable, preserving the value category and constness of the `either` object:

```c++
struct Error;
struct Context;
struct Window;

frt::either<Error, Context> extractContext();
frt::either<Error, Window> createWindow(Context);

auto windowOrError = extractContext()
    .and_then_right(&createWindow);
```

If `extractContext()` holds the right alternative (`Context`), `createWindow` is invoked as:

```c++
std::invoke(&createWindow, std::forward_like<Self>(either).right());
```

#### Passing a single extra argument when the alternative is `void`

When the right alternative of `frt::either<L, void>` is `void`, there is no contained value to unwrap. Supplying a
single extra argument invokes a unary callable with that forwarded argument:

```c++
frt::either<Error, void> initializeGui();
frt::either<Error, Window> createMainWindow(Context&);

Context context{/* ... */};

auto windowOrError = initializeGui()
    .and_then_right(&createMainWindow, context);
```

When `initializeGui()` holds the right (`void`) alternative, `createMainWindow` is invoked with `context` as its sole
argument:

```c++
std::invoke(&createMainWindow, context);
```

#### Passing a single extra argument alongside the contained value

When the right alternative is non-`void` and a single extra argument is passed, the callable receives two arguments:
the contained right value first, followed by the extra argument:

```c++
struct Layout;

frt::either<Error, Window> applyLayout(Window, const Layout&);

Layout layout{/* ... */};

auto configuredWindow = extractContext()
    .and_then_right(&createWindow)
    .and_then_right(&applyLayout, layout);
```

Here, `applyLayout` is invoked as:

```c++
std::invoke(&applyLayout, std::forward_like<Self>(either).right(), layout);
```

---

### 2. Multiple extra arguments (contained value first)

By default, when you supply extra arguments `args...` without a placeholder, the contained value is passed as the
**first** argument to the callable, followed by `args...` in order.

Suppose `createComponent` requires a `Context` (produced by the previous step) along with a `Path` and `MetaData` from
the outer scope:

```c++
struct Path;
struct MetaData;
struct Component;

frt::either<Error, Context> extractContext();
frt::either<Error, Component> createComponent(Context, Path, MetaData);
```

With inline partial application, `path` and `metaData` are passed directly to `and_then_right`:

<table>
<thead>
<tr>
<th>With a lambda</th>
<th>With inline partial application</th>
</tr>
</thead>
<tbody>
<tr>
<td>

```c++
Path path{/* ... */};
MetaData metaData{/* ... */};

auto result = extractContext()
    .and_then_right([&path, &metaData](Context&& ctx) {
        return createComponent(
            std::move(ctx), path, metaData);
    });
```

</td>
<td>

```c++
Path path{/* ... */};
MetaData metaData{/* ... */};

auto result = extractContext()
    .and_then_right(&createComponent, path, metaData);
```

</td>
</tr>
</tbody>
</table>

When the right alternative is active, `createComponent` is invoked as:

```c++
std::invoke(&createComponent, std::forward_like<Self>(either).right(), path, metaData);
```

---

### 3. Controlling the contained value position with `frt::unwrapped`

Often an existing function does not accept the unwrapped value as its first parameter. To support arbitrary parameter
orders without writing adapter lambdas, `frt` provides the positional placeholder `frt::unwrapped` (of type
`frt::unwrapped_t`).

When `frt::unwrapped` is present in the extra argument list, `frt::unwrapped` is replaced with the contained alternative
in that exact position, while preserving its value category and constness.

#### Passing the contained value in the middle of the argument list

Suppose `createComponent` takes `Context` as its **middle** parameter, between `Path` and `MetaData`:

```c++
frt::either<Error, Component> createComponent(Path, Context, MetaData);
```

You can place `frt::unwrapped` in the middle of the trailing arguments to match the target signature:

<table>
<thead>
<tr>
<th>With a lambda</th>
<th>With <code>frt::unwrapped</code> in the middle</th>
</tr>
</thead>
<tbody>
<tr>
<td>

```c++
Path path{/* ... */};
MetaData metaData{/* ... */};

auto result = extractContext()
    .and_then_right([&path, &metaData](Context&& ctx) {
        return createComponent(
            path, std::move(ctx), metaData);
    });
```

</td>
<td>

```c++
Path path{/* ... */};
MetaData metaData{/* ... */};

auto result = extractContext()
    .and_then_right(&createComponent,
                    path, frt::unwrapped, metaData);
```

</td>
</tr>
</tbody>
</table>

When `extractContext()` returns a `Context`, `createComponent` is invoked with the unwrapped `Context` in the second
position:

```c++
std::invoke(&createComponent, path, std::forward_like<Self>(either).right(), metaData);
```

#### Passing the contained value as the last argument

Similarly, if `Context` is the **last** parameter of `createComponent`:

```c++
frt::either<Error, Component> createComponent(Path, MetaData, Context);

Path path{/* ... */};
MetaData metaData{/* ... */};

auto result = extractContext()
    .and_then_right(&createComponent, path, metaData, frt::unwrapped);
```

This invokes `createComponent` as:

```c++
std::invoke(&createComponent, path, metaData, std::forward_like<Self>(either).right());
```

> [!IMPORTANT]
> When only a single extra argument is provided (`sizeof...(Args) == 1`), that argument **must not** be
> `frt::unwrapped`. Passing `frt::unwrapped` as the sole extra argument is redundant because the contained value is
> already passed as the sole argument when no extra arguments are specified.

---

### 4. Invoking member functions

In C++, invoking a pointer to a member function via `std::invoke(&Class::method, obj, args...)` requires the target
object (or pointer/reference to it, such as `this`) as the first argument, followed by the method's parameters.

`frt::unwrapped` makes calling member functions on existing GUI objects seamless, whether the member function takes a
single parameter or multiple parameters with the unwrapped value in the middle or at the end:

```c++
struct WindowManager {
    // Takes a single parameter: the unwrapped Component
    frt::either<Error, Window> attach(Component component);

    // Takes multiple parameters, with the unwrapped Window in the middle
    frt::either<Error, Widget> embed(Layout& layout, Window window, MetaData metaData);
};
```

<table>
<thead>
<tr>
<th>With a lambda</th>
<th>With inline partial application</th>
</tr>
</thead>
<tbody>
<tr>
<td>

```c++
WindowManager manager{/* ... */};
Layout layout{/* ... */};
Path path{/* ... */};
MetaData metaData{/* ... */};

auto widgetOrError = extractContext()
    .and_then_right([&](Context&& ctx) {
        return createComponent(
            path, std::move(ctx), metaData);
    })
    .and_then_right([&](Component&& comp) {
        return manager.attach(std::move(comp));
    })
    .and_then_right([&](Window&& win) {
        return manager.embed(
            layout, std::move(win), metaData);
    });
```

</td>
<td>

```c++
WindowManager manager{/* ... */};
Layout layout{/* ... */};
Path path{/* ... */};
MetaData metaData{/* ... */};

auto widgetOrError = extractContext()
    .and_then_right(&createComponent,
                    path, frt::unwrapped, metaData)
    .and_then_right(&WindowManager::attach,
                    manager, frt::unwrapped)
    .and_then_right(&WindowManager::embed,
                    manager, layout, frt::unwrapped, metaData);
```

</td>
</tr>
</tbody>
</table>

The two member function steps expand directly to:

```c++
std::invoke(&WindowManager::attach, manager, std::forward_like<Self>(compEither).right());
std::invoke(&WindowManager::embed, manager, layout, std::forward_like<Self>(winEither).right(), metaData);
```

---

## Argument Forwarding Rules

For any monadic operation invoked with a callable `f` and trailing arguments `args...`:

1. **When the active alternative has a value (non-`void`)**:
    - **No extra arguments (`sizeof...(Args) == 0`)**: `f` is invoked with the forwarded alternative value as its sole
      argument: `std::invoke(f, value)`.
    - **Extra arguments without `frt::unwrapped`**: `f` is invoked with the forwarded alternative value first, followed
      by `args...`: `std::invoke(f, value, args...)`.
    - **Extra arguments with `frt::unwrapped`**: `f` is invoked with `args...` where the single `frt::unwrapped`
      placeholder is replaced in-place by the forwarded alternative value.
    - **Constraints on `frt::unwrapped`**:
        - `args...` may contain at most **one** `frt::unwrapped` placeholder.
        - If `sizeof...(Args) == 1`, that single extra argument **must not** be `frt::unwrapped`.

2. **When the active alternative has no value (`void` in `either`/`expected`, or `nullopt` in `optional::or_else`)**:
    - `f` is invoked with `args...` directly: `std::invoke(f, args...)`.
    - `args...` **must not** contain `frt::unwrapped`.
