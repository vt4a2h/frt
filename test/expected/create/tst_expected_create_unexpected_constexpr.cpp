#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create unexpected unexpected copy constructor (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::unexpected<std::string> u{makeExpectedError()};
    const Expected e{u};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected copy constructor (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::unexpected<std::string> u{makeExpectedError()};
    const Expected e{u};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected copy constructor (convertible type) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                 "incididunt ut labore et dolore magna aliqua";
    const frt::unexpected<const char *> u{rawError};
    const Expected e{u};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected copy constructor (convertible type) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                 "incididunt ut labore et dolore magna aliqua";
    const frt::unexpected<const char *> u{rawError};
    const Expected e{u};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected move constructor (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::unexpected<std::string> u{makeExpectedError()};
    const Expected e{std::move(u)};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected move constructor (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::unexpected<std::string> u{makeExpectedError()};
    const Expected e{std::move(u)};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected move constructor (rvalue) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::unexpected{makeExpectedError()}};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected unexpected move constructor (rvalue) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::unexpected{makeExpectedError()}};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::in_place_unexpected, makeExpectedError()};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::in_place_unexpected, makeExpectedError()};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (custom struct) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    const ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (custom struct) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    const ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};
    return e.error() == TestArgsIl{1, 2.0};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list, Args...) (custom struct) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    const ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list, Args...) (custom struct) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    const ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
    return e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list) (vector) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
    const ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list) (vector) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
    const ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    return e.error() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}
