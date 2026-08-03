#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::vector<int>, std::string>;
using EitherVoidLeft = frt::either<void, std::string>;
using EitherVoidRight = frt::either<std::vector<int>, void>;

inline static const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::string expectedRightValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";

TEST_CASE("Observers (is_*)", "[either]")
{
  const Either left{frt::in_place_left, expectedLeftValue};
  const EitherVoidLeft voidLeftLeft{frt::in_place_left};
  const EitherVoidLeft voidLeftRight{frt::in_place_right};
  const EitherVoidRight voidRightLeft{frt::in_place_left};
  const EitherVoidRight voidRightRight{frt::in_place_right};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("is_left")
  {
    SECTION("returns true for left") { REQUIRE(left.is_left()); }
    SECTION("returns false for right") { REQUIRE_FALSE(right.is_left()); }
    SECTION("returns true for void left left") { REQUIRE(voidLeftLeft.is_left()); }
    SECTION("returns true for void right left") { REQUIRE(voidRightLeft.is_left()); }
    SECTION("returns false for void left right") { REQUIRE_FALSE(voidLeftRight.is_left()); }
    SECTION("returns false for void right right") { REQUIRE_FALSE(voidRightRight.is_left()); }
  }

  SECTION("is_right")
  {
    SECTION("returns false for left") { REQUIRE_FALSE(left.is_right()); }
    SECTION("returns true for right") { REQUIRE(right.is_right()); }
    SECTION("returns false for void left left") { REQUIRE_FALSE(voidLeftLeft.is_right()); }
    SECTION("returns false for void right left") { REQUIRE_FALSE(voidRightLeft.is_right()); }
    SECTION("returns true for void left right") { REQUIRE(voidLeftRight.is_right()); }
    SECTION("returns true for void right right") { REQUIRE(voidRightRight.is_right()); }
  }
}

template <class T>
concept LeftInvocable = requires(T &&t) { t.left(); };

template <class T>
concept RightInvocable = requires(T &&t) { t.right(); };

