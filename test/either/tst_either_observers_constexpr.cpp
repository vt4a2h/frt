#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::vector<int>, std::string>;

constexpr auto makeExpectedLeft() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; };
constexpr auto makeExpectedRight()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
};

TEST_CASE("Observers (is_*) constexpr", "[either][constexpr]")
{
  SECTION("is_left")
  {
    SECTION("returns true for left")
    {
      constexpr auto valid = [] {
        const Either left{frt::in_place_left, makeExpectedLeft()};
        return left.is_left();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for right")
    {
      constexpr auto valid = [] {
        const Either right{frt::in_place_right, makeExpectedRight()};
        return !right.is_left();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("is_right")
  {
    SECTION("returns false for left")
    {
      constexpr auto valid = [] {
        const Either left{frt::in_place_left, makeExpectedLeft()};
        return !left.is_right();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns true for right")
    {
      constexpr auto valid = [] {
        const Either right{frt::in_place_right, makeExpectedRight()};
        return right.is_right();
      }();
      STATIC_REQUIRE(valid);
    }
  }
}

TEST_CASE("Observers (left/right) constexpr", "[either][constexpr]")
{
  SECTION("left")
  {
    SECTION("returns reference to left (const)")
    {
      constexpr auto valid = [] {
        const Either either{frt::in_place_left, makeExpectedLeft()};
        return either.left() == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to left (non-const)")
    {
      constexpr auto valid = [] {
        Either either{frt::in_place_left, makeExpectedLeft()};
        return either.left() == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          Either either{frt::in_place_left, makeExpectedLeft()};
          either.left().clear();
          return either.left().empty();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to left (rvalue)")
    {
      constexpr auto valid = [] {
        Either either{frt::in_place_left, makeExpectedLeft()};
        return std::move(either).left() == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("right")
  {
    SECTION("returns reference to right (const)")
    {
      constexpr auto valid = [] {
        const Either either{frt::in_place_right, makeExpectedRight()};
        return either.right() == makeExpectedRight();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to right (non-const)")
    {
      constexpr auto valid = [] {
        Either either{frt::in_place_right, makeExpectedRight()};
        return either.right() == makeExpectedRight();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          Either either{frt::in_place_right, makeExpectedRight()};
          either.right().clear();
          return either.right().empty();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to right (rvalue)")
    {
      constexpr auto valid = [] {
        Either either{frt::in_place_right, makeExpectedRight()};
        return std::move(either).right() == makeExpectedRight();
      }();
      STATIC_REQUIRE(valid);
    }
  }
}

TEST_CASE("Observers (*_or) constexpr", "[either][constexpr]")
{
  SECTION("left_or")
  {
    constexpr auto makeDefaultLeft = [] { return std::vector{10, 11, 12}; };

    SECTION("returns left value if present (const &)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        const Either either{frt::in_place_left, makeExpectedLeft()};
        return either.left_or(makeDefaultLeft()) == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if right value present (const &)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        const Either either{frt::in_place_right, makeExpectedRight()};
        return either.left_or(makeDefaultLeft()) == makeDefaultLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns left value if present (&&)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        Either either{frt::in_place_left, makeExpectedLeft()};
        return std::move(either).left_or(makeDefaultLeft()) == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if right value present (&&)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        Either either{frt::in_place_right, makeExpectedRight()};
        return std::move(either).left_or(makeDefaultLeft()) == makeDefaultLeft();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("right_or")
  {
    constexpr auto makeDefaultRight = [] { return std::string{"Default string value"}; };

    SECTION("returns right value if present (const &)")
    {
      constexpr auto valid = [makeDefaultRight] {
        const Either either{frt::in_place_right, makeExpectedRight()};
        return either.right_or(makeDefaultRight()) == makeExpectedRight();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if left value present (const &)")
    {
      constexpr auto valid = [makeDefaultRight] {
        const Either either{frt::in_place_left, makeExpectedLeft()};
        return either.right_or(makeDefaultRight()) == makeDefaultRight();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns right value if present (&&)")
    {
      constexpr auto valid = [makeDefaultRight] {
        Either either{frt::in_place_right, makeExpectedRight()};
        return std::move(either).right_or(makeDefaultRight()) == makeExpectedRight();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if left value present (&&)")
    {
      constexpr auto valid = [makeDefaultRight] {
        Either either{frt::in_place_left, makeExpectedLeft()};
        return std::move(either).right_or(makeDefaultRight()) == makeDefaultRight();
      }();
      STATIC_REQUIRE(valid);
    }
  }
}
