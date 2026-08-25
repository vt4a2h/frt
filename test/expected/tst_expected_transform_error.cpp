#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/expected.hpp>

using Expected = frt::expected<std::string, std::vector<int>>;
using ExpectedVoid = frt::expected<void, std::vector<int>>;

inline static const std::string expectedValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline static const std::vector expectedError{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::vector expectedTransformedError{1.0, 2.0, 3.0, 4.0, 5.0};
inline static constexpr int extraArg = 42;

TEST_CASE("transform_error", "[expected]")
{
  auto transform = [](const std::vector<int> &) -> std::vector<double> { return expectedTransformedError; };
  auto transformExtra = [](const std::vector<int> &, int) -> std::vector<double> { return expectedTransformedError; };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::vector<double> {
    return expectedTransformedError;
  };

  const Expected val{expectedValue};
  const Expected err{frt::unexpected{expectedError}};

  SECTION("error to different error type without extra args (!has_value)")
  {
    const auto res = err.transform_error(transform);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error to different error type without extra args (error)")
  {
    const auto res = err.transform_error(transform);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error to different error type with extra args without unwrapped (!has_value)")
  {
    const auto res = err.transform_error(transformExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error to different error type with extra args without unwrapped (error)")
  {
    const auto res = err.transform_error(transformExtra, extraArg);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error to different error type with unwrapped in extra args (!has_value)")
  {
    const auto res = err.transform_error(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error to different error type with unwrapped in extra args (error)")
  {
    const auto res = err.transform_error(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error with non-const lvalue forwarding")
  {
    Expected expected{frt::unexpected{expectedError}};
    auto fn = [](std::vector<int> &v) -> bool {
      v.clear();
      return true;
    };
    [[maybe_unused]] const auto res = expected.transform_error(fn);
    REQUIRE(expected.error().empty());
  }

  SECTION("error with const lvalue forwarding")
  {
    const Expected expected{frt::unexpected{expectedError}};
    auto fn = [](const std::vector<int> &v) -> bool { return v == expectedError; };
    const auto res = expected.transform_error(fn);
    REQUIRE(res.error());
  }

  SECTION("error with rvalue forwarding")
  {
    Expected expected{frt::unexpected{expectedError}};
    auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
    const auto res = std::move(expected).transform_error(fn);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("value without extra args (has_value)")
  {
    const auto res = val.transform_error(transform);
    REQUIRE(res.has_value());
  }

  SECTION("value without extra args (value)")
  {
    const auto res = val.transform_error(transform);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with extra args without unwrapped (has_value)")
  {
    const auto res = val.transform_error(transformExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("value with extra args without unwrapped (value)")
  {
    const auto res = val.transform_error(transformExtra, extraArg);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with unwrapped in extra args (has_value)")
  {
    const auto res = val.transform_error(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.has_value());
  }

  SECTION("value with unwrapped in extra args (value)")
  {
    const auto res = val.transform_error(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.value() == expectedValue);
  }
}

TEST_CASE("transform_error (void value)", "[expected]")
{
  auto transformVoidVal = [](const std::vector<int> &) -> std::vector<double> { return expectedTransformedError; };
  auto transformVoidValExtra = [](const std::vector<int> &, int) -> std::vector<double> {
    return expectedTransformedError;
  };

  const ExpectedVoid val;
  const ExpectedVoid err{frt::unexpected{expectedError}};

  SECTION("error with void value without extra args (!has_value)")
  {
    const auto res = err.transform_error(transformVoidVal);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with void value without extra args (error)")
  {
    const auto res = err.transform_error(transformVoidVal);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error with void value with extra args (!has_value)")
  {
    const auto res = err.transform_error(transformVoidValExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with void value with extra args (error)")
  {
    const auto res = err.transform_error(transformVoidValExtra, extraArg);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("value with void value without extra args (has_value)")
  {
    const auto res = val.transform_error(transformVoidVal);
    REQUIRE(res.has_value());
  }

  SECTION("value with void value with extra args (has_value)")
  {
    const auto res = val.transform_error(transformVoidValExtra, extraArg);
    REQUIRE(res.has_value());
  }
}
