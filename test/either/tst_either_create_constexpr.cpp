#include <catch2/catch_all.hpp>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

constexpr auto makeExpectedLeftValue() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedRightValue()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

TEST_CASE("Create constexpr", "[either][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  SECTION("in_place_left (Args...) (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_left (Args...) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_left (initializer_list, Args...) (is_left)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, std::string>;
      EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_left (initializer_list, Args...) (value)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, std::string>;
      EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};
      return either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (Args...) (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (Args...) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (initializer_list, Args...) (is_right)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<std::string, TestArgsIl>;
      EitherFoo either{frt::in_place_right, {"a"s, "b"s}, 1, 2.0};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (initializer_list, Args...) (value)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<std::string, TestArgsIl>;
      EitherFoo either{frt::in_place_right, {"a"s, "b"s}, 1, 2.0};
      return either.right() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (left) (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{either};
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (left) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{either};
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (right) (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{either};
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (right) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{either};
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (left) (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{std::move(either)};
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (left) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{std::move(either)};
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (right) (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{std::move(either)};
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (right) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{std::move(either)};
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (left to left) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{frt::in_place_left, std::vector<int>{}};
      other = either;
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (left to right) (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{frt::in_place_right, ""s};
      other = either;
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (left to right) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{frt::in_place_right, ""s};
      other = either;
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (right to left) (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{frt::in_place_left, std::vector<int>{}};
      other = either;
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (right to left) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{frt::in_place_left, std::vector<int>{}};
      other = either;
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (right to right) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{frt::in_place_right, ""s};
      other = either;
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (left to left) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{frt::in_place_left, std::vector<int>{}};
      other = std::move(either);
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (left to right) (is_left)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{frt::in_place_right, ""s};
      other = std::move(either);
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (left to right) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      Either other{frt::in_place_right, ""s};
      other = std::move(either);
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (right to left) (is_right)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{frt::in_place_left, std::vector<int>{}};
      other = std::move(either);
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (right to left) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{frt::in_place_left, std::vector<int>{}};
      other = std::move(either);
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (right to right) (value)")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      Either other{frt::in_place_right, ""s};
      other = std::move(either);
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("either<T, T> (left)")
  {
    constexpr auto valid = [] {
      using EitherTT = frt::either<int, int>;
      EitherTT either{frt::in_place_left, 42};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("either<T, T> (right)")
  {
    constexpr auto valid = [] {
      using EitherTT = frt::either<int, int>;
      EitherTT either{frt::in_place_right, 42};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }
  SECTION("converting copy constructor from another either (left) (is_left)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      const AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      const Either either{src};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor from another either (left) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      const AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      const Either either{src};
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor from another either (right) (is_right)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      const AnotherEither src{frt::in_place_right, expectedRightCStr};
      const Either either{src};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting copy constructor from another either (right) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      const AnotherEither src{frt::in_place_right, expectedRightCStr};
      const Either either{src};
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor from another either (left) (is_left)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      const Either either{std::move(src)};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor from another either (left) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      const Either either{std::move(src)};
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor from another either (right) (is_right)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      AnotherEither src{frt::in_place_right, expectedRightCStr};
      const Either either{std::move(src)};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("converting move constructor from another either (right) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      AnotherEither src{frt::in_place_right, expectedRightCStr};
      const Either either{std::move(src)};
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment from another either (left to left) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      const AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      Either either{frt::in_place_left, std::vector<int>{}};
      either = src;
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment from another either (left to right) (is_left)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      const AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      Either either{frt::in_place_right, ""s};
      either = src;
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment from another either (left to right) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      const AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      Either either{frt::in_place_right, ""s};
      either = src;
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment from another either (right to left) (is_right)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      const AnotherEither src{frt::in_place_right, expectedRightCStr};
      Either either{frt::in_place_left, std::vector<int>{}};
      either = src;
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment from another either (right to left) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      const AnotherEither src{frt::in_place_right, expectedRightCStr};
      Either either{frt::in_place_left, std::vector<int>{}};
      either = src;
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment from another either (right to right) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      const AnotherEither src{frt::in_place_right, expectedRightCStr};
      Either either{frt::in_place_right, ""s};
      either = src;
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment from another either (left to left) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      Either either{frt::in_place_left, std::vector<int>{}};
      either = std::move(src);
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment from another either (left to right) (is_left)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      Either either{frt::in_place_right, ""s};
      either = std::move(src);
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment from another either (left to right) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      AnotherEither src{frt::in_place_left, makeExpectedLeftValue()};
      Either either{frt::in_place_right, ""s};
      either = std::move(src);
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment from another either (right to left) (is_right)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      AnotherEither src{frt::in_place_right, expectedRightCStr};
      Either either{frt::in_place_left, std::vector<int>{}};
      either = std::move(src);
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment from another either (right to left) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      AnotherEither src{frt::in_place_right, expectedRightCStr};
      Either either{frt::in_place_left, std::vector<int>{}};
      either = std::move(src);
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment from another either (right to right) (value)")
  {
    constexpr auto valid = [] {
      using AnotherEither = frt::either<std::vector<int>, const char *>;
      constexpr const char *expectedRightCStr =
          "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
          "incididunt ut labore et dolore magna aliqua";
      AnotherEither src{frt::in_place_right, expectedRightCStr};
      Either either{frt::in_place_right, ""s};
      either = std::move(src);
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Create either<void, T> constexpr", "[either][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using EitherVoidLeft = frt::either<void, std::string>;

  SECTION("in_place_left (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_left};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (value)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      return either.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (left) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_left};
      EitherVoidLeft other{either};
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (right) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{either};
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (right) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{either};
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (left) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_left};
      EitherVoidLeft other{std::move(either)};
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (right) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{std::move(either)};
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (right) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{std::move(either)};
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (left to right) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_left};
      EitherVoidLeft other{frt::in_place_right, ""s};
      other = either;
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (left to left) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_left};
      EitherVoidLeft other{frt::in_place_left};
      other = either;
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (right to right) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{frt::in_place_right, ""s};
      other = either;
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (right to left) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{frt::in_place_left};
      other = std::move(either);
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (left to left) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_left};
      EitherVoidLeft other{frt::in_place_left};
      other = std::move(either);
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (right to right) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      EitherVoidLeft other{frt::in_place_right, ""s};
      other = std::move(either);
      return other.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Create either<T, void> constexpr", "[either][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using EitherVoidRight = frt::either<std::vector<int>, void>;

  SECTION("in_place_left (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      return either.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_left (value)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      return either.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_left (initializer_list, Args...) (value)")
  {
    constexpr auto valid = [] {
      using EitherFoo = frt::either<TestArgsIl, void>;
      EitherFoo either{frt::in_place_left, {"a"s, "b"s}, 1, 2.0};
      return either.left() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("in_place_right (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_right};
      return either.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (left) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{either};
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (left) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{either};
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (right) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_right};
      EitherVoidRight other{either};
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (left) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{std::move(either)};
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (left) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{std::move(either)};
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (right) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_right};
      EitherVoidRight other{std::move(either)};
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (right to left) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_right};
      EitherVoidRight other{frt::in_place_left, std::vector<int>{}};
      other = either;
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (left to left) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{frt::in_place_left, std::vector<int>{}};
      other = either;
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (right to right) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_right};
      EitherVoidRight other{frt::in_place_right};
      other = either;
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (left to right) (is_left)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{frt::in_place_right};
      other = std::move(either);
      return other.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (left to left) (value)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      EitherVoidRight other{frt::in_place_left, std::vector<int>{}};
      other = std::move(either);
      return other.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (right to right) (is_right)")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_right};
      EitherVoidRight other{frt::in_place_right};
      other = std::move(either);
      return other.is_right();
    }();

    STATIC_REQUIRE(valid);
  }
}
