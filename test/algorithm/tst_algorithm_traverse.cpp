#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>

TEST_CASE("frt::traverse", "[algorithm]")
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
