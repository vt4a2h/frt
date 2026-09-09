#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create copy and move copy constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{makeExpectedValue()};
    const Expected other{e};
    return other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy constructor (value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{makeExpectedValue()};
    const Expected other{e};
    return other.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::unexpected{makeExpectedError()}};
    const Expected other{e};
    return !other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e{frt::unexpected{makeExpectedError()}};
    const Expected other{e};
    return other.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e{makeExpectedValue()};
    const Expected other{std::move(e)};
    return other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move constructor (value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e{makeExpectedValue()};
    const Expected other{std::move(e)};
    return other.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e{frt::unexpected{makeExpectedError()}};
    const Expected other{std::move(e)};
    return !other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e{frt::unexpected{makeExpectedError()}};
    const Expected other{std::move(e)};
    return other.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting copy constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<int, std::string> src{makeExpectedInt()};
    const frt::expected<double, std::string> dest{src};
    return dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting copy constructor (value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<int, std::string> src{makeExpectedInt()};
    const frt::expected<double, std::string> dest{src};
    return dest.value() == makeExpectedDouble();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting copy constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
    const frt::expected<double, std::string> dest{src};
    return !dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting copy constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
    const frt::expected<double, std::string> dest{src};
    return dest.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting move constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<int, std::string> src{makeExpectedInt()};
    const frt::expected<double, std::string> dest{std::move(src)};
    return dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting move constructor (value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<int, std::string> src{makeExpectedInt()};
    const frt::expected<double, std::string> dest{std::move(src)};
    return dest.value() == makeExpectedDouble();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting move constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
    const frt::expected<double, std::string> dest{std::move(src)};
    return !dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move converting move constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
    const frt::expected<double, std::string> dest{std::move(src)};
    return dest.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy assignment (value to value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e1{makeExpectedValue()};
    Expected e2;
    e2 = e1;
    return e2.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy assignment (value to error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e1{makeExpectedValue()};
    Expected e2{frt::unexpected{""s}};
    e2 = e1;
    return e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy assignment (value to error) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e1{makeExpectedValue()};
    Expected e2{frt::unexpected{""s}};
    e2 = e1;
    return e2.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy assignment (error to value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e1{frt::unexpected{makeExpectedError()}};
    Expected e2{makeExpectedValue()};
    e2 = e1;
    return !e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy assignment (error to value) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e1{frt::unexpected{makeExpectedError()}};
    Expected e2{makeExpectedValue()};
    e2 = e1;
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move copy assignment (error to error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const Expected e1{frt::unexpected{makeExpectedError()}};
    Expected e2{frt::unexpected{""s}};
    e2 = e1;
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move assignment (value to value) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{makeExpectedValue()};
    Expected e2;
    e2 = std::move(e1);
    return e2.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move assignment (value to error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{makeExpectedValue()};
    Expected e2{frt::unexpected{""s}};
    e2 = std::move(e1);
    return e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move assignment (value to error) (value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{makeExpectedValue()};
    Expected e2{frt::unexpected{""s}};
    e2 = std::move(e1);
    return e2.value() == makeExpectedValue();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move assignment (error to value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{makeExpectedError()}};
    Expected e2{makeExpectedValue()};
    e2 = std::move(e1);
    return !e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move assignment (error to value) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{makeExpectedError()}};
    Expected e2{makeExpectedValue()};
    e2 = std::move(e1);
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create copy and move move assignment (error to error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{makeExpectedError()}};
    Expected e2{frt::unexpected{""s}};
    e2 = std::move(e1);
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}
