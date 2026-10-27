# Sequence

`frt::sequence` and `frt::sequence_into` convert a range of result objects into a single result object containing a
container of the unwrapped values (for example, turning `std::vector<frt::expected<Widget, Error>>` into
`frt::expected<std::vector<Widget>, Error>`).

Effectively, `frt::sequence` is a shortcut for [`frt::traverse`](Traverse.md) with `std::identity{}` as the
transformation function:

- If **every** element in the input range holds the right (expected/present) alternative, `sequence` returns a result
  object holding a container of all unwrapped values in order.
- If **any** element holds the left (error/empty) alternative, `sequence` short-circuits at the first failure and
  returns a result object containing that failure.

Because `sequence` operates on a **range** of result objects rather than a single result instance, it is **not
implemented as a member function** on [`frt::either`](Either.md), [`frt::expected`](Expected.md), or
[`frt::optional`](Optional.md). Instead, it is provided in `<frt/algorithm/sequence.hpp>` as a free function object
(inheriting from `std::ranges::range_adaptor_closure`) that supports both regular function call syntax and pipe (`|`)
syntax.

---

## Signatures and Invocation Syntaxes

```c++
#include <frt/algorithm/sequence.hpp>
```

| Algorithm                       | Output container | Direct call syntax                     | Pipe (`\|`) syntax                       |
|---------------------------------|------------------|----------------------------------------|------------------------------------------|
| `frt::sequence`                 | `std::vector`    | `frt::sequence(range)`                 | `range \| frt::sequence`                 |
| `frt::sequence_into<Container>` | `Container`      | `frt::sequence_into<Container>(range)` | `range \| frt::sequence_into<Container>` |

- **`range`**: Any `std::ranges::input_range` whose elements are move-constructible result types (`frt::IsResultType`)
  with a non-`void` right alternative.

> [!NOTE]
> Because `frt::sequence` and `frt::sequence_into<Container>` derive from `std::ranges::range_adaptor_closure` and take
> no extra arguments, they are piped directly without parentheses (`range | frt::sequence` and
> `range | frt::sequence_into<std::list>`) and compose naturally with `std::views` pipelines.

---

## Examples

### 1. Basic usage (`frt::sequence`)

Suppose you load several GUI `Component` instances where each load operation returns
`frt::expected<Component, Error>`, and you want either all loaded components in a `std::vector<Component>` or the first
`Error` that occurred:

```c++
#include <frt/algorithm/sequence.hpp>
#include <frt/expected.hpp>
#include <vector>

struct Component;
struct Error;

using ComponentResult = frt::expected<Component, Error>;

std::vector<ComponentResult> results = loadAllComponents(/* ... */);

// 1. Regular free-function call
frt::expected<std::vector<Component>, Error> allComponents1 =
    frt::sequence(results);

// 2. Pipe (`|`) syntax
frt::expected<std::vector<Component>, Error> allComponents2 =
    results | frt::sequence;
```

### 2. Composing with `std::views`

Because `frt::sequence` is a `std::ranges::range_adaptor_closure`, you can pipe a lazy `std::views` range directly into
`frt::sequence` to evaluate and collect the results in a single expression:

```c++
#include <ranges>
#include <vector>

struct Path;
struct Window;
struct Error;

frt::either<Error, Window> loadWindow(const Path& path);

std::vector<Path> paths = getWindowPaths(/* ... */);

// Lazily map each Path to frt::either<Error, Window>, then sequence into frt::either<Error, std::vector<Window>>
frt::either<Error, std::vector<Window>> windowsOrError = paths
    | std::views::transform(&loadWindow)
    | frt::sequence;
```

If `loadWindow` returns `Error` for any `Path`, `frt::sequence` stops iterating immediately and `windowsOrError` holds
that first `Error` without calling `loadWindow` on the remaining paths.

### 3. Customizing the output container (`frt::sequence_into`)

Use `frt::sequence_into<OutputContainer>` to collect the unwrapped values into a container other than `std::vector`,
such as `std::list`, `std::set`, or `std::map`:

```c++
#include <list>
#include <map>
#include <string>
#include <utility>

struct Widget;

// Collect into a std::list<Widget>
std::vector<frt::optional<Widget>> optWidgets = findWidgets(/* ... */);

frt::optional<std::list<Widget>> widgetList1 = frt::sequence_into<std::list>(optWidgets);
frt::optional<std::list<Widget>> widgetList2 = optWidgets | frt::sequence_into<std::list>;

// Collect key-value pairs into a std::map<std::string, Widget>
using NamedWidgetResult = frt::expected<std::pair<std::string, Widget>, Error>;
std::vector<NamedWidgetResult> namedWidgets = loadNamedWidgets(/* ... */);

frt::expected<std::map<std::string, Widget>, Error> widgetMap =
    namedWidgets | frt::sequence_into<std::map>;
```

---

## Customization

`frt::sequence(range)` and `frt::sequence_into<OutputContainer>(range)` are implemented directly as
`frt::traverse(range, std::identity{})` and `frt::traverse_into<OutputContainer>(range, std::identity{})`.

Consequently, they support the exact same customization points:

- **Custom result types**: Specialize `frt::is_result_type<R> = true` and `frt::meta_either<R>` (works out of the box
  with `frt::either`, `frt::expected`, `frt::optional`, `std::expected`, and `std::optional`).
- **Custom output containers**: Specialize `frt::meta_container<C>` if the target container does not provide
  `emplace_back`, `push_back`, `emplace`, or `insert`.

See the [Traverse documentation](Traverse.md#customization) for complete details and examples of `frt::meta_either` and
`frt::meta_container` specializations.
