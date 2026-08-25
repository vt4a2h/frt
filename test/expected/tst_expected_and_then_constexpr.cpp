#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/expected.hpp>

using Expected = frt::expected<std::vector<int>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

constexpr auto makeExpectedValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedError()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}
constexpr auto makeExpectedTransformedValue() { return std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0}; }
inline static constexpr int extraArg = 42;

TEST_CASE("and_then constexpr", "[expected][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> frt::expected<std::vector<double>, std::string> {
    return makeExpectedTransformedValue();
  };
  auto transformToVoid = [](const std::vector<int> &) -> frt::expected<void, std::string> { return {}; };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::expected<std::vector<double>, std::string> {
    return makeExpectedTransformedValue();
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::expected<std::vector<double>, std::string> {
    return makeExpectedTransformedValue();
  };

  SECTION("value to different value type without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transform);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transform);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to void value without extra args (has_value)")
  {
    constexpr auto valid = [transformToVoid] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transformToVoid);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transformExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transformExtra, extraArg);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected val{makeExpectedValue()};
      const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      auto fn = [](std::vector<int> &v) -> frt::expected<bool, std::string> {
        v.clear();
        return true;
      };
      [[maybe_unused]] const auto res = expected.and_then(fn);
      return expected.value().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{makeExpectedValue()};
      auto fn = [](const std::vector<int> &v) -> frt::expected<bool, std::string> { return v == makeExpectedValue(); };
      const auto res = expected.and_then(fn);
      return res.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      auto fn = [](std::vector<int> &&v) -> frt::expected<std::vector<int>, std::string> { return std::move(v); };
      const auto res = std::move(expected).and_then(fn);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (!has_value)")
  {
    constexpr auto valid = [transform] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transform);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (error)")
  {
    constexpr auto valid = [transform] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transform);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    constexpr auto valid = [transformToVoid] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformToVoid);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args without unwrapped (!has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args without unwrapped (error)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformExtra, extraArg);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with unwrapped in extra args (!has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with unwrapped in extra args (error)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("and_then (void value) constexpr", "[expected][constexpr]")
{
  auto transformVoid = []() -> frt::expected<std::vector<double>, std::string> {
    return makeExpectedTransformedValue();
  };
  auto transformVoidToVoid = []() -> frt::expected<void, std::string> { return {}; };
  auto transformVoidExtra = [](int) -> frt::expected<std::vector<double>, std::string> {
    return makeExpectedTransformedValue();
  };
  auto transformVoidToVoidExtra = [](int) -> frt::expected<void, std::string> { return {}; };

  SECTION("void value to non-void value without extra args (has_value)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid val;
      const auto res = val.and_then(transformVoid);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to non-void value without extra args (value)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid val;
      const auto res = val.and_then(transformVoid);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to void value without extra args (has_value)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const ExpectedVoid val;
      const auto res = val.and_then(transformVoidToVoid);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to non-void value with extra args (has_value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid val;
      const auto res = val.and_then(transformVoidExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to non-void value with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid val;
      const auto res = val.and_then(transformVoidExtra, extraArg);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to void value with extra args (has_value)")
  {
    constexpr auto valid = [transformVoidToVoidExtra] {
      const ExpectedVoid val;
      const auto res = val.and_then(transformVoidToVoidExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (!has_value)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformVoid);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (error)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformVoid);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformVoidToVoid);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args (!has_value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformVoidExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args (error)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.and_then(transformVoidExtra, extraArg);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }
}
