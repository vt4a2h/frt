#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create default and value default constructor (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e;
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value default constructor (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e;
    return e.value() == std::vector<int>{};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value default constructor (bool operator) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e;
    return static_cast<bool>(e);
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value value constructor (lvalue) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{makeExpectedValue()};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value value constructor (lvalue) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{makeExpectedValue()};
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value value constructor (rvalue) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{makeExpectedValue()};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value value constructor (rvalue) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{makeExpectedValue()};
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value value constructor (convertible type) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<double, std::string> e{makeExpectedInt()};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value value constructor (convertible type) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<double, std::string> e{makeExpectedInt()};
    return e.value() == makeExpectedDouble();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (Args...) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::in_place_expected, makeExpectedValue()};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (Args...) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::in_place_expected, makeExpectedValue()};
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (Args...) (custom struct) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    const ExpectedFoo e{frt::in_place_expected, 1, 2.0};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (Args...) (custom struct) (value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    const ExpectedFoo e{frt::in_place_expected, 1, 2.0};
    return e.value() == TestArgsIl{1, 2.0};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE(
    "Create default and value in_place_expected (initializer_list, Args...) (custom struct) (has_value) constexpr",
    "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    const ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (initializer_list, Args...) (custom struct) (value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    const ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};
    return e.value() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (initializer_list) (vector) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create default and value in_place_expected (initializer_list) (vector) (value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}
