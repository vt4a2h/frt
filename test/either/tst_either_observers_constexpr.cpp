#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::vector<int>, std::string>;
using EitherVoidLeft = frt::either<void, std::string>;
using EitherVoidRight = frt::either<std::vector<int>, void>;

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

    SECTION("returns true for void left left")
    {
      constexpr auto valid = [] {
        const EitherVoidLeft voidLeftLeft{frt::in_place_left};
        return voidLeftLeft.is_left();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns true for void right left")
    {
      constexpr auto valid = [] {
        const EitherVoidRight voidRightLeft{frt::in_place_left, makeExpectedLeft()};
        return voidRightLeft.is_left();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for void left right")
    {
      constexpr auto valid = [] {
        const EitherVoidLeft voidLeftRight{frt::in_place_right, makeExpectedRight()};
        return !voidLeftRight.is_left();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for void right right")
    {
      constexpr auto valid = [] {
        const EitherVoidRight voidRightRight{frt::in_place_right};
        return !voidRightRight.is_left();
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

    SECTION("returns false for void left left")
    {
      constexpr auto valid = [] {
        const EitherVoidLeft voidLeftLeft{frt::in_place_left};
        return !voidLeftLeft.is_right();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for void right left")
    {
      constexpr auto valid = [] {
        const EitherVoidRight voidRightLeft{frt::in_place_left, makeExpectedLeft()};
        return !voidRightLeft.is_right();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns true for void left right")
    {
      constexpr auto valid = [] {
        const EitherVoidLeft voidLeftRight{frt::in_place_right, makeExpectedRight()};
        return voidLeftRight.is_right();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns true for void right right")
    {
      constexpr auto valid = [] {
        const EitherVoidRight voidRightRight{frt::in_place_right};
        return voidRightRight.is_right();
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

    SECTION("returns reference to left for EitherVoidRight (const)")
    {
      constexpr auto valid = [] {
        const EitherVoidRight either{frt::in_place_left, makeExpectedLeft()};
        return either.left() == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to left for EitherVoidRight (non-const)")
    {
      constexpr auto valid = [] {
        EitherVoidRight either{frt::in_place_left, makeExpectedLeft()};
        return either.left() == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          EitherVoidRight either{frt::in_place_left, makeExpectedLeft()};
          either.left().clear();
          return either.left().empty();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to left for EitherVoidRight (rvalue)")
    {
      constexpr auto valid = [] {
        EitherVoidRight either{frt::in_place_left, makeExpectedLeft()};
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

    SECTION("returns reference to right for EitherVoidLeft (const)")
    {
      constexpr auto valid = [] {
        const EitherVoidLeft either{frt::in_place_right, makeExpectedRight()};
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

    SECTION("returns reference to right for EitherVoidLeft (non-const)")
    {
      constexpr auto valid = [] {
        EitherVoidLeft either{frt::in_place_right, makeExpectedRight()};
        return either.right() == makeExpectedRight();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          EitherVoidLeft either{frt::in_place_right, makeExpectedRight()};
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

    SECTION("returns reference to right for EitherVoidLeft (rvalue)")
    {
      constexpr auto valid = [] {
        EitherVoidLeft either{frt::in_place_right, makeExpectedRight()};
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

    SECTION("returns left value for EitherVoidRight if present (const &)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        const EitherVoidRight either{frt::in_place_left, makeExpectedLeft()};
        return either.left_or(makeDefaultLeft()) == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value for EitherVoidRight if right value present (const &)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        const EitherVoidRight either{frt::in_place_right};
        return either.left_or(makeDefaultLeft()) == makeDefaultLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns left value for EitherVoidRight if present (&&)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        EitherVoidRight either{frt::in_place_left, makeExpectedLeft()};
        return std::move(either).left_or(makeDefaultLeft()) == makeExpectedLeft();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value for EitherVoidRight if right value present (&&)")
    {
      constexpr auto valid = [makeDefaultLeft] {
        EitherVoidRight either{frt::in_place_right};
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

    SECTION("returns right value for EitherVoidLeft if present (const &)")
    {
      constexpr auto valid = [makeDefaultRight] {
        const EitherVoidLeft either{frt::in_place_right, makeExpectedRight()};
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

    SECTION("returns default value for EitherVoidLeft if left value present (const &)")
    {
      constexpr auto valid = [makeDefaultRight] {
        const EitherVoidLeft either{frt::in_place_left};
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

    SECTION("returns right value for EitherVoidLeft if present (&&)")
    {
      constexpr auto valid = [makeDefaultRight] {
        EitherVoidLeft either{frt::in_place_right, makeExpectedRight()};
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

    SECTION("returns default value for EitherVoidLeft if left value present (&&)")
    {
      constexpr auto valid = [makeDefaultRight] {
        EitherVoidLeft either{frt::in_place_left};
        return std::move(either).right_or(makeDefaultRight()) == makeDefaultRight();
      }();
      STATIC_REQUIRE(valid);
    }
  }
}
