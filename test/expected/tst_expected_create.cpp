#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

TEST_CASE("Create", "[expected]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Expected = frt::expected<std::vector<int>, std::string>;

  const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::string expectedError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                    "incididunt ut labore et dolore magna aliqua";

  SECTION("default constructor (has_value)")
  {
    Expected e;

    REQUIRE(e.has_value());
  }

  SECTION("default constructor (value)")
  {
    Expected e;

    REQUIRE(e.value() == std::vector<int>{});
  }

  SECTION("default constructor (bool operator)")
  {
    Expected e;

    REQUIRE(static_cast<bool>(e));
  }

  SECTION("value constructor (lvalue) (has_value)")
  {
    Expected e{expectedValue};

    REQUIRE(e.has_value());
  }

  SECTION("value constructor (lvalue) (value)")
  {
    Expected e{expectedValue};

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("value constructor (rvalue) (has_value)")
  {
    Expected e{std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE(e.has_value());
  }

  SECTION("value constructor (rvalue) (value)")
  {
    Expected e{std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("value constructor (convertible type) (has_value)")
  {
    const int intVal = 42;
    frt::expected<double, std::string> e{intVal};

    REQUIRE(e.has_value());
  }

  SECTION("value constructor (convertible type) (value)")
  {
    const int intVal = 42;
    const double expectedDoubleValue = 42.0;
    frt::expected<double, std::string> e{intVal};

    REQUIRE(e.value() == expectedDoubleValue);
  }

  SECTION("in_place_expected (Args...) (has_value)")
  {
    Expected e{frt::in_place_expected, expectedValue};

    REQUIRE(e.has_value());
  }

  SECTION("in_place_expected (Args...) (value)")
  {
    Expected e{frt::in_place_expected, expectedValue};

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("in_place_expected (Args...) (custom struct) (has_value)")
  {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    ExpectedFoo e{frt::in_place_expected, 1, 2.0};

    REQUIRE(e.has_value());
  }

  SECTION("in_place_expected (Args...) (custom struct) (value)")
  {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    ExpectedFoo e{frt::in_place_expected, 1, 2.0};

    REQUIRE(e.value() == TestArgsIl{1, 2.0});
  }

  SECTION("in_place_expected (initializer_list, Args...) (custom struct) (has_value)")
  {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(e.has_value());
  }

  SECTION("in_place_expected (initializer_list, Args...) (custom struct) (value)")
  {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(e.value() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("in_place_expected (initializer_list) (vector) (has_value)")
  {
    Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE(e.has_value());
  }

  SECTION("in_place_expected (initializer_list) (vector) (value)")
  {
    Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("unexpected copy constructor (has_value)")
  {
    frt::unexpected u{expectedError};
    Expected e{u};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected copy constructor (error)")
  {
    frt::unexpected u{expectedError};
    Expected e{u};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("unexpected copy constructor (convertible type) (has_value)")
  {
    frt::unexpected u{expectedError.data()};
    Expected e{u};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected copy constructor (convertible type) (error)")
  {
    frt::unexpected u{expectedError.data()};
    Expected e{u};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("unexpected move constructor (has_value)")
  {
    frt::unexpected u{expectedError};
    Expected e{std::move(u)};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected move constructor (error)")
  {
    frt::unexpected u{expectedError};
    Expected e{std::move(u)};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("unexpected move constructor (rvalue) (has_value)")
  {
    Expected e{frt::unexpected{std::string{expectedError}}};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected move constructor (rvalue) (error)")
  {
    Expected e{frt::unexpected{std::string{expectedError}}};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("in_place_unexpected (Args...) (has_value)")
  {
    Expected e{frt::in_place_unexpected, expectedError};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (Args...) (error)")
  {
    Expected e{frt::in_place_unexpected, expectedError};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (has_value)")
  {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (error)")
  {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};

    REQUIRE(e.error() == TestArgsIl{1, 2.0});
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (has_value)")
  {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (error)")
  {
    using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
    ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (has_value)")
  {
    using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
    ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (error)")
  {
    using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
    ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE(e.error() == expectedValue);
  }

  SECTION("copy constructor (value) (has_value)")
  {
    const Expected e{expectedValue};
    const Expected other{e};

    REQUIRE(other.has_value());
  }

  SECTION("copy constructor (value) (value)")
  {
    const Expected e{expectedValue};
    const Expected other{e};

    REQUIRE(other.value() == expectedValue);
  }

  SECTION("copy constructor (error) (has_value)")
  {
    const Expected e{frt::unexpected{expectedError}};
    const Expected other{e};

    REQUIRE_FALSE(other.has_value());
  }

  SECTION("copy constructor (error) (error)")
  {
    const Expected e{frt::unexpected{expectedError}};
    const Expected other{e};

    REQUIRE(other.error() == expectedError);
  }

  SECTION("move constructor (value) (has_value)")
  {
    Expected e{expectedValue};
    const Expected other{std::move(e)};

    REQUIRE(other.has_value());
  }

  SECTION("move constructor (value) (value)")
  {
    Expected e{expectedValue};
    const Expected other{std::move(e)};

    REQUIRE(other.value() == expectedValue);
  }

  SECTION("move constructor (error) (has_value)")
  {
    Expected e{frt::unexpected{expectedError}};
    const Expected other{std::move(e)};

    REQUIRE_FALSE(other.has_value());
  }

  SECTION("move constructor (error) (error)")
  {
    Expected e{frt::unexpected{expectedError}};
    const Expected other{std::move(e)};

    REQUIRE(other.error() == expectedError);
  }

  SECTION("converting copy constructor (value) (has_value)")
  {
    const int intVal = 42;
    const frt::expected<int, std::string> src{intVal};
    const frt::expected<double, std::string> dest{src};

    REQUIRE(dest.has_value());
  }

  SECTION("converting copy constructor (value) (value)")
  {
    const int intVal = 42;
    const double expectedDoubleValue = 42.0;
    const frt::expected<int, std::string> src{intVal};
    const frt::expected<double, std::string> dest{src};

    REQUIRE(dest.value() == expectedDoubleValue);
  }

  SECTION("converting copy constructor (error) (has_value)")
  {
    const frt::expected<int, std::string> src{frt::unexpected{expectedError}};
    const frt::expected<double, std::string> dest{src};

    REQUIRE_FALSE(dest.has_value());
  }

  SECTION("converting copy constructor (error) (error)")
  {
    const frt::expected<int, std::string> src{frt::unexpected{expectedError}};
    const frt::expected<double, std::string> dest{src};

    REQUIRE(dest.error() == expectedError);
  }

  SECTION("converting move constructor (value) (has_value)")
  {
    const int intVal = 42;
    frt::expected<int, std::string> src{intVal};
    const frt::expected<double, std::string> dest{std::move(src)};

    REQUIRE(dest.has_value());
  }

  SECTION("converting move constructor (value) (value)")
  {
    const int intVal = 42;
    const double expectedDoubleValue = 42.0;
    frt::expected<int, std::string> src{intVal};
    const frt::expected<double, std::string> dest{std::move(src)};

    REQUIRE(dest.value() == expectedDoubleValue);
  }

  SECTION("converting move constructor (error) (has_value)")
  {
    frt::expected<int, std::string> src{frt::unexpected{expectedError}};
    const frt::expected<double, std::string> dest{std::move(src)};

    REQUIRE_FALSE(dest.has_value());
  }

  SECTION("converting move constructor (error) (error)")
  {
    frt::expected<int, std::string> src{frt::unexpected{expectedError}};
    const frt::expected<double, std::string> dest{std::move(src)};

    REQUIRE(dest.error() == expectedError);
  }

  SECTION("copy assignment (value to value) (value)")
  {
    const Expected e1{expectedValue};
    Expected e2;
    e2 = e1;

    REQUIRE(e2.value() == expectedValue);
  }

  SECTION("copy assignment (value to error) (has_value)")
  {
    const Expected e1{expectedValue};
    Expected e2{frt::unexpected{""s}};
    e2 = e1;

    REQUIRE(e2.has_value());
  }

  SECTION("copy assignment (value to error) (value)")
  {
    const Expected e1{expectedValue};
    Expected e2{frt::unexpected{""s}};
    e2 = e1;

    REQUIRE(e2.value() == expectedValue);
  }

  SECTION("copy assignment (error to value) (has_value)")
  {
    const Expected e1{frt::unexpected{expectedError}};
    Expected e2{expectedValue};
    e2 = e1;

    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("copy assignment (error to value) (error)")
  {
    const Expected e1{frt::unexpected{expectedError}};
    Expected e2{expectedValue};
    e2 = e1;

    REQUIRE(e2.error() == expectedError);
  }

  SECTION("copy assignment (error to error) (error)")
  {
    const Expected e1{frt::unexpected{expectedError}};
    Expected e2{frt::unexpected{""s}};
    e2 = e1;

    REQUIRE(e2.error() == expectedError);
  }

  SECTION("move assignment (value to value) (value)")
  {
    const Expected e1{expectedValue};
    Expected e2;
    e2 = std::move(e1);

    REQUIRE(e2.value() == expectedValue);
  }

  SECTION("move assignment (value to error) (has_value)")
  {
    const Expected e1{expectedValue};
    Expected e2{frt::unexpected{""s}};
    e2 = std::move(e1);

    REQUIRE(e2.has_value());
  }

  SECTION("move assignment (value to error) (value)")
  {
    const Expected e1{expectedValue};
    Expected e2{frt::unexpected{""s}};
    e2 = std::move(e1);

    REQUIRE(e2.value() == expectedValue);
  }

  SECTION("move assignment (error to value) (has_value)")
  {
    const Expected e1{frt::unexpected{expectedError}};
    Expected e2{expectedValue};
    e2 = std::move(e1);

    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("move assignment (error to value) (error)")
  {
    const Expected e1{frt::unexpected{expectedError}};
    Expected e2{expectedValue};
    e2 = std::move(e1);

    REQUIRE(e2.error() == expectedError);
  }

  SECTION("move assignment (error to error) (error)")
  {
    const Expected e1{frt::unexpected{expectedError}};
    Expected e2{frt::unexpected{""s}};
    e2 = std::move(e1);

    REQUIRE(e2.error() == expectedError);
  }
  SECTION("converting copy constructor from either (value) (has_value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    const Expected e{src};

    REQUIRE(e.has_value());
  }

  SECTION("converting copy constructor from either (value) (value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    const Expected e{src};

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("converting copy constructor from either (error) (has_value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    const Expected e{src};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("converting copy constructor from either (error) (error)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    const Expected e{src};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("converting move constructor from either (value) (has_value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    const Expected e{std::move(src)};

    REQUIRE(e.has_value());
  }

  SECTION("converting move constructor from either (value) (value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    const Expected e{std::move(src)};

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("converting move constructor from either (error) (has_value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    const Expected e{std::move(src)};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("converting move constructor from either (error) (error)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    const Expected e{std::move(src)};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("copy assignment from either (value to value) (value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = src;

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("copy assignment from either (value to error) (has_value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    Expected e{frt::unexpected{""s}};
    e = src;

    REQUIRE(e.has_value());
  }

  SECTION("copy assignment from either (value to error) (value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    Expected e{frt::unexpected{""s}};
    e = src;

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("copy assignment from either (error to value) (has_value)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = src;

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("copy assignment from either (error to value) (error)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = src;

    REQUIRE(e.error() == expectedError);
  }

  SECTION("copy assignment from either (error to error) (error)")
  {
    const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    Expected e{frt::unexpected{""s}};
    e = src;

    REQUIRE(e.error() == expectedError);
  }

  SECTION("move assignment from either (value to value) (value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = std::move(src);

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("move assignment from either (value to error) (has_value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    Expected e{frt::unexpected{""s}};
    e = std::move(src);

    REQUIRE(e.has_value());
  }

  SECTION("move assignment from either (value to error) (value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
    Expected e{frt::unexpected{""s}};
    e = std::move(src);

    REQUIRE(e.value() == expectedValue);
  }

  SECTION("move assignment from either (error to value) (has_value)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = std::move(src);

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("move assignment from either (error to value) (error)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    Expected e{frt::in_place_expected, std::vector<int>{}};
    e = std::move(src);

    REQUIRE(e.error() == expectedError);
  }

  SECTION("move assignment from either (error to error) (error)")
  {
    frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
    Expected e{frt::unexpected{""s}};
    e = std::move(src);

    REQUIRE(e.error() == expectedError);
  }
}

TEST_CASE("Create expected<void, E>", "[expected]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using ExpectedVoid = frt::expected<void, std::string>;

  const std::string expectedError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                    "incididunt ut labore et dolore magna aliqua";

  SECTION("default constructor (has_value)")
  {
    const ExpectedVoid e;

    REQUIRE(e.has_value());
  }

  SECTION("default constructor (bool operator)")
  {
    const ExpectedVoid e;

    REQUIRE(static_cast<bool>(e));
  }

  SECTION("in_place_expected (has_value)")
  {
    ExpectedVoid e{frt::in_place_expected};

    REQUIRE(e.has_value());
  }

  SECTION("unexpected copy constructor (has_value)")
  {
    frt::unexpected u{expectedError};
    ExpectedVoid e{u};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected copy constructor (error)")
  {
    frt::unexpected u{expectedError};
    ExpectedVoid e{u};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("unexpected copy constructor (convertible type) (has_value)")
  {
    frt::unexpected u{expectedError.data()};
    ExpectedVoid e{u};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected copy constructor (convertible type) (error)")
  {
    frt::unexpected u{expectedError.data()};
    ExpectedVoid e{u};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("unexpected move constructor (has_value)")
  {
    frt::unexpected u{expectedError};
    ExpectedVoid e{std::move(u)};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected move constructor (error)")
  {
    frt::unexpected u{expectedError};
    ExpectedVoid e{std::move(u)};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("unexpected move constructor (rvalue) (has_value)")
  {
    ExpectedVoid e{
        frt::unexpected{std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                    "incididunt ut labore et dolore magna aliqua"}}};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("unexpected move constructor (rvalue) (error)")
  {
    ExpectedVoid e{
        frt::unexpected{std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                    "incididunt ut labore et dolore magna aliqua"}}};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("in_place_unexpected (Args...) (has_value)")
  {
    ExpectedVoid e{frt::in_place_unexpected, expectedError};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (Args...) (error)")
  {
    ExpectedVoid e{frt::in_place_unexpected, expectedError};

    REQUIRE(e.error() == expectedError);
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (has_value)")
  {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (Args...) (custom struct) (error)")
  {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};

    REQUIRE(e.error() == TestArgsIl{1, 2.0});
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (has_value)")
  {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (initializer_list, Args...) (custom struct) (error)")
  {
    using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
    ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (has_value)")
  {
    using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
    const std::vector expectedVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE_FALSE(e.has_value());
  }

  SECTION("in_place_unexpected (initializer_list) (vector) (error)")
  {
    using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
    const std::vector expectedVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

    REQUIRE(e.error() == expectedVector);
  }

  SECTION("copy constructor (value) (has_value)")
  {
    const ExpectedVoid e;
    const ExpectedVoid other{e};

    REQUIRE(other.has_value());
  }

  SECTION("copy constructor (error) (has_value)")
  {
    const ExpectedVoid e{frt::unexpected{expectedError}};
    const ExpectedVoid other{e};

    REQUIRE_FALSE(other.has_value());
  }

  SECTION("copy constructor (error) (error)")
  {
    const ExpectedVoid e{frt::unexpected{expectedError}};
    const ExpectedVoid other{e};

    REQUIRE(other.error() == expectedError);
  }

  SECTION("move constructor (value) (has_value)")
  {
    const ExpectedVoid e;
    const ExpectedVoid other{std::move(e)};

    REQUIRE(other.has_value());
  }

  SECTION("move constructor (error) (has_value)")
  {
    ExpectedVoid e{frt::unexpected{expectedError}};
    const ExpectedVoid other{std::move(e)};

    REQUIRE_FALSE(other.has_value());
  }

  SECTION("move constructor (error) (error)")
  {
    ExpectedVoid e{frt::unexpected{expectedError}};
    const ExpectedVoid other{std::move(e)};

    REQUIRE(other.error() == expectedError);
  }

  SECTION("converting copy constructor (value) (has_value)")
  {
    const frt::expected<void, int> src;
    const frt::expected<void, double> dest{src};

    REQUIRE(dest.has_value());
  }

  SECTION("converting copy constructor (error) (has_value)")
  {
    const int intVal = 42;
    const frt::expected<void, int> src{frt::unexpected{intVal}};
    const frt::expected<void, double> dest{src};

    REQUIRE_FALSE(dest.has_value());
  }

  SECTION("converting copy constructor (error) (error)")
  {
    const int intVal = 42;
    const double expectedDoubleValue = 42.0;
    const frt::expected<void, int> src{frt::unexpected{intVal}};
    const frt::expected<void, double> dest{src};

    REQUIRE(dest.error() == expectedDoubleValue);
  }

  SECTION("converting move constructor (value) (has_value)")
  {
    frt::expected<void, int> src;
    const frt::expected<void, double> dest{std::move(src)};

    REQUIRE(dest.has_value());
  }

  SECTION("converting move constructor (error) (has_value)")
  {
    const int intVal = 42;
    frt::expected<void, int> src{frt::unexpected{intVal}};
    const frt::expected<void, double> dest{std::move(src)};

    REQUIRE_FALSE(dest.has_value());
  }

  SECTION("converting move constructor (error) (error)")
  {
    const int intVal = 42;
    const double expectedDoubleValue = 42.0;
    frt::expected<void, int> src{frt::unexpected{intVal}};
    const frt::expected<void, double> dest{std::move(src)};

    REQUIRE(dest.error() == expectedDoubleValue);
  }

  SECTION("copy assignment (value to error) (has_value)")
  {
    const ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = e1;

    REQUIRE(e2.has_value());
  }

  SECTION("copy assignment (value to value) (has_value)")
  {
    const ExpectedVoid e1;
    ExpectedVoid e2;
    e2 = e1;

    REQUIRE(e2.has_value());
  }

  SECTION("copy assignment (error to value) (has_value)")
  {
    const ExpectedVoid e1{frt::unexpected{expectedError}};
    ExpectedVoid e2;
    e2 = e1;

    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("copy assignment (error to value) (error)")
  {
    const ExpectedVoid e1{frt::unexpected{expectedError}};
    ExpectedVoid e2;
    e2 = e1;

    REQUIRE(e2.error() == expectedError);
  }

  SECTION("copy assignment (error to error) (error)")
  {
    const ExpectedVoid e1{frt::unexpected{expectedError}};
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = e1;

    REQUIRE(e2.error() == expectedError);
  }

  SECTION("move assignment (value to error) (has_value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = std::move(e1);

    REQUIRE(e2.has_value());
  }

  SECTION("move assignment (value to value) (has_value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2;
    e2 = std::move(e1);

    REQUIRE(e2.has_value());
  }

  SECTION("move assignment (error to value) (has_value)")
  {
    ExpectedVoid e1{frt::unexpected{expectedError}};
    ExpectedVoid e2;
    e2 = std::move(e1);

    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("move assignment (error to value) (error)")
  {
    ExpectedVoid e1{frt::unexpected{expectedError}};
    ExpectedVoid e2;
    e2 = std::move(e1);

    REQUIRE(e2.error() == expectedError);
  }

  SECTION("move assignment (error to error) (error)")
  {
    ExpectedVoid e1{frt::unexpected{expectedError}};
    ExpectedVoid e2{frt::unexpected{""s}};
    e2 = std::move(e1);

    REQUIRE(e2.error() == expectedError);
  }
}
