# Agents configuration for the project

## Project overview

- Read README.md at the root of the repository.

## Building

- Use cmake for building.
- Available presets are located at the root of the repository in the CMakePresets.json file.
- Prefer using `host-clang` preset.
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
- Instead of having two or more `REQUIRE`-like instructions in a single `SECTION`, consider splitting the test into
  multiple `SECTION`s. For example, one `SECTION` for `REQUIRE(result.is_a())` and another `SECTION` for
  `REQUIRE(result.get_a() == expected_a)`.
- Test negative scenarios as well.
- The `TEST_CASE` macro has two arguments: the test name and the tag. The tag should correspond to the feature name. For
  example, `TEST_CASE("Create", "[either]")`.
- The `TEST_CASE` macro for constexpr tests contains an additional tag `[constexpr]`. For example,
  `TEST_CASE("Create", "[either][constexpr]")`.
- Constexpr tests use lambda expression for testsing:
  `constexpr auto valid = [] { /* test code here; return bool convertable expr, e.g. a == b */ }; STATIC_REQUIRE(valid);`.
  Use all `_constexpr.cpp` tests as example.
- Constexpr tests must cover all the same scenarios as non-constexpr tests. The number of test cases must be the same.
  The number of sections must be the same. Ignore this rule if this is technically impossible due to the language
  limitations or similar.
- More complicated types like `std::vector` or `std::string` must be used for constexpr tests as well.
- Common functions and variables used by more than one `TEST_CASE` in scope of the same file can be placed above the
  first `TEST_CASE`.
- The default values, expected values, and similar should be placed as close as possible to the place when they are
  used.
- Avoid duplication among the default values, expected values, and similar.
- Don't use magic numbers.
- Don't use magic string literals
- Double-check for unused statements (e.g. `using namespace`).
- Double-check for unused variables.
- Double-check for unused headers.
- Use long strings as test data. For example, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
  tempor incididunt ut labore et dolore magna aliqua".
- Always build and run all tests.

## Formatting

- Use clang-format for formatting.