TEST_CASE("Observers (left/right)", "[either]")
{
  SECTION("left")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight voidRight{frt::in_place_left, expectedLeftValue};

    SECTION("returns reference to left (const)")
    {
      const auto &constEither = either;
      REQUIRE(constEither.left() == expectedLeftValue);
    }

    SECTION("returns reference to left for EitherVoidRight (const)")
    {
      const auto &constEither = voidRight;
      REQUIRE(constEither.left() == expectedLeftValue);
    }

    SECTION("returns reference to left (non-const)")
    {
      REQUIRE(either.left() == expectedLeftValue);

      SECTION("allows modification")
      {
        auto &val = either.left();
        val.clear();
        REQUIRE(either.left().empty());
      }
    }

    SECTION("returns reference to left for EitherVoidRight (non-const)")
    {
      REQUIRE(voidRight.left() == expectedLeftValue);

      SECTION("allows modification")
      {
        auto &val = voidRight.left();
        val.clear();
        REQUIRE(voidRight.left().empty());
      }
    }

    SECTION("returns reference to left (rvalue)") { REQUIRE(std::move(either).left() == expectedLeftValue); }
    SECTION("returns reference to left for EitherVoidRight (rvalue)")
    {
      REQUIRE(std::move(voidRight).left() == expectedLeftValue);
    }
    SECTION("non-invokable for left void")
    {
      STATIC_REQUIRE_FALSE(LeftInvocable<EitherVoidLeft &>);
      STATIC_REQUIRE_FALSE(LeftInvocable<const EitherVoidLeft &>);
      STATIC_REQUIRE_FALSE(LeftInvocable<EitherVoidLeft &&>);
      STATIC_REQUIRE_FALSE(LeftInvocable<const EitherVoidLeft &&>);

      STATIC_REQUIRE(LeftInvocable<EitherVoidRight &>);
      STATIC_REQUIRE(LeftInvocable<const EitherVoidRight &>);
      STATIC_REQUIRE(LeftInvocable<EitherVoidRight &&>);
      STATIC_REQUIRE(LeftInvocable<const EitherVoidRight &&>);
    }
  }

  SECTION("right")
  {
    const std::string modifiedRightValue = "Modified string value";

    Either either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft voidLeft{frt::in_place_right, expectedRightValue};

    SECTION("returns reference to right (const)")
    {
      const auto &constEither = either;
      REQUIRE(constEither.right() == expectedRightValue);
    }

    SECTION("returns reference to right for EitherVoidLeft (const)")
    {
      const auto &constEither = voidLeft;
      REQUIRE(constEither.right() == expectedRightValue);
    }

    SECTION("returns reference to right (non-const)")
    {
      REQUIRE(either.right() == expectedRightValue);

      SECTION("allows modification")
      {
        auto &val = either.right();
        val = modifiedRightValue;
        REQUIRE(either.right() == modifiedRightValue);
      }
    }

    SECTION("returns reference to right for EitherVoidLeft (non-const)")
    {
      REQUIRE(voidLeft.right() == expectedRightValue);

      SECTION("allows modification")
      {
        auto &val = voidLeft.right();
        val = modifiedRightValue;
        REQUIRE(voidLeft.right() == modifiedRightValue);
      }
    }

    SECTION("returns reference to right (rvalue)") { REQUIRE(std::move(either).right() == expectedRightValue); }
    SECTION("returns reference to right for EitherVoidLeft (rvalue)")
    {
      REQUIRE(std::move(voidLeft).right() == expectedRightValue);
    }

    SECTION("non-invokable for right void")
    {
      STATIC_REQUIRE_FALSE(RightInvocable<EitherVoidRight &>);
      STATIC_REQUIRE_FALSE(RightInvocable<const EitherVoidRight &>);
      STATIC_REQUIRE_FALSE(RightInvocable<EitherVoidRight &&>);
      STATIC_REQUIRE_FALSE(RightInvocable<const EitherVoidRight &&>);

      STATIC_REQUIRE(RightInvocable<EitherVoidLeft &>);
      STATIC_REQUIRE(RightInvocable<const EitherVoidLeft &>);
      STATIC_REQUIRE(RightInvocable<EitherVoidLeft &&>);
      STATIC_REQUIRE(RightInvocable<const EitherVoidLeft &&>);
    }
  }
}

template <class T>
concept LeftOrInvocable = requires(T &&t) { t.left_or({}); };

template <class T>
concept RightOrInvocable = requires(T &&t) { t.right_or({}); };

