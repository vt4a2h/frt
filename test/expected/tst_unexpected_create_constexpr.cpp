#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/data.hpp>
#include <test/helpers/types.hpp>

TEST_CASE("Create constexpr", "[unexpected][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;

  SECTION("Value constructor (same type - int)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedInt()};
      return u.error() == makeExpectedInt();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value constructor (same type - string)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      return u.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value constructor (same type - vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      return u.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value constructor (convertible type - const char*)")
  {
    constexpr auto valid = [] {
      const char *const rawString =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
          "dolore magna aliqua";
      frt::unexpected<std::string> u{rawString};
      return u.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value constructor (convertible type - int to double)")
  {
    constexpr auto valid = [] {
      const int intVal = 42;
      const double expectedDoubleValue = 42.0;
      frt::unexpected<double> u{intVal};
      return u.error() == expectedDoubleValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value constructor (move rvalue)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{std::string{
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
          "dolore magna aliqua"}};
      return u.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (string)")
  {
    constexpr auto valid = [] {
      const std::size_t count = 5;
      const char ch = 'a';
      const std::string expectedFilledString{"aaaaa"};
      frt::unexpected<std::string> u{frt::in_place_unexpected, count, ch};
      return u.error() == expectedFilledString;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (custom struct)")
  {
    constexpr auto valid = [] {
      const int v = 1;
      const double v1 = 2.0;
      frt::unexpected<TestArgsIl> u{frt::in_place_unexpected, v, v1};
      return u.error() == TestArgsIl{v, v1};
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct)")
  {
    constexpr auto valid = [] {
      const int v = 1;
      const double v1 = 2.0;
      frt::unexpected<TestArgsIl> u{frt::in_place_unexpected, {"a"s, "b"s}, v, v1};
      return u.error() == TestArgsIl{{"a"s, "b"s}, v, v1};
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list) (vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::vector<int>> u{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return u.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (int)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedInt()};
      return u.error() == makeExpectedInt();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (string)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      return u.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      return u.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (int)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedInt()};
      frt::unexpected other{u};
      return other.error() == makeExpectedInt();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (string)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      frt::unexpected other{u};
      return other.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      frt::unexpected other{u};
      return other.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (string)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      frt::unexpected other{std::move(u)};
      return other.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      frt::unexpected other{std::move(u)};
      return other.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (string)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      frt::unexpected other{""s};
      other = u;
      return other.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      frt::unexpected other{std::vector<int>{}};
      other = u;
      return other.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (string)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      frt::unexpected other{""s};
      other = std::move(u);
      return other.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (vector)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      frt::unexpected other{std::vector<int>{}};
      other = std::move(u);
      return other.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }
}
