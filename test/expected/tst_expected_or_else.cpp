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

TEST_CASE("or_else", "[expected]")
{
  auto transform = [](const std::vector<int> &) -> frt::expected<std::string, std::vector<double>> {
    return frt::unexpected{expectedTransformedError};
  };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::expected<std::string, std::vector<double>> {
    return frt::unexpected{expectedTransformedError};
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::expected<std::string, std::vector<double>> {
    return frt::unexpected{expectedTransformedError};
  };

  const Expected val{expectedValue};
  const Expected err{frt::unexpected{expectedError}};

  SECTION("error to different error type without extra args (!has_value)")
  {
    const auto res = err.or_else(transform);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error to different error type without extra args (error)")
  {
    const auto res = err.or_else(transform);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error to different error type with extra args without unwrapped (!has_value)")
  {
    const auto res = err.or_else(transformExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error to different error type with extra args without unwrapped (error)")
  {
    const auto res = err.or_else(transformExtra, extraArg);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error to different error type with unwrapped in extra args (!has_value)")
  {
    const auto res = err.or_else(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error to different error type with unwrapped in extra args (error)")
  {
    const auto res = err.or_else(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error with non-const lvalue forwarding")
  {
    Expected expected{frt::unexpected{expectedError}};
    auto fn = [](std::vector<int> &v) -> frt::expected<std::string, bool> {
      v.clear();
      return frt::unexpected{true};
    };
    [[maybe_unused]] const auto res = expected.or_else(fn);
    REQUIRE(expected.error().empty());
  }

  SECTION("error with const lvalue forwarding")
  {
    const Expected expected{frt::unexpected{expectedError}};
    auto fn = [](const std::vector<int> &v) -> frt::expected<std::string, bool> {
      return frt::unexpected{v == expectedError};
    };
    const auto res = expected.or_else(fn);
    REQUIRE(res.error());
  }

  SECTION("error with rvalue forwarding")
  {
    Expected expected{frt::unexpected{expectedError}};
    auto fn = [](std::vector<int> &&v) -> frt::expected<std::string, std::vector<int>> {
      return frt::unexpected{std::move(v)};
    };
    const auto res = std::move(expected).or_else(fn);
    REQUIRE(res.error() == expectedError);
  }

  SECTION("value without extra args (has_value)")
  {
    const auto res = val.or_else(transform);
    REQUIRE(res.has_value());
  }

  SECTION("value without extra args (value)")
  {
    const auto res = val.or_else(transform);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with extra args without unwrapped (has_value)")
  {
    const auto res = val.or_else(transformExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("value with extra args without unwrapped (value)")
  {
    const auto res = val.or_else(transformExtra, extraArg);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with unwrapped in extra args (has_value)")
  {
    const auto res = val.or_else(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.has_value());
  }

  SECTION("value with unwrapped in extra args (value)")
  {
    const auto res = val.or_else(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.value() == expectedValue);
  }
}

TEST_CASE("or_else (void value)", "[expected]")
{
  auto transformVoidVal = [](const std::vector<int> &) -> frt::expected<void, std::vector<double>> {
    return frt::unexpected{expectedTransformedError};
  };
  auto transformVoidValExtra = [](const std::vector<int> &, int) -> frt::expected<void, std::vector<double>> {
    return frt::unexpected{expectedTransformedError};
  };

  const ExpectedVoid val;
  const ExpectedVoid err{frt::unexpected{expectedError}};

  SECTION("error with void value without extra args (!has_value)")
  {
    const auto res = err.or_else(transformVoidVal);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with void value without extra args (error)")
  {
    const auto res = err.or_else(transformVoidVal);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("error with void value with extra args (!has_value)")
  {
    const auto res = err.or_else(transformVoidValExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("error with void value with extra args (error)")
  {
    const auto res = err.or_else(transformVoidValExtra, extraArg);
    REQUIRE(res.error() == expectedTransformedError);
  }

  SECTION("value with void value without extra args (has_value)")
  {
    const auto res = val.or_else(transformVoidVal);
    REQUIRE(res.has_value());
  }

  SECTION("value with void value with extra args (has_value)")
  {
    const auto res = val.or_else(transformVoidValExtra, extraArg);
    REQUIRE(res.has_value());
  }
}
