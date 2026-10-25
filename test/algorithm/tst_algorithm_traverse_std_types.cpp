#include <catch2/catch_all.hpp>

#include <frt/meta/std/meta_either_std_expected.hpp>
#include <frt/meta/std/meta_either_std_optional.hpp>

#include <frt/algorithm/traverse.hpp>

#include <functional>
#include <utility>
#include <vector>

TEST_CASE("frt::traverse -- std::expected", "[algorithm][traverse]")
{
  using Result = std::expected<int, std::string>;

  std::array input{
      Result{std::in_place, 42},
      Result{std::in_place, 41},
      Result{std::in_place, 40},
  };

  const auto actualResult = frt::traverse(input, std::identity{});

  using ExpectedResultType = std::expected<std::vector<int>, std::string>;
  const auto expectedResult = ExpectedResultType{std::in_place, std::vector{42, 41, 40}};

  REQUIRE(actualResult.value() == expectedResult.value());
}

TEST_CASE("frt::traverse -- std::expected<void, ...>", "[algorithm][traverse]")
{
  using Result = std::expected<void, std::string>;

  std::array input{
      Result{std::in_place},
      Result{std::in_place},
      Result{std::in_place},
  };

  const auto actualResult = frt::traverse(input, [](const Result &) -> std::expected<int, std::string> { return 42; });

  using ExpectedResultType = std::expected<std::vector<int>, std::string>;
  const auto expectedResult = ExpectedResultType{std::in_place, std::vector{42, 42, 42}};

  REQUIRE(actualResult.value() == expectedResult.value());
}

TEST_CASE("frt::traverse -- optional", "[algorithm][traverse]")
{
  using Result = std::optional<int>;

  std::array input{
      Result{std::in_place, 42},
      Result{std::in_place, 41},
      Result{std::in_place, 40},
  };

  const auto actualResult = frt::traverse(input, std::identity{});

  using ExpectedResultType = std::optional<std::vector<int>>;
  const auto expectedResult = ExpectedResultType{std::in_place, std::vector{42, 41, 40}};

  REQUIRE(actualResult.value() == expectedResult.value());
}
