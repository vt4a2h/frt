#include <catch2/catch_all.hpp>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

using namespace test::helpers;
using either = frt::either<ComparableTypeFirst, ComparableTypeSecond>;

TEST_CASE("Compare two either (constexpr)", "[either][constexpr]")
{
  using namespace std::literals;

  SECTION("Compare two either (both left) - equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      return e1 == e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both left) - not equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
      return e1 != e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both left) - less than")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
      return e1 < e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both left) - less or equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
      return e1 <= e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both left) - greater than")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
      return e2 > e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both left) - greater or equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_left, ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor "}};
      return e2 >= e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both right) - equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_right, ComparableTypeSecond{1}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return e1 == e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both right) - not equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_right, ComparableTypeSecond{1}};
      const either e2{frt::create_right, ComparableTypeSecond{2}};
      return e1 != e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both right) - less than")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_right, ComparableTypeSecond{1}};
      const either e2{frt::create_right, ComparableTypeSecond{2}};
      return e1 < e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both right) - less or equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_right, ComparableTypeSecond{1}};
      const either e2{frt::create_right, ComparableTypeSecond{2}};
      return e1 <= e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both right) - greater than")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_right, ComparableTypeSecond{1}};
      const either e2{frt::create_right, ComparableTypeSecond{2}};
      return e2 > e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (both right) - greater or equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_right, ComparableTypeSecond{1}};
      const either e2{frt::create_right, ComparableTypeSecond{2}};
      return e2 >= e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (left and right) - not equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return e1 != e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (left and right) - not less than")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return e1 < e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Compare two either (left and right) - not greater than")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return e1 > e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Compare two either (left and right) - unordered")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return (e1 <=> e2) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare two either (left and right) - not less or equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return e1 <= e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Compare two either (left and right) - not greater or equal")
  {
    constexpr auto valid = [] {
      const either e1{frt::create_left, ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}};
      const either e2{frt::create_right, ComparableTypeSecond{1}};
      return e1 >= e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }
}
