# Agents configuration for the project

## Project overview

- Read README.md at the root of the repository.

## Building

- Use cmake for building.
- Available presets are located at the root of the repository in the CMakePresets.json file.
- Prefer using `*-clang` presets.
- Prefer building all targets (`--target all`).
- Add `-- -v` flag for verbose output.

## Testing

- All tests located at the `test` directory.
- Tests for each large feature are located at the respective subdirectories. For example, `test/either`.
- The test file name should follow this pattern: `tst_feature_name_functionality.cpp`. For example,
  `tst_either_create.cpp`. Where `tst_` is a prefix, `either` is a `feature_name`, `create` is a
  `functionality`, and `cpp` is a file extension.
- The `frt_add_test` cmake function is used to define a new test. For example, `frt_add_test(either_create)` should be
  added for a file with the name `tst_either_create.cpp`.
- All tests exist in both constexpr and non-constexpr versions. For example, `tst_either_create_constexpr.cpp` and
  `tst_either_create.cpp`.
- The `Catch2` framework is used for testing.
- Each `TEST_CASE` should contain a single `REQUIRE`-like instruction or from zero to many `SECTION`s with a single
  `REQUIRE`-like instruction pre each.
- The `TEST_CASE` macro has two arguments: the test name and the tag. The tag should correspond to the feature name. For
  example, `TEST_CASE("Create", "[either]")`.
- The `TEST_CASE` macro for constexpr tests contains an additional tag `[constexpr]`. For example,
  `TEST_CASE("Create", "[either][constexpr]")`.
- Constexpr tests use lambda expression for testsing:
  `constexpr auto valid = [] { /* test code here; return bool convertable expr, e.g. a == b */ }; STATIC_REQUIRE(valid);`.
  Use all `_constexpr.cpp` tests as example.
- Use long strings as test data. For example, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
  tempor incididunt ut labore et dolore magna aliqua".
- Always build and run all tests.

## Formatting

- Use clang-format for formatting.