#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create from either converting copy constructor from either (value) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    const Expected e{src};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting copy constructor from either (value) (value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    const Expected e{src};
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting copy constructor from either (error) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    const Expected e{src};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting copy constructor from either (error) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    const Expected e{src};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting move constructor from either (value) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    const Expected e{std::move(src)};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting move constructor from either (value) (value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    const Expected e{std::move(src)};
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting move constructor from either (error) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    const Expected e{std::move(src)};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either converting move constructor from either (error) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    const Expected e{std::move(src)};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either copy assignment from either (value to value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = src;
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either copy assignment from either (value to error) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    Expected e{frt::unexpected{""s}};
    e = src;
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either copy assignment from either (value to error) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    Expected e{frt::unexpected{""s}};
    e = src;
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either copy assignment from either (error to value) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = src;
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either copy assignment from either (error to value) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = src;
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either copy assignment from either (error to error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    Expected e{frt::unexpected{""s}};
    e = src;
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either move assignment from either (value to value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = std::move(src);
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either move assignment from either (value to error) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    Expected e{frt::unexpected{""s}};
    e = std::move(src);
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either move assignment from either (value to error) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, makeExpectedValue()};
    Expected e{frt::unexpected{""s}};
    e = std::move(src);
    return e.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either move assignment from either (error to value) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = std::move(src);
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either move assignment from either (error to value) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = std::move(src);
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create from either move assignment from either (error to error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, makeExpectedError()};
    Expected e{frt::unexpected{""s}};
    e = std::move(src);
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}
