#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

TEST_CASE("Emplace", "[either]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::string expectedRightValue =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
      "incididunt ut labore et dolore magna aliqua";

  SECTION("emplaceLeft on empty (is_left)")
  {
    Either either{frt::create_left, std::vector<int>{}};
    either.emplaceLeft(expectedLeftValue);

    REQUIRE(either.is_left());
  }

  SECTION("emplaceLeft on empty (value)")
  {
    Either either{frt::create_left, std::vector<int>{}};
    either.emplaceLeft(expectedLeftValue);

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("emplaceLeft on left (is_left)")
  {
    Either either{frt::create_left, std::vector{1, 2, 3}};
    either.emplaceLeft(expectedLeftValue);

    REQUIRE(either.is_left());
  }

  SECTION("emplaceLeft on left (value)")
  {
    Either either{frt::create_left, std::vector{1, 2, 3}};
    either.emplaceLeft(expectedLeftValue);

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("emplaceLeft on right (is_left)")
  {
    Either either{frt::create_right, "original"s};
    either.emplaceLeft(expectedLeftValue);

    REQUIRE(either.is_left());
  }

  SECTION("emplaceLeft on right (value)")
  {
    Either either{frt::create_right, "original"s};
    either.emplaceLeft(expectedLeftValue);

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("emplaceRight on empty (is_right)")
  {
    Either either{frt::create_left, std::vector<int>{}};
    either.emplaceRight(expectedRightValue);

    REQUIRE(either.is_right());
  }

  SECTION("emplaceRight on empty (value)")
  {
    Either either{frt::create_left, std::vector<int>{}};
    either.emplaceRight(expectedRightValue);

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("emplaceRight on left (is_right)")
  {
    Either either{frt::create_left, std::vector<int>{1, 2, 3}};
    either.emplaceRight(expectedRightValue);

    REQUIRE(either.is_right());
  }

  SECTION("emplaceRight on left (value)")
  {
    Either either{frt::create_left, std::vector<int>{1, 2, 3}};
    either.emplaceRight(expectedRightValue);

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("emplaceRight on right (is_right)")
  {
    Either either{frt::create_right, "original"s};
    either.emplaceRight(expectedRightValue);

    REQUIRE(either.is_right());
  }

  SECTION("emplaceRight on right (value)")
  {
    Either either{frt::create_right, "original"s};
    either.emplaceRight(expectedRightValue);

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("emplaceLeft (Args...)")
  {
    using EitherFoo = frt::either<TestArgsIl, std::string>;
    EitherFoo either{frt::create_right, ""s};
    either.emplaceLeft(1, 2.0);

    REQUIRE(either.left() == TestArgsIl{1, 2.0});
  }

  SECTION("emplaceLeft (initializer_list, Args...)")
  {
    using EitherFoo = frt::either<TestArgsIl, std::string>;
    EitherFoo either{frt::create_right, ""s};
    either.emplaceLeft({"a"s, "b"s}, 1, 2.0);

    REQUIRE(either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("emplaceRight (Args...)")
  {
    using EitherFoo = frt::either<std::string, TestArgsIl>;
    EitherFoo either{frt::create_left, ""s};
    either.emplaceRight(1, 2.0);

    REQUIRE(either.right() == TestArgsIl{1, 2.0});
  }

  SECTION("emplaceRight (initializer_list, Args...)")
  {
    using EitherFoo = frt::either<std::string, TestArgsIl>;
    EitherFoo either{frt::create_left, ""s};
    either.emplaceRight({"a"s, "b"s}, 1, 2.0);

    REQUIRE(either.right() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }
}
