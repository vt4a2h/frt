#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/data.hpp>
#include <test/helpers/types.hpp>

TEST_CASE("Create", "[unexpected]")
{
  using namespace test::helpers;
  using namespace std::literals;

  SECTION("Value constructor (same type - int)")
  {
    frt::unexpected u{expectedIntValue};
    REQUIRE(u.error() == expectedIntValue);
  }

  SECTION("Value constructor (same type - string)")
  {
    frt::unexpected u{expectedStringValue};
    REQUIRE(u.error() == expectedStringValue);
  }

  SECTION("Value constructor (same type - vector)")
  {
    frt::unexpected u{expectedVectorValue};
    REQUIRE(u.error() == expectedVectorValue);
  }

  SECTION("Value constructor (convertible type - const char*)")
  {
    const char *const rawString =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
        "dolore magna aliqua";
    frt::unexpected<std::string> u{rawString};
    REQUIRE(u.error() == expectedStringValue);
  }

  SECTION("Value constructor (convertible type - int to double)")
  {
    const int intVal = 42;
    frt::unexpected<double> u{intVal};
    REQUIRE(u.error() == expectedDoubleValue);
  }

  SECTION("Value constructor (move rvalue)")
  {
    frt::unexpected u{std::string{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
        "dolore magna aliqua"}};
    REQUIRE(u.error() == expectedStringValue);
  }

  SECTION("in_place_unexpected (Args...) (string)")
  {
    const std::size_t count = 5;
    const char ch = 'a';
    const std::string expectedFilledString{"aaaaa"};
    frt::unexpected<std::string> u{frt::in_place_unexpected, count, ch};
    REQUIRE(u.error() == expectedFilledString);
  }

  SECTION("in_place_unexpected (Args...) (custom struct)")
  {
    const int v = 1;
    const double v1 = 2.0;
    frt::unexpected<TestArgsIl> u{frt::in_place_unexpected, v, v1};
    REQUIRE(u.error() == TestArgsIl{v, v1});
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct)")
  {
    const int v = 1;
    const double v1 = 2.0;
    frt::unexpected<TestArgsIl> u{frt::in_place_unexpected, {"a"s, "b"s}, v, v1};
    REQUIRE(u.error() == TestArgsIl{{"a"s, "b"s}, v, v1});
  }

  SECTION("in_place_unexpected (initializer_list) (vector)")
  {
    frt::unexpected<std::vector<int>> u{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    REQUIRE(u.error() == expectedVectorValue);
  }

  SECTION("CTAD (int)")
  {
    frt::unexpected u{expectedIntValue};
    REQUIRE(u.error() == expectedIntValue);
  }

  SECTION("CTAD (string)")
  {
    frt::unexpected u{expectedStringValue};
    REQUIRE(u.error() == expectedStringValue);
  }

  SECTION("CTAD (vector)")
  {
    frt::unexpected u{expectedVectorValue};
    REQUIRE(u.error() == expectedVectorValue);
  }

  SECTION("copy constructor (int)")
  {
    frt::unexpected u{expectedIntValue};
    frt::unexpected other{u};
    REQUIRE(other.error() == expectedIntValue);
  }

  SECTION("copy constructor (string)")
  {
    frt::unexpected u{expectedStringValue};
    frt::unexpected other{u};
    REQUIRE(other.error() == expectedStringValue);
  }

  SECTION("copy constructor (vector)")
  {
    frt::unexpected u{expectedVectorValue};
    frt::unexpected other{u};
    REQUIRE(other.error() == expectedVectorValue);
  }

  SECTION("move constructor (string)")
  {
    frt::unexpected u{expectedStringValue};
    frt::unexpected other{std::move(u)};
    REQUIRE(other.error() == expectedStringValue);
  }

  SECTION("move constructor (vector)")
  {
    frt::unexpected u{expectedVectorValue};
    frt::unexpected other{std::move(u)};
    REQUIRE(other.error() == expectedVectorValue);
  }

  SECTION("copy assignment (string)")
  {
    frt::unexpected u{expectedStringValue};
    frt::unexpected other{""s};
    other = u;
    REQUIRE(other.error() == expectedStringValue);
  }

  SECTION("copy assignment (vector)")
  {
    frt::unexpected u{expectedVectorValue};
    frt::unexpected other{std::vector<int>{}};
    other = u;
    REQUIRE(other.error() == expectedVectorValue);
  }

  SECTION("move assignment (string)")
  {
    frt::unexpected u{expectedStringValue};
    frt::unexpected other{""s};
    other = std::move(u);
    REQUIRE(other.error() == expectedStringValue);
  }

  SECTION("move assignment (vector)")
  {
    frt::unexpected u{expectedVectorValue};
    frt::unexpected other{std::vector<int>{}};
    other = std::move(u);
    REQUIRE(other.error() == expectedVectorValue);
  }
}
