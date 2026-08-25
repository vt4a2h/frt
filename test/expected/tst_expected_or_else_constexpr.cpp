#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/expected.hpp>

using Expected = frt::expected<std::string, std::vector<int>>;
using ExpectedVoid = frt::expected<void, std::vector<int>>;

constexpr auto makeExpectedValue()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}
constexpr auto makeExpectedError() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedTransformedError() { return std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0}; }
inline static constexpr int extraArg = 42;

TEST_CASE("or_else constexpr", "[expected][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> frt::expected<std::string, std::vector<double>> {
    return frt::unexpected{makeExpectedTransformedError()};
  };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::expected<std::string, std::vector<double>> {
    return frt::unexpected{makeExpectedTransformedError()};
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::expected<std::string, std::vector<double>> {
    return frt::unexpected{makeExpectedTransformedError()};
  };

  SECTION("error to different error type without extra args (!has_value)")
  {
    constexpr auto valid = [transform] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transform);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error to different error type without extra args (error)")
  {
    constexpr auto valid = [transform] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transform);
      return res.error() == makeExpectedTransformedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error to different error type with extra args without unwrapped (!has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error to different error type with extra args without unwrapped (error)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformExtra, extraArg);
      return res.error() == makeExpectedTransformedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error to different error type with unwrapped in extra args (!has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformUnwrapped, extraArg, frt::unwrapped);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error to different error type with unwrapped in extra args (error)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformUnwrapped, extraArg, frt::unwrapped);
      return res.error() == makeExpectedTransformedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      auto fn = [](std::vector<int> &v) -> frt::expected<std::string, bool> {
        v.clear();
        return frt::unexpected{true};
      };
      [[maybe_unused]] const auto res = expected.or_else(fn);
      return expected.error().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{frt::unexpected{makeExpectedError()}};
      auto fn = [](const std::vector<int> &v) -> frt::expected<std::string, bool> {
        return frt::unexpected{v == makeExpectedError()};
      };
      const auto res = expected.or_else(fn);
      return res.error();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      auto fn = [](std::vector<int> &&v) -> frt::expected<std::string, std::vector<int>> {
        return frt::unexpected{std::move(v)};
      };
      const auto res = std::move(expected).or_else(fn);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Expected val{makeExpectedValue()};
      const auto res = val.or_else(transform);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Expected val{makeExpectedValue()};
      const auto res = val.or_else(transform);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected val{makeExpectedValue()};
      const auto res = val.or_else(transformExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected val{makeExpectedValue()};
      const auto res = val.or_else(transformExtra, extraArg);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected val{makeExpectedValue()};
      const auto res = val.or_else(transformUnwrapped, extraArg, frt::unwrapped);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected val{makeExpectedValue()};
      const auto res = val.or_else(transformUnwrapped, extraArg, frt::unwrapped);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("or_else (void value) constexpr", "[expected][constexpr]")
{
  auto transformVoidVal = [](const std::vector<int> &) -> frt::expected<void, std::vector<double>> {
    return frt::unexpected{makeExpectedTransformedError()};
  };
  auto transformVoidValExtra = [](const std::vector<int> &, int) -> frt::expected<void, std::vector<double>> {
    return frt::unexpected{makeExpectedTransformedError()};
  };

  SECTION("error with void value without extra args (!has_value)")
  {
    constexpr auto valid = [transformVoidVal] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformVoidVal);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value without extra args (error)")
  {
    constexpr auto valid = [transformVoidVal] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformVoidVal);
      return res.error() == makeExpectedTransformedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value with extra args (!has_value)")
  {
    constexpr auto valid = [transformVoidValExtra] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformVoidValExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value with extra args (error)")
  {
    constexpr auto valid = [transformVoidValExtra] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.or_else(transformVoidValExtra, extraArg);
      return res.error() == makeExpectedTransformedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with void value without extra args (has_value)")
  {
    constexpr auto valid = [transformVoidVal] {
      const ExpectedVoid val;
      const auto res = val.or_else(transformVoidVal);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with void value with extra args (has_value)")
  {
    constexpr auto valid = [transformVoidValExtra] {
      const ExpectedVoid val;
      const auto res = val.or_else(transformVoidValExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }
}
