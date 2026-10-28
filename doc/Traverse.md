# Traverse

`frt::traverse` and `frt::traverse_into` take an input range of result objects, apply a transformation function `f` to
each element in order, and collect the unwrapped right (value) alternatives into a single container wrapped in the
result type. If any invocation of `f` produces a left (error or empty) alternative, traversal short-circuits
immediately and returns that failure.

Because `traverse` operates on a **range** of result objects rather than a single instance, it is **not implemented as a
member function** on [`frt::either`](Either.md), [`frt::expected`](Expected.md), or [`frt::optional`](Optional.md).
Instead, it is provided as a free function in `<frt/algorithm/traverse.hpp>` supporting both regular call syntax and
pipe (`|`) syntax.

> [!TIP]
> If you only need to convert a range of results into a result of a container without applying an extra transformation
> (`f` is `std::identity{}`), use [`frt::sequence` / `frt::sequence_into`](Sequence.md).

## Table of contents

- [Signatures and Invocation Syntaxes](#signatures-and-invocation-syntaxes)
- [Examples](#examples)
    - [1. Basic traversal (`frt::traverse`)](#1-basic-traversal-frttraverse)
    - [2. Traversing `void` result elements](#2-traversing-void-result-elements)
    - [3. Customizing the output container (`frt::traverse_into`)](#3-customizing-the-output-container-frttraverse_into)
        - [Sequential containers (`std::list`, `std::set`, etc.)](#sequential-containers-stdlist-stdset-etc)
        - [Associative key-value containers (`std::map`,
          `std::unordered_map`)](#associative-key-value-containers-stdmap-stdunordered_map)
- [Customization](#customization)
    - [1. Customizing result types (`frt::meta_either`)](#1-customizing-result-types-frtmeta_either)
    - [2. Customizing output containers (`frt::meta_container`)](#2-customizing-output-containers-frtmeta_container)

---

## Signatures and Invocation Syntaxes

```c++
#include <frt/algorithm/traverse.hpp>
```

| Algorithm                       | Output container | Direct call syntax                        | Pipe (`\|`) syntax                          |
|---------------------------------|------------------|-------------------------------------------|---------------------------------------------|
| `frt::traverse`                 | `std::vector`    | `frt::traverse(range, f)`                 | `range \| frt::traverse(f)`                 |
| `frt::traverse_into<Container>` | `Container`      | `frt::traverse_into<Container>(range, f)` | `range \| frt::traverse_into<Container>(f)` |

- **`range`**: Any `std::ranges::input_range` whose value type satisfies `frt::IsResultType`.
- **`f`**: A callable invoked with each element of `range` (`Result` object) and returning a move-constructible result
  type whose right alternative is the element type to collect.

> [!NOTE]
> `frt::traverse` and `frt::traverse_into` are **eager** algorithms: they evaluate immediately and return a container
> wrapped in a result type, not a lazy range view.

---

## Examples

### 1. Basic traversal (`frt::traverse`)

Suppose you have a collection of `frt::expected<Component, Error>` results and want to convert each into a `Widget`,
producing a single `frt::expected<std::vector<Widget>, Error>`:

```c++
#include <frt/algorithm/traverse.hpp>
#include <frt/expected.hpp>
#include <vector>

struct Component;
struct Widget;
struct Error;

using ComponentResult = frt::expected<Component, Error>;
using WidgetResult    = frt::expected<Widget, Error>;

WidgetResult toWidget(const ComponentResult& componentResult);

std::vector<ComponentResult> components = loadComponents(/* ... */);

// Regular free-function syntax
frt::expected<std::vector<Widget>, Error> widgets1 =
    frt::traverse(components, &toWidget);

// Pipe (`|`) syntax
frt::expected<std::vector<Widget>, Error> widgets2 =
    components | frt::traverse(&toWidget);
```

- **When all elements succeed**: `widgets1` and `widgets2` contain a `std::vector<Widget>` with all transformed widgets
  in order.
- **When any element fails**: As soon as `toWidget` returns an unexpected `Error`, `frt::traverse` stops iterating and
  returns `frt::expected<std::vector<Widget>, Error>` holding that first `Error`.

### 2. Traversing `void` result elements

While the return type of `f` must have a non-`void` right alternative (so its values can be stored in the output
container), the **input** range can contain `void` result types such as `frt::expected<void, Error>`:

```c++
using InitStatus = frt::expected<void, Error>;

std::vector<InitStatus> statuses = initializeSubsystems(/* ... */);

// Convert each successful status into a Widget, or short-circuit on the first Error
frt::expected<std::vector<Widget>, Error> widgets =
    statuses | frt::traverse([](const InitStatus& status) -> frt::expected<Widget, Error> {
        return status.transform([]() { return Widget{/* ... */}; });
    });
```

### 3. Customizing the output container (`frt::traverse_into`)

By default, `frt::traverse` collects elements into a `std::vector`. Use `frt::traverse_into<OutputContainer>` to collect
into any sequential or associative container:

#### Sequential containers (`std::list`, `std::set`, etc.)

```c++
#include <list>
#include <set>

// Collect into a std::list<Widget>
frt::expected<std::list<Widget>, Error> widgetList =
    components | frt::traverse_into<std::list>(&toWidget);

// Collect into a std::set<Widget> (requires Widget to be ordered)
frt::expected<std::set<Widget>, Error> widgetSet =
    components | frt::traverse_into<std::set>(&toWidget);
```

#### Associative key-value containers (`std::map`, `std::unordered_map`)

When `OutputContainer` is a key-value associative container such as `std::map` or `std::unordered_map`, the right
alternative of `f`'s return type must be a pair-like type (`std::pair<Key, Value>`) providing `first_type` and
`second_type`:

```c++
#include <map>
#include <string>
#include <utility>

using NamedComponent = frt::optional<Component>;
using NamedWidget    = frt::optional<std::pair<std::string, Widget>>;

NamedWidget toNamedWidget(const NamedComponent& comp);

std::vector<NamedComponent> components = findComponents(/* ... */);

// Collects key-value pairs into frt::optional<std::map<std::string, Widget>>
frt::optional<std::map<std::string, Widget>> widgetRegistry =
    components | frt::traverse_into<std::map>(&toNamedWidget);
```

---

## Customization

`frt::traverse` and `frt::traverse_into` can be customized along two orthogonal axes:

1. **Result types** via `frt::is_result_type` and `frt::meta_either`.
2. **Output container types** via `frt::meta_container`.

### 1. Customizing result types (`frt::meta_either`)

`frt::traverse` works out of the box with `frt::either`, `frt::expected`, `frt::optional`, as well as `std::expected`
and `std::optional` (via `<frt/meta/std/meta_either_std_expected.hpp>` and
`<frt/meta/std/meta_either_std_optional.hpp>`).

To use `frt::traverse` with a custom result type:

1. Both the input range's element type and the callable's return type `Ret` must satisfy `frt::IsResultType`
   (`frt::is_result_type<T> = true`).
2. `Ret` must be move-constructible, and neither `meta_either<Ret>::left_t` nor `meta_either<Ret>::right_t` may be
   `void` (use `frt::void_t` if the left alternative carries no value, as in `optional`).
3. `frt::meta_either<Ret>` must provide:
    - `left_t` and `right_t`
    - `rebind_right<U>` (used to form the output result type `Out = rebind_right<Container>`)
    - `is_right(res)`
    - `right(std::move(res))`
    - `left(std::move(res))`
4. `frt::meta_either<Out>` for the rebound output type `Out` must provide:
    - `make_left(left)`
    - `make_right(container)`

See [Monadic Operations as Free Functions](MonadicOperationsAsFreeFunctions.md#customizing-monadic-operations-with-frtmeta_either)
for a full walkthrough of `frt::meta_either`.

### 2. Customizing output containers (`frt::meta_container`)

Out of the box, the default `frt::meta_container<C>` implementation in `<frt/meta/meta_container.hpp>` supports any
move-constructible container `C` that provides:

- `c.emplace_back(elem)` or `c.push_back(elem)` (prefers `emplace_back` for sequential containers), or
- `c.emplace(elem)` or `c.insert(elem)` (prefers `emplace` for associative/set containers).

In addition, if `C` is a `std::ranges::sized_range` and provides `c.reserve(n)`, `frt::traverse` automatically
pre-allocates capacity for `std::ranges::size(inputRange)` elements before iterating.

If you want to collect into a container with a different insertion API (such as `std::forward_list`, which only provides
`push_front`), specialize `frt::meta_container<C>`:

```c++
#include <forward_list>
#include <frt/algorithm/traverse.hpp>
#include <frt/either.hpp>

namespace frt {

template <class Elem>
struct meta_container<std::forward_list<Elem>> {
    // Required: insert an element into the container
    static constexpr auto add_element = [](std::forward_list<Elem>& container, Elem&& elem) {
        container.push_front(std::move(elem));
    };

    // Optional: pre-allocate space for `n` elements if supported by the container
    // static constexpr auto reserve = [](Container& container, std::size_t n) { ... };
};

} // namespace frt
```

With this specialization in place, `std::forward_list` works seamlessly with `frt::traverse_into`:

```c++
auto widgetForwardList = components
    | frt::traverse_into<std::forward_list>(&toWidget);
```
