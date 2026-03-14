#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::vector<int>, std::string>;

inline static const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::string expectedRightValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";

TEST_CASE("Observers (is_*)", "[either]")
{
  SECTION("is_left")
  {
    const Either left{frt::create_left, expectedLeftValue};
    const Either right{frt::create_right, expectedRightValue};

    SECTION("returns true for left") { REQUIRE(left.is_left()); }
    SECTION("returns false for right") { REQUIRE_FALSE(right.is_left()); }
  }

  SECTION("is_right")
  {
    const Either left{frt::create_left, expectedLeftValue};
    const Either right{frt::create_right, expectedRightValue};

    SECTION("returns false for left") { REQUIRE_FALSE(left.is_right()); }
    SECTION("returns true for right") { REQUIRE(right.is_right()); }
  }
}

TEST_CASE("Observers (left/right)", "[either]")
{
  SECTION("left")
  {
    Either either{frt::create_left, expectedLeftValue};

    SECTION("returns reference to left (const)")
    {
      const auto &constEither = either;
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

    SECTION("returns reference to left (rvalue)") { REQUIRE(std::move(either).left() == expectedLeftValue); }
  }

  SECTION("right")
  {
    const std::string modifiedRightValue = "Modified string value";

    Either either{frt::create_right, expectedRightValue};

    SECTION("returns reference to right (const)")
    {
      const auto &constEither = either;
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

    SECTION("returns reference to right (rvalue)") { REQUIRE(std::move(either).right() == expectedRightValue); }
  }
}

TEST_CASE("Observers (*_or)", "[either]")
{
  SECTION("left_or")
  {
    const std::vector defaultLeftValue{10, 11, 12};

    SECTION("returns left value if present (const &)")
    {
      const Either either{frt::create_left, expectedLeftValue};
      REQUIRE(either.left_or(defaultLeftValue) == expectedLeftValue);
    }

    SECTION("returns default value if right value present (const &)")
    {
      const Either either{frt::create_right, expectedRightValue};
      REQUIRE(either.left_or(defaultLeftValue) == defaultLeftValue);
    }

    SECTION("returns left value if present (&&)")
    {
      Either either{frt::create_left, expectedLeftValue};
      REQUIRE(std::move(either).left_or(defaultLeftValue) == expectedLeftValue);
    }

    SECTION("returns default value if right value present (&&)")
    {
      Either either{frt::create_right, expectedRightValue};
      REQUIRE(std::move(either).left_or(defaultLeftValue) == defaultLeftValue);
    }
  }

  SECTION("right_or")
  {
    const std::string defaultRightValue = "Default string value";

    SECTION("returns right value if present (const &)")
    {
      const Either either{frt::create_right, expectedRightValue};
      REQUIRE(either.right_or(defaultRightValue) == expectedRightValue);
    }

    SECTION("returns default value if left value present (const &)")
    {
      const Either either{frt::create_left, expectedLeftValue};
      REQUIRE(either.right_or(defaultRightValue) == defaultRightValue);
    }

    SECTION("returns right value if present (&&)")
    {
      Either either{frt::create_right, expectedRightValue};
      REQUIRE(std::move(either).right_or(defaultRightValue) == expectedRightValue);
    }

    SECTION("returns default value if left value present (&&)")
    {
      Either either{frt::create_left, expectedLeftValue};
      REQUIRE(std::move(either).right_or(defaultRightValue) == defaultRightValue);
    }
  }
}
