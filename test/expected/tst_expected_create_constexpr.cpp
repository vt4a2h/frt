#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

constexpr auto makeExpectedInt() { return 42; }
constexpr auto makeExpectedDouble() { return 42.0; }
constexpr auto makeExpectedValue() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedError()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

TEST_CASE("Create constexpr", "[expected][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Expected = frt::expected<std::vector<int>, std::string>;

  SECTION("default constructor (has_value)")
  {
    constexpr auto valid = [] {
      Expected e;
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("default constructor (value)")
  {
    constexpr auto valid = [] {
      Expected e;
      return e.value() == std::vector<int>{};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("default constructor (bool operator)")
  {
    constexpr auto valid = [] {
      Expected e;
      return static_cast<bool>(e);
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value constructor (lvalue) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value constructor (lvalue) (value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      return e.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value constructor (rvalue) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value constructor (rvalue) (value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      return e.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value constructor (convertible type) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<double, std::string> e{makeExpectedInt()};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value constructor (convertible type) (value)")
  {
    constexpr auto valid = [] {
      frt::expected<double, std::string> e{makeExpectedInt()};
      return e.value() == makeExpectedDouble();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (Args...) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::in_place_expected, makeExpectedValue()};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (Args...) (value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::in_place_expected, makeExpectedValue()};
      return e.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (Args...) (custom struct) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
      ExpectedFoo e{frt::in_place_expected, 1, 2.0};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (Args...) (custom struct) (value)")
  {
    constexpr auto valid = [] {
      using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
      ExpectedFoo e{frt::in_place_expected, 1, 2.0};
      return e.value() == TestArgsIl{1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (initializer_list, Args...) (custom struct) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
      ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (initializer_list, Args...) (custom struct) (value)")
  {
    constexpr auto valid = [] {
      using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
      ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};
      return e.value() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (initializer_list) (vector) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (initializer_list) (vector) (value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return e.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (has_value)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      Expected e{u};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (error)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      Expected e{u};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (convertible type) (has_value)")
  {
    constexpr auto valid = [] {
      const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                   "incididunt ut labore et dolore magna aliqua";
      frt::unexpected<const char *> u{rawError};
      Expected e{u};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (convertible type) (error)")
  {
    constexpr auto valid = [] {
      const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                   "incididunt ut labore et dolore magna aliqua";
      frt::unexpected<const char *> u{rawError};
      Expected e{u};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (has_value)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      Expected e{std::move(u)};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (error)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      Expected e{std::move(u)};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (rvalue) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::unexpected{makeExpectedError()}};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (rvalue) (error)")
  {
    constexpr auto valid = [] {
      Expected e{frt::unexpected{makeExpectedError()}};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::in_place_unexpected, makeExpectedError()};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (error)")
  {
    constexpr auto valid = [] {
      Expected e{frt::in_place_unexpected, makeExpectedError()};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
      ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (error)")
  {
    constexpr auto valid = [] {
      using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
      ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};
      return e.error() == TestArgsIl{1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
      ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (error)")
  {
    constexpr auto valid = [] {
      using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
      ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
      return e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
      ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (error)")
  {
    constexpr auto valid = [] {
      using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
      ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return e.error() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      Expected other{e};
      return other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (value) (value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      Expected other{e};
      return other.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::unexpected{makeExpectedError()}};
      Expected other{e};
      return !other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (error) (error)")
  {
    constexpr auto valid = [] {
      Expected e{frt::unexpected{makeExpectedError()}};
      Expected other{e};
      return other.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      Expected other{std::move(e)};
      return other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (value) (value)")
  {
    constexpr auto valid = [] {
      Expected e{makeExpectedValue()};
      Expected other{std::move(e)};
      return other.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e{frt::unexpected{makeExpectedError()}};
      Expected other{std::move(e)};
      return !other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (error) (error)")
  {
    constexpr auto valid = [] {
      Expected e{frt::unexpected{makeExpectedError()}};
      Expected other{std::move(e)};
      return other.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{makeExpectedInt()};
      frt::expected<double, std::string> dest{src};
      return dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (value) (value)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{makeExpectedInt()};
      frt::expected<double, std::string> dest{src};
      return dest.value() == makeExpectedDouble();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
      frt::expected<double, std::string> dest{src};
      return !dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (error) (error)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
      frt::expected<double, std::string> dest{src};
      return dest.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{makeExpectedInt()};
      frt::expected<double, std::string> dest{std::move(src)};
      return dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (value) (value)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{makeExpectedInt()};
      frt::expected<double, std::string> dest{std::move(src)};
      return dest.value() == makeExpectedDouble();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
      frt::expected<double, std::string> dest{std::move(src)};
      return !dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (error) (error)")
  {
    constexpr auto valid = [] {
      frt::expected<int, std::string> src{frt::unexpected{makeExpectedError()}};
      frt::expected<double, std::string> dest{std::move(src)};
      return dest.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (value to value) (value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeExpectedValue()};
      Expected e2;
      e2 = e1;
      return e2.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (value to error) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeExpectedValue()};
      Expected e2{frt::unexpected{""s}};
      e2 = e1;
      return e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (value to error) (value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeExpectedValue()};
      Expected e2{frt::unexpected{""s}};
      e2 = e1;
      return e2.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (error to value) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeExpectedError()}};
      Expected e2{makeExpectedValue()};
      e2 = e1;
      return !e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (error to value) (error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeExpectedError()}};
      Expected e2{makeExpectedValue()};
      e2 = e1;
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (error to error) (error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeExpectedError()}};
      Expected e2{frt::unexpected{""s}};
      e2 = e1;
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (value to value) (value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeExpectedValue()};
      Expected e2;
      e2 = std::move(e1);
      return e2.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (value to error) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeExpectedValue()};
      Expected e2{frt::unexpected{""s}};
      e2 = std::move(e1);
      return e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (value to error) (value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeExpectedValue()};
      Expected e2{frt::unexpected{""s}};
      e2 = std::move(e1);
      return e2.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (error to value) (has_value)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeExpectedError()}};
      Expected e2{makeExpectedValue()};
      e2 = std::move(e1);
      return !e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (error to value) (error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeExpectedError()}};
      Expected e2{makeExpectedValue()};
      e2 = std::move(e1);
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (error to error) (error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeExpectedError()}};
      Expected e2{frt::unexpected{""s}};
      e2 = std::move(e1);
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Create expected<void, E> constexpr", "[expected][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using ExpectedVoid = frt::expected<void, std::string>;

  SECTION("default constructor (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e;
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("default constructor (bool operator)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e;
      return static_cast<bool>(e);
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_expected (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::in_place_expected};
      return e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (has_value)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      ExpectedVoid e{u};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (error)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      ExpectedVoid e{u};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (convertible type) (has_value)")
  {
    constexpr auto valid = [] {
      const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                   "incididunt ut labore et dolore magna aliqua";
      frt::unexpected<const char *> u{rawError};
      ExpectedVoid e{u};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected copy constructor (convertible type) (error)")
  {
    constexpr auto valid = [] {
      const char *const rawError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                   "incididunt ut labore et dolore magna aliqua";
      frt::unexpected<const char *> u{rawError};
      ExpectedVoid e{u};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (has_value)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      ExpectedVoid e{std::move(u)};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (error)")
  {
    constexpr auto valid = [] {
      frt::unexpected<std::string> u{makeExpectedError()};
      ExpectedVoid e{std::move(u)};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (rvalue) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::unexpected{makeExpectedError()}};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("unexpected move constructor (rvalue) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::unexpected{makeExpectedError()}};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::in_place_unexpected, makeExpectedError()};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::in_place_unexpected, makeExpectedError()};
      return e.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
      ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (error)")
  {
    constexpr auto valid = [] {
      using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
      ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};
      return e.error() == TestArgsIl{1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
      ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (error)")
  {
    constexpr auto valid = [] {
      using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
      ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};
      return e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (has_value)")
  {
    constexpr auto valid = [] {
      using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
      ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return !e.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (error)")
  {
    constexpr auto valid = [] {
      using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
      ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
      return e.error() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e;
      ExpectedVoid other{e};
      return other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::unexpected{makeExpectedError()}};
      ExpectedVoid other{e};
      return !other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (error) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::unexpected{makeExpectedError()}};
      ExpectedVoid other{e};
      return other.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e;
      ExpectedVoid other{std::move(e)};
      return other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::unexpected{makeExpectedError()}};
      ExpectedVoid other{std::move(e)};
      return !other.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (error) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e{frt::unexpected{makeExpectedError()}};
      ExpectedVoid other{std::move(e)};
      return other.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<void, int> src;
      frt::expected<void, double> dest{src};
      return dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
      frt::expected<void, double> dest{src};
      return !dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor (error) (error)")
  {
    constexpr auto valid = [] {
      frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
      frt::expected<void, double> dest{src};
      return dest.error() == makeExpectedDouble();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (value) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<void, int> src;
      frt::expected<void, double> dest{std::move(src)};
      return dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (error) (has_value)")
  {
    constexpr auto valid = [] {
      frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
      frt::expected<void, double> dest{std::move(src)};
      return !dest.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor (error) (error)")
  {
    constexpr auto valid = [] {
      frt::expected<void, int> src{frt::unexpected{makeExpectedInt()}};
      frt::expected<void, double> dest{std::move(src)};
      return dest.error() == makeExpectedDouble();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (value to error) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{""s}};
      e2 = e1;
      return e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (value to value) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2;
      e2 = e1;
      return e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (error to value) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
      ExpectedVoid e2;
      e2 = e1;
      return !e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (error to value) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
      ExpectedVoid e2;
      e2 = e1;
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (error to error) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
      ExpectedVoid e2{frt::unexpected{""s}};
      e2 = e1;
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (value to error) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{""s}};
      e2 = std::move(e1);
      return e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (value to value) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2;
      e2 = std::move(e1);
      return e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (error to value) (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
      ExpectedVoid e2;
      e2 = std::move(e1);
      return !e2.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (error to value) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
      ExpectedVoid e2;
      e2 = std::move(e1);
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (error to error) (error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeExpectedError()}};
      ExpectedVoid e2{frt::unexpected{""s}};
      e2 = std::move(e1);
      return e2.error() == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }
}
