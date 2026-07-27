#include <catch2/catch_all.hpp>
#include <expected>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

using namespace test::helpers;
using either = frt::either<ComparableTypeFirst, ComparableTypeSecond>;

TEST_CASE("Compare two either", "[either]")
{
  const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
  const either e2{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
  const either e3{frt::create_right, ComparableTypeSecond{1}};
  const either e4{frt::create_right, ComparableTypeSecond{2}};

  SECTION("Compare two either (both left) - equal") { REQUIRE(e1 == e1); }
  SECTION("Compare two either (both left) - not equal") { REQUIRE(e1 != e2); }
  SECTION("Compare two either (both left) - less than") { REQUIRE(e1 < e2); }
  SECTION("Compare two either (both left) - less or equal") { REQUIRE(e1 <= e2); }
  SECTION("Compare two either (both left) - greater than") { REQUIRE(e2 > e1); }
  SECTION("Compare two either (both left) - greater or equal") { REQUIRE(e2 >= e1); }

  SECTION("Compare two either (both right) - equal") { REQUIRE(e3 == e3); }
  SECTION("Compare two either (both right) - not equal") { REQUIRE(e3 != e4); }
  SECTION("Compare two either (both right) - less than") { REQUIRE(e3 < e4); }
  SECTION("Compare two either (both right) - less or equal") { REQUIRE(e3 <= e4); }
  SECTION("Compare two either (both right) - greater than") { REQUIRE(e4 > e3); }
  SECTION("Compare two either (both right) - greater or equal") { REQUIRE(e4 >= e3); }

  SECTION("Compare two either (left and right) - not equal") { REQUIRE(e1 != e3); }
  SECTION("Compare two either (left and right) - not less than") { REQUIRE_FALSE(e1 < e3); }
  SECTION("Compare two either (left and right) - not greater than") { REQUIRE_FALSE(e1 > e3); }
  SECTION("Compare two either (left and right) - unordered")
  {
    REQUIRE((e1 <=> e3) == std::partial_ordering::unordered);
  }
  SECTION("Compare two either (left and right) - not less or equal") { REQUIRE_FALSE(e1 <= e3); }
  SECTION("Compare two either (left and right) - not greater or equal") { REQUIRE_FALSE(e1 >= e3); }
}
