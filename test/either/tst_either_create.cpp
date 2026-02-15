#include <catch2/catch_all.hpp>

#include <string>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

TEST_CASE("Create", "[either]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::string expectedRightValue =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
      "incididunt ut labore et dolore magna aliqua";

  SECTION("Copy ctor")
  {
    const Either expected{frt::create_left, expectedLeftValue};

    const Either actual{expected};

    REQUIRE(actual == expected);
  }

  SECTION("Copy operator")
  {
    const Either expected{frt::create_left, expectedLeftValue};

    Either actual{frt::create_left, std::vector<int>{}};
    actual = expected;

    REQUIRE(actual == expected);
  }

  SECTION("Copy operator (different type)")
  {
    const Either expected{frt::create_left, expectedLeftValue};

    Either actual{frt::create_right, std::string{}};
    actual = expected;

    REQUIRE(actual == expected);
  }

  SECTION("Copy operator (different type -- right)")
  {
    const Either expected{frt::create_right, expectedRightValue};

    Either actual{frt::create_left, std::vector<int>{}};
    actual = expected;

    REQUIRE(actual == expected);
  }

  SECTION("Move ctor")
  {
    const Either expected{frt::create_left, expectedLeftValue};

    auto tmp = expected;

    const Either actual{std::move(tmp)};

    REQUIRE(actual == expected);
  }

  SECTION("Move operator")
  {
    const Either expected{frt::create_left, expectedLeftValue};
    auto tmp = expected;

    Either actual{frt::create_left, std::vector<int>{}};
    actual = std::move(tmp);

    REQUIRE(actual == expected);
  }

  SECTION("Move operator (different type)")
  {
    const Either expected{frt::create_left, expectedLeftValue};
    auto tmp = expected;

    Either actual{frt::create_right, std::string{}};
    actual = std::move(tmp);

    REQUIRE(actual == expected);
  }

  SECTION("Move operator (different type -- right)")
  {
    const Either expected{frt::create_right, expectedRightValue};
    auto tmp = expected;

    Either actual{frt::create_left, std::vector<int>{}};
    actual = std::move(tmp);

    REQUIRE(actual == expected);
  }

  SECTION("From left (const)")
  {
    const Either either{frt::create_left, expectedLeftValue};

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("From left (non const)")
  {
    auto tmp = expectedLeftValue;
    const Either either{frt::create_left, tmp};

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("From left (move)")
  {
    auto tmp = expectedLeftValue;
    const Either either{frt::create_left, std::move(tmp)};

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("From right (const)")
  {
    const Either either{frt::create_right, expectedRightValue};

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("From right (non const)")
  {
    auto tmp = expectedRightValue;
    const Either either{frt::create_right, tmp};

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("From right (move)")
  {
    auto tmp = expectedRightValue;
    const Either either{frt::create_right, std::move(tmp)};

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("In-place left (Args...)")
  {
    using EitherFoo = frt::either<TestArgsIl, std::string>;
    const EitherFoo either{frt::in_place_left, 1, 2.0};

    REQUIRE(either.left() == TestArgsIl{1, 2.0});
  }

  SECTION("In-place left (initializer_list, Args...)")
  {
    using EitherFoo = frt::either<TestArgsIl, std::string>;
    const EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("In-place right (Args...)")
  {
    using EitherFoo = frt::either<std::string, TestArgsIl>;
    const EitherFoo either{frt::in_place_right, 1, 2.0};

    REQUIRE(either.right() == TestArgsIl{1, 2.0});
  }

  SECTION("In-place right (initializer_list, Args...)")
  {
    using EitherFoo = frt::either<std::string, TestArgsIl>;
    const EitherFoo either{frt::in_place_right, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.right() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }
}
