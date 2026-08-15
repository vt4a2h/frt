#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::string, std::vector<int>>;
using EitherVoidRight = frt::either<std::string, void>;
using EitherVoidLeft = frt::either<void, std::vector<int>>;
using EitherVoidVoid = frt::either<void, void>;

constexpr auto makeExpectedLeftValue()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}
constexpr auto makeExpectedRightValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedTransformedValue()
{
  return std::string{"consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}
inline static constexpr int extraArg = 42;

TEST_CASE("and_then_right constexpr", "[either][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  auto transformToVoid = [](const std::vector<int> &) -> frt::either<std::string, void> {
    return {frt::in_place_right};
  };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };

  SECTION("right to different right type without extra args (is_right)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transform);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transform);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to void right without extra args (is_right)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with extra args without unwrapped (is_right)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformExtra, extraArg);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformExtra, extraArg);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with unwrapped in extra args (is_right)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      auto fn = [](std::vector<int> &v) -> frt::either<std::string, bool> {
        v.clear();
        return {frt::in_place_right, true};
      };
      [[maybe_unused]] auto res = either.and_then_right(fn);
      return either.right().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      auto fn = [](const std::vector<int> &v) -> frt::either<std::string, bool> {
        return {frt::in_place_right, v == makeExpectedRightValue()};
      };
      auto res = either.and_then_right(fn);
      return res.right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      auto fn = [](std::vector<int> &&v) -> frt::either<std::string, std::vector<int>> {
        return {frt::in_place_right, std::move(v)};
      };
      auto res = std::move(either).and_then_right(fn);
      return res.right() == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (is_left)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transform);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transform);
      return res.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args without unwrapped (is_left)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformExtra, extraArg);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformExtra, extraArg);
      return res.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with unwrapped in extra args (is_left)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
      return res.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("and_then_right (void right) constexpr", "[either][constexpr]")
{
  auto transformVoidRight = []() -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  auto transformVoidToVoid = []() -> frt::either<std::string, void> { return {frt::in_place_right}; };
  auto transformVoidExtra = [](int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  auto transformVoidToVoidExtra = [](int) -> frt::either<std::string, void> { return {frt::in_place_right}; };

  SECTION("void right to non-void right without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidRight);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to non-void right without extra args (value)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidRight);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to void right without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to non-void right with extra args (is_right)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidExtra, extraArg);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to non-void right with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidExtra, extraArg);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to void right with extra args (is_right)")
  {
    constexpr auto valid = [transformVoidToVoidExtra] {
      const EitherVoidRight right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidToVoidExtra, extraArg);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformVoidRight);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (value)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformVoidRight);
      return res.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformVoidToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args (is_left)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformVoidExtra, extraArg);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      auto res = left.and_then_right(transformVoidExtra, extraArg);
      return res.left() == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("and_then_right (void left) constexpr", "[either][constexpr]")
{
  auto transformVoidLeft = [](const std::vector<int> &) -> frt::either<void, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  auto transformVoidVoidToNonVoid = []() -> frt::either<void, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  auto transformVoidVoidToVoid = []() -> frt::either<void, void> { return {frt::in_place_right}; };

  SECTION("non-void right with void left (is_right)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformVoidLeft);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("non-void right with void left (value)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      auto res = right.and_then_right(transformVoidLeft);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left from non-void right containing left (is_left)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      auto res = left.and_then_right(transformVoidLeft);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right with void left to non-void right (is_right)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidVoidToNonVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right with void left to non-void right (value)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidVoidToNonVoid);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right with void left to void right (is_right)")
  {
    constexpr auto valid = [transformVoidVoidToVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      auto res = right.and_then_right(transformVoidVoidToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left from void right containing left (is_left)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid left{frt::in_place_left};
      auto res = left.and_then_right(transformVoidVoidToNonVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }
}