TEST_CASE("Observers (*_or)", "[either]")
{
  SECTION("left_or")
  {
    const std::vector defaultLeftValue{10, 11, 12};

    SECTION("returns left value if present (const &)")
    {
      const Either either{frt::in_place_left, expectedLeftValue};
      REQUIRE(either.left_or(defaultLeftValue) == expectedLeftValue);
    }

    SECTION("returns left value for EitherVoidRight if present (const &)")
    {
      const EitherVoidRight either{frt::in_place_left, expectedLeftValue};
      REQUIRE(either.left_or(defaultLeftValue) == expectedLeftValue);
    }

    SECTION("returns default value if right value present (const &)")
    {
      const Either either{frt::in_place_right, expectedRightValue};
      REQUIRE(either.left_or(defaultLeftValue) == defaultLeftValue);
    }

    SECTION("returns default value for EitherVoidRight if right value present (const &)")
    {
      const EitherVoidRight either{frt::in_place_right};
      REQUIRE(either.left_or(defaultLeftValue) == defaultLeftValue);
    }

    SECTION("returns left value if present (&&)")
    {
      Either either{frt::in_place_left, expectedLeftValue};
      REQUIRE(std::move(either).left_or(defaultLeftValue) == expectedLeftValue);
    }

    SECTION("returns left value for EitherVoidRight if present (&&)")
    {
      EitherVoidRight either{frt::in_place_left, expectedLeftValue};
      REQUIRE(std::move(either).left_or(defaultLeftValue) == expectedLeftValue);
    }

    SECTION("returns default value if right value present (&&)")
    {
      Either either{frt::in_place_right, expectedRightValue};
      REQUIRE(std::move(either).left_or(defaultLeftValue) == defaultLeftValue);
    }

    SECTION("returns default value for EitherVoidRight if right value present (&&)")
    {
      EitherVoidRight either{frt::in_place_right};
      REQUIRE(std::move(either).left_or(defaultLeftValue) == defaultLeftValue);
    }

    SECTION("non-invokable for left void")
    {
      STATIC_REQUIRE_FALSE(LeftOrInvocable<EitherVoidLeft &>);
      STATIC_REQUIRE_FALSE(LeftOrInvocable<const EitherVoidLeft &>);
      STATIC_REQUIRE_FALSE(LeftOrInvocable<EitherVoidLeft &&>);
      STATIC_REQUIRE_FALSE(LeftOrInvocable<const EitherVoidLeft &&>);

      STATIC_REQUIRE(LeftOrInvocable<EitherVoidRight &>);
      STATIC_REQUIRE(LeftOrInvocable<const EitherVoidRight &>);
      STATIC_REQUIRE(LeftOrInvocable<EitherVoidRight &&>);
      STATIC_REQUIRE(LeftOrInvocable<const EitherVoidRight &&>);
    }
  }

  SECTION("right_or")
  {
    const std::string defaultRightValue = "Default string value";

    SECTION("returns right value if present (const &)")
    {
      const Either either{frt::in_place_right, expectedRightValue};
      REQUIRE(either.right_or(defaultRightValue) == expectedRightValue);
    }

    SECTION("returns right value for EitherVoidLeft if present (const &)")
    {
      const EitherVoidLeft either{frt::in_place_right, expectedRightValue};
      REQUIRE(either.right_or(defaultRightValue) == expectedRightValue);
    }

    SECTION("returns default value if left value present (const &)")
    {
      const Either either{frt::in_place_left, expectedLeftValue};
      REQUIRE(either.right_or(defaultRightValue) == defaultRightValue);
    }

    SECTION("returns default value for EitherVoidLeft if left value present (const &)")
    {
      const EitherVoidLeft either{frt::in_place_left};
      REQUIRE(either.right_or(defaultRightValue) == defaultRightValue);
    }

    SECTION("returns right value if present (&&)")
    {
      Either either{frt::in_place_right, expectedRightValue};
      REQUIRE(std::move(either).right_or(defaultRightValue) == expectedRightValue);
    }

    SECTION("returns right value for EitherVoidLeft if present (&&)")
    {
      EitherVoidLeft either{frt::in_place_right, expectedRightValue};
      REQUIRE(std::move(either).right_or(defaultRightValue) == expectedRightValue);
    }

    SECTION("returns default value if left value present (&&)")
    {
      Either either{frt::in_place_left, expectedLeftValue};
      REQUIRE(std::move(either).right_or(defaultRightValue) == defaultRightValue);
    }

    SECTION("returns default value for EitherVoidLeft if left value present (&&)")
    {
      EitherVoidLeft either{frt::in_place_left};
      REQUIRE(std::move(either).right_or(defaultRightValue) == defaultRightValue);
    }

    SECTION("non-invokable for right void")
    {
      STATIC_REQUIRE_FALSE(RightOrInvocable<EitherVoidRight &>);
      STATIC_REQUIRE_FALSE(RightOrInvocable<const EitherVoidRight &>);
      STATIC_REQUIRE_FALSE(RightOrInvocable<EitherVoidRight &&>);
      STATIC_REQUIRE_FALSE(RightOrInvocable<const EitherVoidRight &&>);

      STATIC_REQUIRE(RightOrInvocable<EitherVoidLeft &>);
      STATIC_REQUIRE(RightOrInvocable<const EitherVoidLeft &>);
      STATIC_REQUIRE(RightOrInvocable<EitherVoidLeft &&>);
      STATIC_REQUIRE(RightOrInvocable<const EitherVoidLeft &&>);
    }
  }
}
