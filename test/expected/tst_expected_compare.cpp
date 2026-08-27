#include <catch2/catch_all.hpp>

#include <compare>
#include <string>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

using namespace test::helpers;
using Expected = frt::expected<ComparableTypeFirst, ComparableTypeSecond>;
using ExpectedVoid = frt::expected<void, ComparableTypeSecond>;

inline static const auto value1 = ComparableTypeFirst{"Lorem ipsum dolor sit amet, "};
inline static const auto value2 =
    ComparableTypeFirst{"consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna "
                        "aliqua"};
inline static const auto error1 = ComparableTypeSecond{10};
inline static const auto error2 = ComparableTypeSecond{20};

TEST_CASE("Compare two expected", "[expected]")
{
  const Expected e1{value1};
  const Expected e2{value2};
  const Expected e3{frt::unexpected{error1}};
  const Expected e4{frt::unexpected{error2}};

  SECTION("Both value - equal") { REQUIRE(e1 == e1); }
  SECTION("Both value - not equal") { REQUIRE(e1 != e2); }
  SECTION("Both value - less than") { REQUIRE(e1 < e2); }
  SECTION("Both value - less or equal") { REQUIRE(e1 <= e2); }
  SECTION("Both value - greater than") { REQUIRE(e2 > e1); }
  SECTION("Both value - greater or equal") { REQUIRE(e2 >= e1); }
  SECTION("Both value - three-way equal") { REQUIRE((e1 <=> e1) == std::strong_ordering::equal); }
  SECTION("Both value - three-way less") { REQUIRE((e1 <=> e2) == std::strong_ordering::less); }
  SECTION("Both value - three-way greater") { REQUIRE((e2 <=> e1) == std::strong_ordering::greater); }

  SECTION("Both error - equal") { REQUIRE(e3 == e3); }
  SECTION("Both error - not equal") { REQUIRE(e3 != e4); }
  SECTION("Both error - less than") { REQUIRE(e3 < e4); }
  SECTION("Both error - less or equal") { REQUIRE(e3 <= e4); }
  SECTION("Both error - greater than") { REQUIRE(e4 > e3); }
  SECTION("Both error - greater or equal") { REQUIRE(e4 >= e3); }
  SECTION("Both error - three-way equal") { REQUIRE((e3 <=> e3) == std::strong_ordering::equal); }
  SECTION("Both error - three-way less") { REQUIRE((e3 <=> e4) == std::strong_ordering::less); }
  SECTION("Both error - three-way greater") { REQUIRE((e4 <=> e3) == std::strong_ordering::greater); }

  SECTION("Value and error - not equal") { REQUIRE(e1 != e3); }
  SECTION("Value and error - equal is false") { REQUIRE_FALSE(e1 == e3); }
  SECTION("Value and error - not less than") { REQUIRE_FALSE(e1 < e3); }
  SECTION("Value and error - not less or equal") { REQUIRE_FALSE(e1 <= e3); }
  SECTION("Value and error - not greater than") { REQUIRE_FALSE(e1 > e3); }
  SECTION("Value and error - not greater or equal") { REQUIRE_FALSE(e1 >= e3); }
  SECTION("Value and error - three-way unordered") { REQUIRE((e1 <=> e3) == std::partial_ordering::unordered); }

  SECTION("Error and value - not equal") { REQUIRE(e3 != e1); }
  SECTION("Error and value - equal is false") { REQUIRE_FALSE(e3 == e1); }
  SECTION("Error and value - not less than") { REQUIRE_FALSE(e3 < e1); }
  SECTION("Error and value - not less or equal") { REQUIRE_FALSE(e3 <= e1); }
  SECTION("Error and value - not greater than") { REQUIRE_FALSE(e3 > e1); }
  SECTION("Error and value - not greater or equal") { REQUIRE_FALSE(e3 >= e1); }
  SECTION("Error and value - three-way unordered") { REQUIRE((e3 <=> e1) == std::partial_ordering::unordered); }
}

