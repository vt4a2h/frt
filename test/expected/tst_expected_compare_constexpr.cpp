#include <catch2/catch_all.hpp>

#include <compare>
#include <string>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

using namespace test::helpers;
using Expected = frt::expected<ComparableTypeFirst, ComparableTypeSecond>;
using ExpectedVoid = frt::expected<void, ComparableTypeSecond>;

constexpr auto makeValue1() { return ComparableTypeFirst{"Lorem ipsum dolor sit amet, "}; }
constexpr auto makeValue2()
{
  return ComparableTypeFirst{
      "consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}
constexpr auto makeError1() { return ComparableTypeSecond{10}; }
constexpr auto makeError2() { return ComparableTypeSecond{20}; }

constexpr auto makeIntVal1() { return 10; }
constexpr auto makeIntVal2() { return 20; }
constexpr auto makeDoubleVal1() { return 10.0; }
constexpr auto makeDoubleVal2() { return 20.0; }

TEST_CASE("Compare two expected (constexpr)", "[expected][constexpr]")
{
  SECTION("Both value - equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      return e1 == e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - not equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return e1 != e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - less than")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return e1 < e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - less or equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return e1 <= e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - greater than")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return e2 > e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - greater or equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return e2 >= e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      return (e1 <=> e1) == std::strong_ordering::equal;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way less")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return (e1 <=> e2) == std::strong_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way greater")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e2{makeValue2()};
      return (e2 <=> e1) == std::strong_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - equal")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 == e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - not equal")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return e3 != e4;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - less than")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return e3 < e4;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - less or equal")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return e3 <= e4;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - greater than")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return e4 > e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - greater or equal")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return e4 >= e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way equal")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      return (e3 <=> e3) == std::strong_ordering::equal;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way less")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return (e3 <=> e4) == std::strong_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way greater")
  {
    constexpr auto valid = [] {
      const Expected e3{frt::unexpected{makeError1()}};
      const Expected e4{frt::unexpected{makeError2()}};
      return (e4 <=> e3) == std::strong_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error - not equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e1 != e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error - equal is false")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e1 == e3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not less than")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e1 < e3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not less or equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e1 <= e3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not greater than")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e1 > e3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not greater or equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e1 >= e3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - three-way unordered")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return (e1 <=> e3) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value - not equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 != e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value - equal is false")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 == e1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not less than")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 < e1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not less or equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 <= e1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not greater than")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 > e1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not greater or equal")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return e3 >= e1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - three-way unordered")
  {
    constexpr auto valid = [] {
      const Expected e1{makeValue1()};
      const Expected e3{frt::unexpected{makeError1()}};
      return (e3 <=> e1) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Compare two expected <void, E> (constexpr)", "[expected][constexpr]")
{
  SECTION("Both value - equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid v2;
      return v1 == v2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - not not-equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid v2;
      return v1 != v2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Both value - less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid v2;
      return v1 <= v2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid v2;
      return v1 >= v2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid v2;
      return (v1 <=> v2) == std::partial_ordering::equivalent;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 == err1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return err1 != err2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - less than")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return err1 < err2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return err1 <= err2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - greater than")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return err2 > err1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return err2 >= err1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return (err1 <=> err1) == std::strong_ordering::equal;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way less")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return (err1 <=> err2) == std::strong_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way greater")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      const ExpectedVoid err2{frt::unexpected{makeError2()}};
      return (err2 <=> err1) == std::strong_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return v1 != err1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error - equal is false")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return v1 == err1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not less than")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return v1 < err1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return v1 <= err1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not greater than")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return v1 > err1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return v1 >= err1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - three-way unordered")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return (v1 <=> err1) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 != v1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value - equal is false")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 == v1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not less than")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 < v1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 <= v1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not greater than")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 > v1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return err1 >= v1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - three-way unordered")
  {
    constexpr auto valid = [] {
      const ExpectedVoid v1;
      const ExpectedVoid err1{frt::unexpected{makeError1()}};
      return (err1 <=> v1) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Heterogeneous compare two expected (constexpr)", "[expected][constexpr]")
{
  using ExpectedIntDouble = frt::expected<int, double>;
  using ExpectedDoubleInt = frt::expected<double, int>;

  SECTION("Both value - equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return e1 == e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return e1 != e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - less than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return e1 < e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return e1 <= e3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - greater than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return e3 > e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return e3 >= e1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way equivalent")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return (e1 <=> e2) == std::partial_ordering::equivalent;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way less")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return (e1 <=> e3) == std::partial_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value - three-way greater")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt e3{makeDoubleVal2()};
      return (e3 <=> e1) == std::partial_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return err1 == err2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return err1 != err3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - less than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return err1 < err3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return err1 <= err3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - greater than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return err3 > err1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return err3 >= err1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way equivalent")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return (err1 <=> err2) == std::partial_ordering::equivalent;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way less")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return (err1 <=> err3) == std::partial_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error - three-way greater")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt err3{frt::unexpected{makeIntVal2()}};
      return (err3 <=> err1) == std::partial_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return e1 != err2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error - equal is false")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return e1 == err2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not less than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return e1 < err2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return e1 <= err2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not greater than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return e1 > err2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - not greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return e1 >= err2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Value and error - three-way unordered")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble e1{makeIntVal1()};
      const ExpectedDoubleInt err2{frt::unexpected{makeIntVal1()}};
      return (e1 <=> err2) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value - not equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return err1 != e2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value - equal is false")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return err1 == e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not less than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return err1 < e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not less or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return err1 <= e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not greater than")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return err1 > e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - not greater or equal")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return err1 >= e2;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("Error and value - three-way unordered")
  {
    constexpr auto valid = [] {
      const ExpectedIntDouble err1{frt::unexpected{makeDoubleVal1()}};
      const ExpectedDoubleInt e2{makeDoubleVal1()};
      return (err1 <=> e2) == std::partial_ordering::unordered;
    }();
    STATIC_REQUIRE(valid);
  }
}
