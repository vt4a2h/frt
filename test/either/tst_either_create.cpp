#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

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

  SECTION("in_place_left (Args...) (is_left)")
  {
    Either either{frt::in_place_left, expectedLeftValue};

    REQUIRE(either.is_left());
  }

  SECTION("in_place_left (Args...) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("in_place_left (initializer_list, Args...) (is_left)")
  {
    using EitherFoo = frt::either<TestArgsIl, std::string>;
    EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.is_left());
  }

  SECTION("in_place_left (initializer_list, Args...) (value)")
  {
    using EitherFoo = frt::either<TestArgsIl, std::string>;
    EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("in_place_right (Args...) (is_right)")
  {
    Either either{frt::in_place_right, expectedRightValue};

    REQUIRE(either.is_right());
  }

  SECTION("in_place_right (Args...) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("in_place_right (initializer_list, Args...) (is_right)")
  {
    using EitherFoo = frt::either<std::string, TestArgsIl>;
    EitherFoo either{frt::in_place_right, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.is_right());
  }

  SECTION("in_place_right (initializer_list, Args...) (value)")
  {
    using EitherFoo = frt::either<std::string, TestArgsIl>;
    EitherFoo either{frt::in_place_right, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.right() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("copy constructor (left) (is_left)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{either};

    REQUIRE(other.is_left());
  }

  SECTION("copy constructor (left) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{either};

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("copy constructor (right) (is_right)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{either};

    REQUIRE(other.is_right());
  }

  SECTION("copy constructor (right) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{either};

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("move constructor (left) (is_left)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{std::move(either)};

    REQUIRE(other.is_left());
  }

  SECTION("move constructor (left) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{std::move(either)};

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("move constructor (right) (is_right)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{std::move(either)};

    REQUIRE(other.is_right());
  }

  SECTION("move constructor (right) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{std::move(either)};

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("copy assignment (left to left) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{frt::in_place_left, std::vector<int>{}};
    other = either;

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("copy assignment (left to right) (is_left)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{frt::in_place_right, ""s};
    other = either;

    REQUIRE(other.is_left());
  }

  SECTION("copy assignment (left to right) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{frt::in_place_right, ""s};
    other = either;

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("copy assignment (right to left) (is_right)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{frt::in_place_left, std::vector<int>{}};
    other = either;

    REQUIRE(other.is_right());
  }

  SECTION("copy assignment (right to left) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{frt::in_place_left, std::vector<int>{}};
    other = either;

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("copy assignment (right to right) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{frt::in_place_right, ""s};
    other = either;

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("move assignment (left to left) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{frt::in_place_left, std::vector<int>{}};
    other = std::move(either);

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("move assignment (left to right) (is_left)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{frt::in_place_right, ""s};
    other = std::move(either);

    REQUIRE(other.is_left());
  }

  SECTION("move assignment (left to right) (value)")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    Either other{frt::in_place_right, ""s};
    other = std::move(either);

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("move assignment (right to left) (is_right)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{frt::in_place_left, std::vector<int>{}};
    other = std::move(either);

    REQUIRE(other.is_right());
  }

  SECTION("move assignment (right to left) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{frt::in_place_left, std::vector<int>{}};
    other = std::move(either);

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("move assignment (right to right) (value)")
  {
    Either either{frt::in_place_right, expectedRightValue};
    Either other{frt::in_place_right, ""s};
    other = std::move(either);

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("either<T, T> (left)")
  {
    using EitherTT = frt::either<int, int>;
    EitherTT either{frt::in_place_left, 42};
    REQUIRE(either.is_left());
  }

  SECTION("either<T, T> (right)")
  {
    using EitherTT = frt::either<int, int>;
    EitherTT either{frt::in_place_right, 42};
    REQUIRE(either.is_right());
  }
}

TEST_CASE("Create either<void, T>", "[either]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using EitherVoidLeft = frt::either<void, std::string>;

  const std::string expectedRightValue =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
      "incididunt ut labore et dolore magna aliqua";

  SECTION("in_place_left (is_left)")
  {
    EitherVoidLeft either{frt::in_place_left};

    REQUIRE(either.is_left());
  }

  SECTION("in_place_right (is_right)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};

    REQUIRE(either.is_right());
  }

  SECTION("in_place_right (value)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};

    REQUIRE(either.right() == expectedRightValue);
  }

  SECTION("copy constructor (left) (is_left)")
  {
    EitherVoidLeft either{frt::in_place_left};
    EitherVoidLeft other{either};

    REQUIRE(other.is_left());
  }

  SECTION("copy constructor (right) (is_right)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{either};

    REQUIRE(other.is_right());
  }

  SECTION("copy constructor (right) (value)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{either};

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("move constructor (left) (is_left)")
  {
    EitherVoidLeft either{frt::in_place_left};
    EitherVoidLeft other{std::move(either)};

    REQUIRE(other.is_left());
  }

  SECTION("move constructor (right) (is_right)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{std::move(either)};

    REQUIRE(other.is_right());
  }

  SECTION("move constructor (right) (value)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{std::move(either)};

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("copy assignment (left to right) (is_left)")
  {
    EitherVoidLeft either{frt::in_place_left};
    EitherVoidLeft other{frt::in_place_right, ""s};
    other = either;

    REQUIRE(other.is_left());
  }

  SECTION("copy assignment (left to left) (is_left)")
  {
    EitherVoidLeft either{frt::in_place_left};
    EitherVoidLeft other{frt::in_place_left};
    other = either;

    REQUIRE(other.is_left());
  }

  SECTION("copy assignment (right to right) (value)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{frt::in_place_right, ""s};
    other = either;

    REQUIRE(other.right() == expectedRightValue);
  }

  SECTION("move assignment (right to left) (is_right)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{frt::in_place_left};
    other = std::move(either);

    REQUIRE(other.is_right());
  }

  SECTION("move assignment (left to left) (is_left)")
  {
    EitherVoidLeft either{frt::in_place_left};
    EitherVoidLeft other{frt::in_place_left};
    other = std::move(either);

    REQUIRE(other.is_left());
  }

  SECTION("move assignment (right to right) (value)")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    EitherVoidLeft other{frt::in_place_right, ""s};
    other = std::move(either);

    REQUIRE(other.right() == expectedRightValue);
  }
}

TEST_CASE("Create either<T, void>", "[either]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using EitherVoidRight = frt::either<std::vector<int>, void>;

  const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  SECTION("in_place_left (is_left)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};

    REQUIRE(either.is_left());
  }

  SECTION("in_place_left (value)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};

    REQUIRE(either.left() == expectedLeftValue);
  }

  SECTION("in_place_left (initializer_list, Args...) (value)")
  {
    using EitherFoo = frt::either<TestArgsIl, void>;
    EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};

    REQUIRE(either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }

  SECTION("in_place_right (is_right)")
  {
    EitherVoidRight either{frt::in_place_right};

    REQUIRE(either.is_right());
  }

  SECTION("copy constructor (left) (is_left)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{either};

    REQUIRE(other.is_left());
  }

  SECTION("copy constructor (left) (value)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{either};

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("copy constructor (right) (is_right)")
  {
    EitherVoidRight either{frt::in_place_right};
    EitherVoidRight other{either};

    REQUIRE(other.is_right());
  }

  SECTION("move constructor (left) (is_left)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{std::move(either)};

    REQUIRE(other.is_left());
  }

  SECTION("move constructor (left) (value)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{std::move(either)};

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("move constructor (right) (is_right)")
  {
    EitherVoidRight either{frt::in_place_right};
    EitherVoidRight other{std::move(either)};

    REQUIRE(other.is_right());
  }

  SECTION("copy assignment (right to left) (is_right)")
  {
    EitherVoidRight either{frt::in_place_right};
    EitherVoidRight other{frt::in_place_left, std::vector<int>{}};
    other = either;

    REQUIRE(other.is_right());
  }

  SECTION("copy assignment (left to left) (value)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{frt::in_place_left, std::vector<int>{}};
    other = either;

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("copy assignment (right to right) (is_right)")
  {
    EitherVoidRight either{frt::in_place_right};
    EitherVoidRight other{frt::in_place_right};
    other = either;

    REQUIRE(other.is_right());
  }

  SECTION("move assignment (left to right) (is_left)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{frt::in_place_right};
    other = std::move(either);

    REQUIRE(other.is_left());
  }

  SECTION("move assignment (left to left) (value)")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    EitherVoidRight other{frt::in_place_left, std::vector<int>{}};
    other = std::move(either);

    REQUIRE(other.left() == expectedLeftValue);
  }

  SECTION("move assignment (right to right) (is_right)")
  {
    EitherVoidRight either{frt::in_place_right};
    EitherVoidRight other{frt::in_place_right};
    other = std::move(either);

    REQUIRE(other.is_right());
  }
}
