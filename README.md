# frt — Result types for modern C++

This project provides a collection of result types for modern C++. Result types are normally used as function return
types. The library also provides useful algorithms and monadic operations implemented as free functions, and supports
partial application syntax. Both algorithms and monadic operations can be used with custom result-like types by
specialising `frt::meta_either` for these types.

## Table of contents

- [Result types](#result-types)
- [Monadic operations](#monadic-operations)
- [Algorithms](#algorithms)
  - [Traverse](#traverse)
  - [Sequence](#sequence)
  - [Match](#match)
- [Configuration options](#configuration-options)

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
std::invoke(&createComponent, std::forward_like<Self>(data).value(), std::forward<P>(path), std::forward<MD>(metaData));
```

You can find more information in the [Monadic Operations](doc/MonadicOperations.md) documentation.

All monadic operations can also be used as free functions and support partial application syntax. For example:

```c++
auto componentResult = frt::and_then(extractContext(), &createComponent, path, metaData);

// or

auto anotherComponentResult = extractContext() | frt::and_then(&createComponent, path, metaData);
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

// This form is also supported
auto anotherResult = components | frt::traverse(asWidget);
```

If the `asWidget` function returns an unexpected result, then the `result` object will contain it.

You can find more information about the traverse algorithm in the [Traverse](doc/Traverse.md) documentation.

### Sequence

The `frt::sequence` algorithm is handy when you need to convert a collection of results into a result of a collection
without applying any extra transformations. In fact, this is `frt::traverse` with `std::identity`. For example:

```c++
std::vector<frt::expected<Component, Error>> components = /* create components */;
frt::expected<std::vector<Component>, Error> result = frt::sequence(components);

// This form is also supported
auto anotherResult = components | frt::sequence();
```

If any of the `components` contains an error, the result will contain the first error as well.

You can find more information about the sequence algorithm in the [Sequence](doc/Sequence.md) documentation.

### Match

The `frt::match` implements pattern matching for the result types.

```c++
const auto componentResult = extractContext()
    .and_then(&createComponent, path, metaData);

frt::match(componentResult, 
    [] (const Component &component) {
        // Do something with the component
        // You can mutate the component in-place if the type is `Component &`
        // You can return something from this function
    },
    [] (const Error &error) {
        // Do something with the error
        // You can mutate the error in-place if the type is `Error &`
        // You can return something from this function
    });

frt::match(componentResult, [](const auto &value) {
    // Do something with the contained value regardless of its type, for example:
    std::println("Value: {}", value);
    
    // You can mutate the value in-place if the type is `auto &`
    // You can return something from this function
});

// This form is also supported
componentResult | frt::match( /* ... */ );
```

You can find more information in the [Match](doc/Match.md) documentation.

## Configuration options

The `frt` library provides several configuration options that can be set during project configuration. These options
allow you to customize the behavior and build of the library to suit your needs.

Options can be configured via the CMake command line:

```sh
cmake -B build -DFRT_OPTIMIZE_EXECUTION_PATH=OFF
```

or set in your `CMakeLists.txt`:

```cmake
target_link_libraries(${my_target}
    PUBLIC
    frt::frt
    # ...
)

target_compile_definitions(${my_target}
    PUBLIC
    FRT_OPTIMIZE_EXECUTION_PATH=OFF
    # ...
)
```

| Option                      | Default value | Description                                                                                                                          |
|-----------------------------|---------------|--------------------------------------------------------------------------------------------------------------------------------------|
| FRT_DEV_MODE                | OFF           | Enable strict compiler checks for the library source code (e.g., `-Wpedantic`).                                                      |
| FRT_DEPENDENCIES_FROM_CPM   | OFF           | Use CPM to fetch third-party dependencies (e.g., Catch2 for testing).                                                                |
| FRT_ENABLE_TESTING          | OFF           | Configure and build the test suite.                                                                                                  |
| FRT_ENABLE_BENCHMARKING     | OFF           | Configure and build the benchmark suite.                                                                                             |
| FRT_HARDENED                | ON            | Contract violations terminate the program. Disabling this option may cause undefined behavior (UB), as invariant checks are ignored. |
| FRT_OPTIMIZE_EXECUTION_PATH | ON            | Use `[[likely]]` and `[[unlikely]]` attributes to allow the compiler to optimize execution paths.                                    |
