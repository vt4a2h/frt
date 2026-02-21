#include <catch2/catch_all.hpp>

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

TEST_CASE("Compare either and value (either has left)", "[either]")
{
  const either e{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
  const ComparableTypeFirst v1{"Lorem ipsum dolor sit amet, "};
  const ComparableTypeFirst v2{"consectetur adipiscing elit, sed do eiusmod tempor "};
  const ComparableTypeSecond v3{1};

  SECTION("Compare either (left) and value (left) - equal") { REQUIRE(e == v2); }
  SECTION("Compare either (left) and value (left) - not equal") { REQUIRE(e != v1); }
  SECTION("Compare either (left) and value (left) - less than") { REQUIRE(v1 < e); }
  SECTION("Compare either (left) and value (left) - greater than") { REQUIRE(e > v1); }
  SECTION("Compare either (left) and value (left) - less or equal") { REQUIRE(v2 <= e); }
  SECTION("Compare either (left) and value (left) - greater or equal") { REQUIRE(e >= v2); }

  SECTION("Compare either (left) and value (right) - not equal") { REQUIRE(e != v3); }
  SECTION("Compare either (left) and value (right) - not equal (reverse)") { REQUIRE(v3 != e); }
  SECTION("Compare either (left) and value (right) - unordered")
  {
    REQUIRE((e <=> v3) == std::partial_ordering::unordered);
  }
  SECTION("Compare either (left) and value (right) - not less than") { REQUIRE_FALSE(e < v3); }
  SECTION("Compare either (left) and value (right) - not greater than") { REQUIRE_FALSE(e > v3); }
  SECTION("Compare either (left) and value (right) - not less or equal") { REQUIRE_FALSE(e <= v3); }
  SECTION("Compare either (left) and value (right) - not greater or equal") { REQUIRE_FALSE(e >= v3); }
}

TEST_CASE("Compare either and value (either has right)", "[either]")
{
  const either e{frt::create_right, ComparableTypeSecond{2}};
  const ComparableTypeSecond v1{1};
  const ComparableTypeSecond v2{2};
  const ComparableTypeFirst v3{"Lorem ipsum dolor sit amet, "};

  SECTION("Compare either (right) and value (right) - equal") { REQUIRE(e == v2); }
  SECTION("Compare either (right) and value (right) - not equal") { REQUIRE(e != v1); }
  SECTION("Compare either (right) and value (right) - less than") { REQUIRE(v1 < e); }
  SECTION("Compare either (right) and value (right) - greater than") { REQUIRE(e > v1); }
  SECTION("Compare either (right) and value (right) - less or equal") { REQUIRE(v2 <= e); }
  SECTION("Compare either (right) and value (right) - greater or equal") { REQUIRE(e >= v2); }

  SECTION("Compare either (right) and value (left) - not equal") { REQUIRE(e != v3); }
  SECTION("Compare either (right) and value (left) - not equal (reverse)") { REQUIRE(v3 != e); }
  SECTION("Compare either (right) and value (left) - unordered")
  {
    REQUIRE((e <=> v3) == std::partial_ordering::unordered);
  }
  SECTION("Compare either (right) and value (left) - not less than") { REQUIRE_FALSE(e < v3); }
  SECTION("Compare either (right) and value (left) - not greater than") { REQUIRE_FALSE(e > v3); }
  SECTION("Compare either (right) and value (left) - not less or equal") { REQUIRE_FALSE(e <= v3); }
  SECTION("Compare either (right) and value (left) - not greater or equal") { REQUIRE_FALSE(e >= v3); }
}
