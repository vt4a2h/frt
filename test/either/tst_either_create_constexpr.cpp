#include <catch2/catch_all.hpp>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

constexpr auto makeExpectedLeft() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedRight()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

TEST_CASE("Create constexpr", "[either]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  SECTION("Copy ctor")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_left, makeExpectedLeft()};
      const Either actual{expected};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Copy operator")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_left, makeExpectedLeft()};
      Either actual{frt::create_left, std::vector<int>{}};
      actual = expected;
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Copy operator (different type)")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_left, makeExpectedLeft()};
      Either actual{frt::create_right, std::string{}};
      actual = expected;
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Copy operator (different type -- right)")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_right, makeExpectedRight()};
      Either actual{frt::create_left, std::vector<int>{}};
      actual = expected;
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Move ctor")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_left, makeExpectedLeft()};
      auto tmp = expected;
      const Either actual{std::move(tmp)};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Move operator")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_left, makeExpectedLeft()};
      auto tmp = expected;
      Either actual{frt::create_left, std::vector<int>{}};
      actual = std::move(tmp);
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Move operator (different type)")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_left, makeExpectedLeft()};
      auto tmp = expected;
      Either actual{frt::create_right, std::string{}};
      actual = std::move(tmp);
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Move operator (different type -- right)")
  {
    constexpr auto valid = [] {
      const Either expected{frt::create_right, makeExpectedRight()};
      auto tmp = expected;
      Either actual{frt::create_left, std::vector<int>{}};
      actual = std::move(tmp);
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("From left (const)")
  {
    constexpr auto valid = [] {
      const auto expectedLeft = makeExpectedLeft();
      Either either{frt::create_left, expectedLeft};
      return either.left() == expectedLeft;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("From left (non const)")
  {
    constexpr auto valid = [] {
      auto expectedLeft = makeExpectedLeft();
      Either either{frt::create_left, expectedLeft};
      return either.left() == expectedLeft;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("From left (move)")
  {
    constexpr auto valid = [] {
      auto tmp = makeExpectedLeft();
      Either either{frt::create_left, std::move(tmp)};
      return either.left() == makeExpectedLeft();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("From right (const)")
  {
    constexpr auto valid = [] {
      const auto expectedRight = makeExpectedRight();
      Either either{frt::create_right, expectedRight};
      return either.right() == expectedRight;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("From right (non const)")
  {
    constexpr auto valid = [] {
      auto expectedRight = makeExpectedRight();
      Either either{frt::create_right, expectedRight};
      return either.right() == expectedRight;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("From right (move)")
  {
    constexpr auto valid = [] {
      auto tmp = makeExpectedRight();
      Either either{frt::create_right, std::move(tmp)};
      return either.right() == makeExpectedRight();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("In-place left (Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, std::string>;
      const EitherFoo either{frt::in_place_left, 1, 2.0};
      return either.left() == TestArgsIl{1, 2.0};
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("In-place left (initializer_list, Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, std::string>;
      const EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};
      return either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("In-place right (Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<std::string, TestArgsIl>;
      const EitherFoo either{frt::in_place_right, 1, 2.0};
      return either.right() == TestArgsIl{1, 2.0};
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("In-place right (initializer_list, Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<std::string, TestArgsIl>;
      const EitherFoo either{frt::in_place_right, {"a"s, "b"s}, 1, 2.0};
      return either.right() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();
    STATIC_REQUIRE(valid);
  }
}
