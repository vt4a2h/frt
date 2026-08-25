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

TEST_CASE("transform constexpr", "[expected][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> std::vector<double> { return makeExpectedTransformedValue(); };
  auto transformToVoid = [](const std::vector<int> &) -> void {};
  auto transformExtra = [](const std::vector<int> &, int) -> std::vector<double> {
    return makeExpectedTransformedValue();
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::vector<double> {
    return makeExpectedTransformedValue();
  };

  SECTION("value to different value type without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transform);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transform);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to void value without extra args (has_value)")
  {
    constexpr auto valid = [transformToVoid] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transformToVoid);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transformExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transformExtra, extraArg);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected val{makeExpectedValue()};
      const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      auto fn = [](std::vector<int> &v) -> bool {
        v.clear();
        return true;
      };
      [[maybe_unused]] const auto res = expected.transform(fn);
      return expected.value().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{makeExpectedValue()};
      auto fn = [](const std::vector<int> &v) -> bool { return v == makeExpectedValue(); };
      const auto res = expected.transform(fn);
      return res.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
      const auto res = std::move(expected).transform(fn);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (!has_value)")
  {
    constexpr auto valid = [transform] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transform);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (error)")
  {
    constexpr auto valid = [transform] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transform);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    constexpr auto valid = [transformToVoid] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformToVoid);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args without unwrapped (!has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args without unwrapped (error)")
  {
    constexpr auto valid = [transformExtra] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformExtra, extraArg);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with unwrapped in extra args (!has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with unwrapped in extra args (error)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("transform (void value) constexpr", "[expected][constexpr]")
{
  auto transformVoid = []() -> std::vector<double> { return makeExpectedTransformedValue(); };
  auto transformVoidToVoid = []() -> void {};
  auto transformVoidExtra = [](int) -> std::vector<double> { return makeExpectedTransformedValue(); };
  auto transformVoidToVoidExtra = [](int) -> void {};

  SECTION("void value to non-void value without extra args (has_value)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid val;
      const auto res = val.transform(transformVoid);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to non-void value without extra args (value)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid val;
      const auto res = val.transform(transformVoid);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to void value without extra args (has_value)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const ExpectedVoid val;
      const auto res = val.transform(transformVoidToVoid);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to non-void value with extra args (has_value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid val;
      const auto res = val.transform(transformVoidExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to non-void value with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid val;
      const auto res = val.transform(transformVoidExtra, extraArg);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void value to void value with extra args (has_value)")
  {
    constexpr auto valid = [transformVoidToVoidExtra] {
      const ExpectedVoid val;
      const auto res = val.transform(transformVoidToVoidExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (!has_value)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformVoid);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error without extra args (error)")
  {
    constexpr auto valid = [transformVoid] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformVoid);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with void value return type without extra args (!has_value)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformVoidToVoid);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args (!has_value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformVoidExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with extra args (error)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      const auto res = err.transform(transformVoidExtra, extraArg);
      return res.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }
}
