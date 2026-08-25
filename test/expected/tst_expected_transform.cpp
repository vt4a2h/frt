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

TEST_CASE("transform", "[expected]")
{
  auto transform = [](const std::vector<int> &) -> std::vector<double> { return expectedTransformedValue; };
  auto transformToVoid = [](const std::vector<int> &) -> void {};
  auto transformExtra = [](const std::vector<int> &, int) -> std::vector<double> { return expectedTransformedValue; };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::vector<double> {
    return expectedTransformedValue;
  };

  const Expected val{expectedValue};
  const Expected err{frt::unexpected{expectedError}};

  SECTION("value to different value type without extra args (has_value)")
  {
    const auto res = val.transform(transform);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type without extra args (value)")
  {
    const auto res = val.transform(transform);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to void value without extra args (has_value)")
  {
    const auto res = val.transform(transformToVoid);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    const auto res = val.transform(transformExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    const auto res = val.transform(transformExtra, extraArg);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    Expected expected{expectedValue};
    auto fn = [](std::vector<int> &v) -> bool {
      v.clear();
      return true;
    };
    [[maybe_unused]] const auto res = expected.transform(fn);
    REQUIRE(expected.value().empty());
  }

  SECTION("value with const lvalue forwarding")
  {
    const Expected expected{expectedValue};
    auto fn = [](const std::vector<int> &v) -> bool { return v == expectedValue; };
    const auto res = expected.transform(fn);
    REQUIRE(res.value());
  }

  SECTION("value with rvalue forwarding")
  {
    Expected expected{expectedValue};
    auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
    const auto res = std::move(expected).transform(fn);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("error without extra args (!has_value)")
  {
    const auto res = err.transform(transform);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error without extra args (error)")
  {
    const auto res = err.transform(transform);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    const auto res = err.transform(transformToVoid);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args without unwrapped (!has_value)")
  {
    const auto res = err.transform(transformExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args without unwrapped (error)")
  {
    const auto res = err.transform(transformExtra, extraArg);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("error with unwrapped in extra args (!has_value)")
  {
    const auto res = err.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with unwrapped in extra args (error)")
  {
    const auto res = err.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.error() == expectedError);
  }
}

TEST_CASE("transform (void value)", "[expected]")
{
  auto transformVoid = []() -> std::vector<double> { return expectedTransformedValue; };
  auto transformVoidToVoid = []() -> void {};
  auto transformVoidExtra = [](int) -> std::vector<double> { return expectedTransformedValue; };
  auto transformVoidToVoidExtra = [](int) -> void {};

  const ExpectedVoid val;
  const ExpectedVoid err{frt::unexpected{expectedError}};

  SECTION("void value to non-void value without extra args (has_value)")
  {
    const auto res = val.transform(transformVoid);
    REQUIRE(res.has_value());
  }

  SECTION("void value to non-void value without extra args (value)")
  {
    const auto res = val.transform(transformVoid);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("void value to void value without extra args (has_value)")
  {
    const auto res = val.transform(transformVoidToVoid);
    REQUIRE(res.has_value());
  }

  SECTION("void value to non-void value with extra args (has_value)")
  {
    const auto res = val.transform(transformVoidExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("void value to non-void value with extra args (value)")
  {
    const auto res = val.transform(transformVoidExtra, extraArg);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("void value to void value with extra args (has_value)")
  {
    const auto res = val.transform(transformVoidToVoidExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("error without extra args (!has_value)")
  {
    const auto res = err.transform(transformVoid);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error without extra args (error)")
  {
    const auto res = err.transform(transformVoid);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    const auto res = err.transform(transformVoidToVoid);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args (!has_value)")
  {
    const auto res = err.transform(transformVoidExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with extra args (error)")
  {
    const auto res = err.transform(transformVoidExtra, extraArg);
    REQUIRE(res.error() == expectedError);
  }
}