TEST_CASE("Compare two expected <void, E>", "[expected]")
{
  const ExpectedVoid v1;
  const ExpectedVoid v2;
  const ExpectedVoid err1{frt::unexpected{error1}};
  const ExpectedVoid err2{frt::unexpected{error2}};

  SECTION("Both value - equal") { REQUIRE(v1 == v2); }
  SECTION("Both value - not not-equal") { REQUIRE_FALSE(v1 != v2); }
  SECTION("Both value - less or equal") { REQUIRE(v1 <= v2); }
  SECTION("Both value - greater or equal") { REQUIRE(v1 >= v2); }
  SECTION("Both value - three-way equal") { REQUIRE((v1 <=> v2) == std::partial_ordering::equivalent); }

  SECTION("Both error - equal") { REQUIRE(err1 == err1); }
  SECTION("Both error - not equal") { REQUIRE(err1 != err2); }
  SECTION("Both error - less than") { REQUIRE(err1 < err2); }
  SECTION("Both error - less or equal") { REQUIRE(err1 <= err2); }
  SECTION("Both error - greater than") { REQUIRE(err2 > err1); }
  SECTION("Both error - greater or equal") { REQUIRE(err2 >= err1); }
  SECTION("Both error - three-way equal") { REQUIRE((err1 <=> err1) == std::strong_ordering::equal); }
  SECTION("Both error - three-way less") { REQUIRE((err1 <=> err2) == std::strong_ordering::less); }
  SECTION("Both error - three-way greater") { REQUIRE((err2 <=> err1) == std::strong_ordering::greater); }

  SECTION("Value and error - not equal") { REQUIRE(v1 != err1); }
  SECTION("Value and error - equal is false") { REQUIRE_FALSE(v1 == err1); }
  SECTION("Value and error - not less than") { REQUIRE_FALSE(v1 < err1); }
  SECTION("Value and error - not less or equal") { REQUIRE_FALSE(v1 <= err1); }
  SECTION("Value and error - not greater than") { REQUIRE_FALSE(v1 > err1); }
  SECTION("Value and error - not greater or equal") { REQUIRE_FALSE(v1 >= err1); }
  SECTION("Value and error - three-way unordered") { REQUIRE((v1 <=> err1) == std::partial_ordering::unordered); }

  SECTION("Error and value - not equal") { REQUIRE(err1 != v1); }
  SECTION("Error and value - equal is false") { REQUIRE_FALSE(err1 == v1); }
  SECTION("Error and value - not less than") { REQUIRE_FALSE(err1 < v1); }
  SECTION("Error and value - not less or equal") { REQUIRE_FALSE(err1 <= v1); }
  SECTION("Error and value - not greater than") { REQUIRE_FALSE(err1 > v1); }
  SECTION("Error and value - not greater or equal") { REQUIRE_FALSE(err1 >= v1); }
  SECTION("Error and value - three-way unordered") { REQUIRE((err1 <=> v1) == std::partial_ordering::unordered); }
}

TEST_CASE("Heterogeneous compare two expected", "[expected]")
{
  using ExpectedIntDouble = frt::expected<int, double>;
  using ExpectedDoubleInt = frt::expected<double, int>;

  const int intVal1 = 10;
  const int intVal2 = 20;
  const double doubleVal1 = 10.0;
  const double doubleVal2 = 20.0;

  const ExpectedIntDouble e1{intVal1};
  const ExpectedDoubleInt e2{doubleVal1};
  const ExpectedDoubleInt e3{doubleVal2};

  const ExpectedIntDouble err1{frt::unexpected{doubleVal1}};
  const ExpectedDoubleInt err2{frt::unexpected{intVal1}};
  const ExpectedDoubleInt err3{frt::unexpected{intVal2}};

  SECTION("Both value - equal") { REQUIRE(e1 == e2); }
  SECTION("Both value - not equal") { REQUIRE(e1 != e3); }
  SECTION("Both value - less than") { REQUIRE(e1 < e3); }
  SECTION("Both value - less or equal") { REQUIRE(e1 <= e3); }
  SECTION("Both value - greater than") { REQUIRE(e3 > e1); }
  SECTION("Both value - greater or equal") { REQUIRE(e3 >= e1); }
  SECTION("Both value - three-way equivalent") { REQUIRE((e1 <=> e2) == std::partial_ordering::equivalent); }
  SECTION("Both value - three-way less") { REQUIRE((e1 <=> e3) == std::partial_ordering::less); }
  SECTION("Both value - three-way greater") { REQUIRE((e3 <=> e1) == std::partial_ordering::greater); }

  SECTION("Both error - equal") { REQUIRE(err1 == err2); }
  SECTION("Both error - not equal") { REQUIRE(err1 != err3); }
  SECTION("Both error - less than") { REQUIRE(err1 < err3); }
  SECTION("Both error - less or equal") { REQUIRE(err1 <= err3); }
  SECTION("Both error - greater than") { REQUIRE(err3 > err1); }
  SECTION("Both error - greater or equal") { REQUIRE(err3 >= err1); }
  SECTION("Both error - three-way equivalent") { REQUIRE((err1 <=> err2) == std::partial_ordering::equivalent); }
  SECTION("Both error - three-way less") { REQUIRE((err1 <=> err3) == std::partial_ordering::less); }
  SECTION("Both error - three-way greater") { REQUIRE((err3 <=> err1) == std::partial_ordering::greater); }

  SECTION("Value and error - not equal") { REQUIRE(e1 != err2); }
  SECTION("Value and error - equal is false") { REQUIRE_FALSE(e1 == err2); }
  SECTION("Value and error - not less than") { REQUIRE_FALSE(e1 < err2); }
  SECTION("Value and error - not less or equal") { REQUIRE_FALSE(e1 <= err2); }
  SECTION("Value and error - not greater than") { REQUIRE_FALSE(e1 > err2); }
  SECTION("Value and error - not greater or equal") { REQUIRE_FALSE(e1 >= err2); }
  SECTION("Value and error - three-way unordered") { REQUIRE((e1 <=> err2) == std::partial_ordering::unordered); }

  SECTION("Error and value - not equal") { REQUIRE(err1 != e2); }
  SECTION("Error and value - equal is false") { REQUIRE_FALSE(err1 == e2); }
  SECTION("Error and value - not less than") { REQUIRE_FALSE(err1 < e2); }
  SECTION("Error and value - not less or equal") { REQUIRE_FALSE(err1 <= e2); }
  SECTION("Error and value - not greater than") { REQUIRE_FALSE(err1 > e2); }
  SECTION("Error and value - not greater or equal") { REQUIRE_FALSE(err1 >= e2); }
  SECTION("Error and value - three-way unordered") { REQUIRE((err1 <=> e2) == std::partial_ordering::unordered); }
}
