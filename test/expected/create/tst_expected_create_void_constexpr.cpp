#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create expected<void, E> default constructor (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e;
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> default constructor (bool operator) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e;
    return static_cast<bool>(e);
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_expected (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::in_place_expected};
    return e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::unexpected<std::string> u{makeExpectedError()};
    const ExpectedVoid e{u};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::unexpected<std::string> u{makeExpectedError()};
    const ExpectedVoid e{u};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (convertible type) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                 "incididunt ut labore et dolore magna aliqua";
    const frt::unexpected<const char *> u{rawError};
    const ExpectedVoid e{u};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (convertible type) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                 "incididunt ut labore et dolore magna aliqua";
    const frt::unexpected<const char *> u{rawError};
    const ExpectedVoid e{u};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected move constructor (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::unexpected<std::string> u{makeExpectedError()};
    const ExpectedVoid e{std::move(u)};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected move constructor (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::unexpected<std::string> u{makeExpectedError()};
    const ExpectedVoid e{std::move(u)};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected move constructor (rvalue) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::unexpected{makeExpectedError()}};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> unexpected move constructor (rvalue) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::unexpected{makeExpectedError()}};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::in_place_unexpected, makeExpectedError()};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::in_place_unexpected, makeExpectedError()};
    return e.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (custom struct) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    const ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (custom struct) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    const ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};
    return e.error() == TestArgsIl{1, 2.0};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE(
    "Create expected<void, E> in_place_unexpected (initializer_list, Args...) (custom struct) (has_value) constexpr",
    "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    const ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list, Args...) (custom struct) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    const ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
    return e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list) (vector) (has_value) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
    const ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    return !e.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list) (vector) (error) constexpr",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
    const ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    return e.error() == makeExpectedVector();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e;
    const ExpectedVoid other{e};
    return other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::unexpected{makeExpectedError()}};
    const ExpectedVoid other{e};
    return !other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e{frt::unexpected{makeExpectedError()}};
    const ExpectedVoid other{e};
    return other.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e;
    const ExpectedVoid other{std::move(e)};
    return other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e{frt::unexpected{makeExpectedError()}};
    const ExpectedVoid other{std::move(e)};
    return !other.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e{frt::unexpected{makeExpectedError()}};
    const ExpectedVoid other{std::move(e)};
    return other.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> converting copy constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<void, int> src;
    const frt::expected<void, double> dest{src};
    return dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> converting copy constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
    const frt::expected<void, double> dest{src};
    return !dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> converting copy constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
    const frt::expected<void, double> dest{src};
    return dest.error() == makeExpectedDouble();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> converting move constructor (value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<void, int> src;
    const frt::expected<void, double> dest{std::move(src)};
    return dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> converting move constructor (error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
    const frt::expected<void, double> dest{std::move(src)};
    return !dest.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> converting move constructor (error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
    const frt::expected<void, double> dest{std::move(src)};
    return dest.error() == makeExpectedDouble();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy assignment (value to error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = e1;
    return e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy assignment (value to value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e1;
    ExpectedVoid e2;
    e2 = e1;
    return e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy assignment (error to value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
    ExpectedVoid e2;
    e2 = e1;
    return !e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy assignment (error to value) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
    ExpectedVoid e2;
    e2 = e1;
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> copy assignment (error to error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = e1;
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move assignment (value to error) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = std::move(e1);
    return e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move assignment (value to value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2;
    e2 = std::move(e1);
    return e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move assignment (error to value) (has_value) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
    ExpectedVoid e2;
    e2 = std::move(e1);
    return !e2.has_value();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move assignment (error to value) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
    ExpectedVoid e2;
    e2 = std::move(e1);
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Create expected<void, E> move assignment (error to error) (error) constexpr", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = std::move(e1);
    return e2.error() == makeExpectedError();
  }();

  STATIC_REQUIRE(valid);
}
