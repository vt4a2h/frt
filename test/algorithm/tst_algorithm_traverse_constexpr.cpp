#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>
#include <frt/either.hpp>
#include <frt/expected.hpp>
#include <frt/optional.hpp>

#include <array>
#include <functional>
#include <vector>

TEST_CASE("frt::traverse constexpr -- either", "[algorithm][traverse][constexpr]")
{
  using Result = frt::either<int, int>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      using ExpectedResult = frt::either<int, std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = input | frt::traverse(std::identity{});
      using ExpectedResult = frt::either<int, std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure short-circuits and preserves left value")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_left, -1},
          Result{frt::in_place_right, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      return actual.is_left() && actual.left() == -1;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse constexpr -- expected", "[algorithm][traverse][constexpr]")
{
  using Result = frt::expected<int, double>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_expected, 42},
          Result{frt::in_place_expected, 41},
          Result{frt::in_place_expected, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      using ExpectedResult = frt::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{frt::in_place_expected, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_expected, 42},
          Result{frt::in_place_expected, 41},
          Result{frt::in_place_expected, 40},
      };

      const auto actual = input | frt::traverse(std::identity{});
      using ExpectedResult = frt::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{frt::in_place_expected, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure short-circuits and returns error")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_expected, 42},
          Result{frt::unexpected{-1}},
          Result{frt::in_place_expected, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      return !actual.has_value() && actual.error() == -1;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse constexpr -- expected<void, ...>", "[algorithm][traverse][constexpr]")
{
  using Result = frt::expected<void, int>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_expected},
          Result{frt::in_place_expected},
          Result{frt::in_place_expected},
      };

      const auto actual = input | frt::traverse([](const Result &) -> frt::expected<int, double> { return 42; });
      using ExpectedResult = frt::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{frt::in_place_expected, std::vector{42, 42, 42}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure from mapped function")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_expected},
          Result{frt::in_place_expected},
      };

      int callCount = 0;
      auto fn = [&callCount](const Result &) -> frt::expected<int, double> {
        ++callCount;
        if (callCount == 1) {
          return frt::unexpected{-42};
        }
        return 100;
      };

      const auto actual = frt::traverse(input, fn);
      return !actual.has_value() && actual.error() == -42 && callCount == 1;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse constexpr -- optional", "[algorithm][traverse][constexpr]")
{
  using Result = frt::optional<int>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_opt, 42},
          Result{frt::in_place_opt, 41},
          Result{frt::in_place_opt, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      using ExpectedResult = frt::optional<std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_opt, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_opt, 42},
          Result{frt::in_place_opt, 41},
          Result{frt::in_place_opt, 40},
      };

      const auto actual = input | frt::traverse(std::identity{});
      using ExpectedResult = frt::optional<std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_opt, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("empty optional short-circuits")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_opt, 42},
          Result{},
          Result{frt::in_place_opt, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      return !actual.has_value();
    }();

    STATIC_REQUIRE(valid);
  }
}
