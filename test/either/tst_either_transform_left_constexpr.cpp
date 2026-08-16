#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::vector<int>, std::string>;
using EitherVoidLeft = frt::either<void, std::string>;
using EitherVoidRight = frt::either<std::vector<int>, void>;
using EitherVoidVoid = frt::either<void, void>;

constexpr auto makeExpectedLeftValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedRightValue()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}
constexpr auto makeExpectedTransformedValue()
{
  return std::string{"consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}
inline static constexpr int extraArg = 42;

TEST_CASE("transform_left constexpr", "[either][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> std::string { return makeExpectedTransformedValue(); };
  auto transformToVoid = [](const std::vector<int> &) -> void {};
  auto transformExtra = [](const std::vector<int> &, int) -> std::string { return makeExpectedTransformedValue(); };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::string { return makeExpectedTransformedValue(); };

  SECTION("left to different left type without extra args (is_left)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transform);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transform);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to void left without extra args (is_left)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with extra args without unwrapped (is_left)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformExtra, extraArg);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformExtra, extraArg);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with unwrapped in extra args (is_left)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      auto fn = [](std::vector<int> &v) -> bool {
        v.clear();
        return true;
      };
      [[maybe_unused]] auto res = either.transform_left(fn);
      return either.left().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_left, makeExpectedLeftValue()};
      auto fn = [](const std::vector<int> &v) -> bool { return v == makeExpectedLeftValue(); };
      auto res = either.transform_left(fn);
      return res.left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
      auto res = std::move(either).transform_left(fn);
      return res.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (is_right)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transform);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transform);
      return res.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args without unwrapped (is_right)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformExtra, extraArg);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformExtra, extraArg);
      return res.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with unwrapped in extra args (is_right)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
      return res.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("transform_left (void left) constexpr", "[either][constexpr]")
{
  auto transformVoidLeft = []() -> std::string { return makeExpectedTransformedValue(); };
  auto transformVoidToVoid = []() -> void {};
  auto transformVoidExtra = [](int) -> std::string { return makeExpectedTransformedValue(); };
  auto transformVoidToVoidExtra = [](int) -> void {};

  SECTION("void left to non-void left without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.transform_left(transformVoidLeft);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to non-void left without extra args (value)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.transform_left(transformVoidLeft);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to void left without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.transform_left(transformVoidToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to non-void left with extra args (is_left)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.transform_left(transformVoidExtra, extraArg);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to non-void left with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.transform_left(transformVoidExtra, extraArg);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to void left with extra args (is_left)")
  {
    constexpr auto valid = [transformVoidToVoidExtra] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.transform_left(transformVoidToVoidExtra, extraArg);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformVoidLeft);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (value)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformVoidLeft);
      return res.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformVoidToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args (is_right)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformVoidExtra, extraArg);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.transform_left(transformVoidExtra, extraArg);
      return res.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("transform_left (void right) constexpr", "[either][constexpr]")
{
  auto transformVoidRight = [](const std::vector<int> &) -> std::string { return makeExpectedTransformedValue(); };
  auto transformVoidVoidToNonVoid = []() -> std::string { return makeExpectedTransformedValue(); };
  auto transformVoidVoidToVoid = []() -> void {};

  SECTION("non-void left with void right (is_left)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformVoidRight);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("non-void left with void right (value)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.transform_left(transformVoidRight);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right from non-void left containing right (is_right)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.transform_left(transformVoidRight);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left with void right to non-void left (is_left)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid left{frt::in_place_left};
      auto res = left.transform_left(transformVoidVoidToNonVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left with void right to non-void left (value)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid left{frt::in_place_left};
      auto res = left.transform_left(transformVoidVoidToNonVoid);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left with void right to void left (is_left)")
  {
    constexpr auto valid = [transformVoidVoidToVoid] {
      const EitherVoidVoid left{frt::in_place_left};
      auto res = left.transform_left(transformVoidVoidToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right from void left containing right (is_right)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      auto res = right.transform_left(transformVoidVoidToNonVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }
}
