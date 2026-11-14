#include <catch2/catch_all.hpp>

#include <frt/algorithm/sequence.hpp>
#include <frt/either.hpp>

#include <functional>
#include <list>
#include <string>
#include <vector>

TEST_CASE("frt::algorithm::sequence -- either", "[algorithm][sequence]")
{
  using Result = frt::either<std::string, int>;

  const std::vector input{
      Result{frt::in_place_right, 42},
      Result{frt::in_place_right, 41},
      Result{frt::in_place_right, 40},
  };

  const std::vector expectedResult{42, 41, 40};

  SECTION("Regular call")
  {
    const auto actualResult = frt::algorithm::sequence(input);
    REQUIRE(actualResult.right() == expectedResult);
  }

  SECTION("Pipe call")
  {
    const auto actualResult = input | frt::algorithm::sequence;
    REQUIRE(actualResult.right() == expectedResult);
  }
}

TEST_CASE("frt::algorithm::sequence_into -- either", "[algorithm][sequence_into]")
{
  using Result = frt::either<std::string, int>;

  const std::vector input{
      Result{frt::in_place_right, 42},
      Result{frt::in_place_right, 41},
      Result{frt::in_place_right, 40},
  };

  const std::list expectedResult{42, 41, 40};

  SECTION("Regular call")
  {
    const auto actualResult = frt::algorithm::sequence_into<std::list>(input);
    REQUIRE(actualResult.right() == expectedResult);
  }

  SECTION("Pipe call")
  {
    const auto actualResult = input | frt::algorithm::sequence_into<std::list>;
    REQUIRE(actualResult.right() == expectedResult);
  }
}
