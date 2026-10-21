#include <catch2/catch_all.hpp>

#include <frt/algorithm/and_then_right.hpp>
#include <frt/meta/meta_either_expected.hpp>

#include <memory>

TEST_CASE("and_then alg -- either", "[algorithm][and_then]")
{
  using Result = frt::either<std::string, int>;

  constexpr int expectedContainedValue = 42;
  const auto rObj = Result{frt::in_place_right, expectedContainedValue - 1};

  const auto finalObj = frt::and_then_right(rObj, [](int v) -> Result { return Result{frt::in_place_right, v + 1}; });

  REQUIRE(finalObj.right() == expectedContainedValue);
}

TEST_CASE("and_then_right partial alg -- either", "[algorithm][and_then]")
{
  using Result = frt::either<std::string, int>;

  constexpr int expectedContainedValue = 42;
  const auto rObj = Result{frt::in_place_right, expectedContainedValue - 1};

  const auto finalObj = rObj | frt::and_then_right([](int v) -> Result { return Result{frt::in_place_right, v + 1}; });

  REQUIRE(finalObj.right() == expectedContainedValue);
}

TEST_CASE("and_then_right partial alg with args -- either", "[algorithm][and_then]")
{
  using Result = frt::either<std::string, int>;

  constexpr auto init = 42;
  constexpr auto add = 1;

  auto er = Result{frt::in_place_right, init} |
            frt::and_then_right([](int v, int s, int s1) -> Result { return Result{frt::in_place_right, v + s + s1}; },
                                add, add);

  constexpr auto expectedRight = init + add + add;

  REQUIRE(er.right() == expectedRight);
}
