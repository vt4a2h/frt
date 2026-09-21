#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>

TEST_CASE("frt::traverse -- either", "[algorithm]")
{
  using Result = frt::either<std::string, int>;

  std::vector input{
      Result{frt::in_place_right, 42},
      Result{frt::in_place_right, 41},
      Result{frt::in_place_right, 40},
  };

  const auto actualResult = frt::detail::trav_alg_::traverse(input, std::identity{});

  using ExpectedResultType = frt::either<std::string, std::vector<int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_right, std::vector{42, 41, 40}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- expected", "[algorithm]")
{
  using Result = frt::expected<int, std::string>;

  std::vector input{
      Result{frt::in_place_expected, 42},
      Result{frt::in_place_expected, 41},
      Result{frt::in_place_expected, 40},
  };

  const auto actualResult = frt::detail::trav_alg_::traverse(input, std::identity{});

  using ExpectedResultType = frt::expected<std::vector<int>, std::string>;
  const auto expectedResult = ExpectedResultType{frt::in_place_expected, std::vector{42, 41, 40}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- expected<void, ...>", "[algorithm]")
{
  using Result = frt::expected<void, std::string>;

  std::vector input{
      Result{frt::in_place_expected},
      Result{frt::in_place_expected},
      Result{frt::in_place_expected},
  };

  const auto actualResult =
      frt::detail::trav_alg_::traverse(input, [](const Result &) -> frt::expected<int, std::string> { return 42; });

  using ExpectedResultType = frt::expected<std::vector<int>, std::string>;
  const auto expectedResult = ExpectedResultType{frt::in_place_expected, std::vector{42, 42, 42}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- optional", "[algorithm]")
{
  using Result = frt::optional<int>;

  std::vector input{
      Result{frt::in_place_opt, 42},
      Result{frt::in_place_opt, 41},
      Result{frt::in_place_opt, 40},
  };

  const auto actualResult = frt::detail::trav_alg_::traverse(input, std::identity{});

  using ExpectedResultType = frt::optional<std::vector<int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_opt, std::vector{42, 41, 40}};

  REQUIRE(actualResult == expectedResult);
}
