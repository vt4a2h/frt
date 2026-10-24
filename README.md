# frt — Result types for modern C++

This project provides a collection of result types for modern C++. Result types are normally used as function return
types. The library also provides useful algorithms and monadic operations implemented as free functions, and supports
partial application syntax.

## Result types

This library provides the following result types:

- `frt::either<L, R>` — a tagged union that contains one of two alternatives, with no special meaning assigned to
  either.
- `frt::expected<T, E>` — a more specialized version of `frt::either`. It contains either the expected result `T` or
  an unexpected value `E`, usually denoted as an error.
- `frt::optional<T>` — represents either the presence or absence of a value.

You can find more information in the respective sections of the
documentation: [Either](doc/Either.md), [Expected](doc/Expected.md), and [Optional](doc/Optional.md).

## Monadic operations

All result types come with the standard set of monadic operations with extended functionality. For example:

```c++
struct Context;
struct Path;
struct MetaData;
struct Component;
struct Error;

frt::expected<Context, Error> extractContext();
frt::expected<Component, Error> createComponent(Context, Path, MetaData);

Path path{/* ... */};
MetaData metaData{/* ... */};

auto componentResult = extractContext()
    .and_then(&createComponent, path, metaData);
```

If `extractContext()` returns an error, `createComponent()` will not be called; otherwise, the function is invoked as
follows:

```c++
std::invoke(&createComponent, std::forward_like<Self>(data).value(), path, metaData);
```

You can find more information in the [Monadic Operations](doc/MonadicOperations.md) documentation.

All monadic operations can also be used as free functions and support partial application syntax. For example:

```c++
auto componentResult = frt::and_then(extractContext(), &createComponent, path, metaData);
```

You can find more information in the [Monadic Operations as Free Functions](doc/MonadicOperationsAsFreeFunctions.md)
documentation.

## Algorithms

### Traverse

The `frt::traverse` algorithm is handy when you need to convert a collection of results into a result of a collection
and apply an extra transformation. For example:

```c++
struct Widget {};

std::vector<frt::expected<Component, Error>> components = /* create components */;

auto asWidget = [] (const frt::expected<Component, Error> &component) -> frt::expected<Widget, Error> {
    return /* create a widget from the component */;
};
frt::expected<std::vector<Widget>, Error> result = frt::traverse(components, asWidget);
```

If the `asWidget` function returns an unexpected result, then the `result` object will contain it.

You can find more information about the traverse algorithm in the [Traverse](doc/Traverse.md) documentation.

### Sequence

The `frt::sequence` algorithm is handy when you need to convert a collection of results into a result of a collection
without applying any extra transformations. In fact, this is `frt::traverse` with `std::identity`. For example:

```c++
std::vector<frt::expected<Component, Error>> components = /* create components */;
frt::expected<std::vector<Component>, Error> result = frt::sequence(components);
```

If any of the `components` contains an error, the result will contain the first error as well.

You can find more information about the sequence algorithm in the [Sequence](doc/Sequence.md) documentation.
