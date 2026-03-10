# frt

This project represents a collection of result types for modern C++. Result types are normally used as function return
types.

`frt::either<L, R>` -- is a tagged union that contains one of two alternatives with no special meaning for each of them.
`frt::expected<E, U>` -- is a more specialized version of `frt::either`. It contains either the expected result `E` or
the unexpected `U`, usually denoted as an error.
`frt::optional<T>` -- represents either the presence or absence of a value.

All interfaces are UB-free. The `frt::expects` is used to ensure the correct invariant. C++ contracts are used when
supported by the compiler.

All APIs support monadic operations.

All functionality is available at compile time. I.e., the library is fully constexpr.
