#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/expected.hpp>

using Expected = frt::expected<std::vector<int>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

inline static const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::string expectedError =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline static const std::vector expectedTransformedValue{1.0, 2.0, 3.0, 4.0, 5.0};
inline static constexpr int extraArg = 42;

TEST_CASE("and_then", "[expected]")
{
  auto transform = [](const std::vector<int> &) -> frt::expected<std::vector<double>, std::string> {
    return expectedTransformedValue;
  };
  auto transformToVoid = [](const std::vector<int> &) -> frt::expected<void, std::string> { return {}; };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::expected<std::vector<double>, std::string> {
    return expectedTransformedValue;
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::expected<std::vector<double>, std::string> {
    return expectedTransformedValue;
  };

  const Expected val{expectedValue};
  const Expected err{frt::unexpected{expectedError}};

  SECTION("value to different value type without extra args (has_value)")
  {
    const auto res = val.and_then(transform);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type without extra args (value)")
  {
    const auto res = val.and_then(transform);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to void value without extra args (has_value)")
  {
    const auto res = val.and_then(transformToVoid);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    const auto res = val.and_then(transformExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    const auto res = val.and_then(transformExtra, extraArg);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    Expected expected{expectedValue};
    auto fn = [](std::vector<int> &v) -> frt::expected<bool, std::string> {
      v.clear();
      return true;
    };
    [[maybe_unused]] const auto res = expected.and_then(fn);
    REQUIRE(expected.value().empty());
  }

  SECTION("value with const lvalue forwarding")
  {
    const Expected expected{expectedValue};
    auto fn = [](const std::vector<int> &v) -> frt::expected<bool, std::string> { return v == expectedValue; };
    const auto res = expected.and_then(fn);
    REQUIRE(res.value());
  }

  SECTION("value with rvalue forwarding")
  {
    Expected expected{expectedValue};
    auto fn = [](std::vector<int> &&v) -> frt::expected<std::vector<int>, std::string> { return std::move(v); };
    const auto res = std::move(expected).and_then(fn);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("error without extra args (!has_value)")
  {
    const auto res = err.and_then(transform);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error without extra args (error)")
  {
    const auto res = err.and_then(transform);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    const auto res = err.and_then(transformToVoid);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args without unwrapped (!has_value)")
  {
    const auto res = err.and_then(transformExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args without unwrapped (error)")
  {
    const auto res = err.and_then(transformExtra, extraArg);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("error with unwrapped in extra args (!has_value)")
  {
    const auto res = err.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with unwrapped in extra args (error)")
  {
    const auto res = err.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.error() == expectedError);
  }
}

TEST_CASE("and_then (void value)", "[expected]")
{
  auto transformVoid = []() -> frt::expected<std::vector<double>, std::string> { return expectedTransformedValue; };
  auto transformVoidToVoid = []() -> frt::expected<void, std::string> { return {}; };
  auto transformVoidExtra = [](int) -> frt::expected<std::vector<double>, std::string> {
    return expectedTransformedValue;
  };
  auto transformVoidToVoidExtra = [](int) -> frt::expected<void, std::string> { return {}; };

  const ExpectedVoid val;
  const ExpectedVoid err{frt::unexpected{expectedError}};

  SECTION("void value to non-void value without extra args (has_value)")
  {
    const auto res = val.and_then(transformVoid);
    REQUIRE(res.has_value());
  }

  SECTION("void value to non-void value without extra args (value)")
  {
    const auto res = val.and_then(transformVoid);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("void value to void value without extra args (has_value)")
  {
    const auto res = val.and_then(transformVoidToVoid);
    REQUIRE(res.has_value());
  }

  SECTION("void value to non-void value with extra args (has_value)")
  {
    const auto res = val.and_then(transformVoidExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("void value to non-void value with extra args (value)")
  {
    const auto res = val.and_then(transformVoidExtra, extraArg);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("void value to void value with extra args (has_value)")
  {
    const auto res = val.and_then(transformVoidToVoidExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("error without extra args (!has_value)")
  {
    const auto res = err.and_then(transformVoid);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error without extra args (error)")
  {
    const auto res = err.and_then(transformVoid);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    const auto res = err.and_then(transformVoidToVoid);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args (!has_value)")
  {
    const auto res = err.and_then(transformVoidExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args (error)")
  {
    const auto res = err.and_then(transformVoidExtra, extraArg);
    REQUIRE(res.error() == expectedError);
  }
}
