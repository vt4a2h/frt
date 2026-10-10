#include <catch2/catch_all.hpp>

#include <frt/algorithm/and_then.hpp>
#include <frt/meta/meta_either_expected.hpp>

#include <memory>

TEST_CASE("and_then alg -- expected", "[algorithm][and_then]")
{
  using Result = frt::either<std::string, int>;

  constexpr int expectedContainedValue = 42;
  const auto rObj = Result{frt::in_place_right, expectedContainedValue - 1};

  const auto finalObj =
      frt::detail::and_then_alg_::and_then(rObj, [](int v) -> Result { return Result{frt::in_place_right, v + 1}; });

  REQUIRE(finalObj.right() == expectedContainedValue);
}
