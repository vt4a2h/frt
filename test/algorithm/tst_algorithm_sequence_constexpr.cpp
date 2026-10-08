#include <catch2/catch_all.hpp>

#include <frt/meta/meta_either_either.hpp>

#include <frt/algorithm/sequence.hpp>

#include <functional>
#include <list>
#include <string>
#include <vector>

using Result = frt::either<std::string, int>;

inline constexpr std::array input{
    Result{frt::in_place_right, 42},
    Result{frt::in_place_right, 41},
    Result{frt::in_place_right, 40},
};

inline constexpr std::array expectedResult{42, 41, 40};

template <class... Args>
struct MyVec : std::vector<Args...>
{
  using std::vector<Args...>::vector;
};

TEST_CASE("frt::sequence constexpr -- either", "[algorithm][sequence][constexpr]")
{
  SECTION("Regular call")
  {
    constexpr auto valid = [] {
      const auto actualResult = frt::sequence(input);
      return std::ranges::equal(actualResult.right(), expectedResult);
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Pipe call")
  {
    constexpr auto valid = [] {
      const auto actualResult = input | frt::sequence;
      return std::ranges::equal(actualResult.right(), expectedResult);
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::sequence_into constexpr -- either", "[algorithm][sequence_into][constexpr]")
{
  SECTION("Regular call")
  {
    constexpr auto valid = [] {
      const auto actualResult = frt::sequence_into<MyVec>(input);
      return std::ranges::equal(actualResult.right(), expectedResult);
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Pipe call")
  {
    constexpr auto valid = [] {
      const auto actualResult = input | frt::sequence_into<MyVec>;
      return std::ranges::equal(actualResult.right(), expectedResult);
    }();
    STATIC_REQUIRE(valid);
  }
}
