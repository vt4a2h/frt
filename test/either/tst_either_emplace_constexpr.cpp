#include <catch2/catch_all.hpp>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

constexpr auto makeExpectedLeftValue() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedRightValue()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

TEST_CASE("Emplace constexpr", "[either][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  SECTION("emplaceLeft on empty (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector<int>{}};
      either.emplaceLeft(makeExpectedLeftValue());
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft on empty (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector<int>{}};
      either.emplaceLeft(makeExpectedLeftValue());
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft on left (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector{1, 2, 3}};
      either.emplaceLeft(makeExpectedLeftValue());
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft on left (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector{1, 2, 3}};
      either.emplaceLeft(makeExpectedLeftValue());
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft on right (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, "original"s};
      either.emplaceLeft(makeExpectedLeftValue());
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft on right (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, "original"s};
      either.emplaceLeft(makeExpectedLeftValue());
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight on empty (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector<int>{}};
      either.emplaceRight(makeExpectedRightValue());
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight on empty (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector<int>{}};
      either.emplaceRight(makeExpectedRightValue());
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight on left (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector{1, 2, 3}};
      either.emplaceRight(makeExpectedRightValue());
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight on left (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, std::vector{1, 2, 3}};
      either.emplaceRight(makeExpectedRightValue());
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight on right (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, "original"s};
      either.emplaceRight(makeExpectedRightValue());
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight on right (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, "original"s};
      either.emplaceRight(makeExpectedRightValue());
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft (Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, std::string>;
      EitherFoo either{frt::in_place_right, ""s};
      either.emplaceLeft(1, 2.0);
      return either.left() == TestArgsIl{1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceLeft (initializer_list, Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, std::string>;
      EitherFoo either{frt::in_place_right, ""s};
      either.emplaceLeft({"a"s, "b"s}, 1, 2.0);
      return either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight (Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<std::string, TestArgsIl>;
      EitherFoo either{frt::in_place_left, ""s};
      either.emplaceRight(1, 2.0);
      return either.right() == TestArgsIl{1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplaceRight (initializer_list, Args...)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<std::string, TestArgsIl>;
      EitherFoo either{frt::in_place_left, ""s};
      either.emplaceRight({"a"s, "b"s}, 1, 2.0);
      return either.right() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }
}
