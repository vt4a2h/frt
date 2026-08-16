#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::vector<int>, std::string>;
using EitherVoidLeft = frt::either<void, std::string>;
using EitherVoidRight = frt::either<std::vector<int>, void>;
using EitherVoidVoid = frt::either<void, void>;

inline static const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::string expectedRightValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline static const std::string expectedTransformedValue =
    "consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";
inline static constexpr int extraArg = 42;

TEST_CASE("transform_left", "[either]")
{
  auto transform = [](const std::vector<int> &) -> std::string { return expectedTransformedValue; };
  auto transformToVoid = [](const std::vector<int> &) -> void {};
  auto transformExtra = [](const std::vector<int> &, int) -> std::string { return expectedTransformedValue; };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::string { return expectedTransformedValue; };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("left to different left type without extra args (is_left)")
  {
    auto res = left.transform_left(transform);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type without extra args (value)")
  {
    auto res = left.transform_left(transform);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("left to void left without extra args (is_left)")
  {
    auto res = left.transform_left(transformToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type with extra args without unwrapped (is_left)")
  {
    auto res = left.transform_left(transformExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type with extra args without unwrapped (value)")
  {
    auto res = left.transform_left(transformExtra, extraArg);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("left to different left type with unwrapped in extra args (is_left)")
  {
    auto res = left.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type with unwrapped in extra args (value)")
  {
    auto res = left.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("left with non-const lvalue forwarding")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto fn = [](std::vector<int> &v) -> bool {
      v.clear();
      return true;
    };
    [[maybe_unused]] auto res = either.transform_left(fn);
    REQUIRE(either.left().empty());
  }

  SECTION("left with const lvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto fn = [](const std::vector<int> &v) -> bool { return v == expectedLeftValue; };
    auto res = either.transform_left(fn);
    REQUIRE(res.left());
  }

  SECTION("left with rvalue forwarding")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
    auto res = std::move(either).transform_left(fn);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("right without extra args (is_right)")
  {
    auto res = right.transform_left(transform);
    REQUIRE(res.is_right());
  }

  SECTION("right without extra args (value)")
  {
    auto res = right.transform_left(transform);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    auto res = right.transform_left(transformToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args without unwrapped (is_right)")
  {
    auto res = right.transform_left(transformExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args without unwrapped (value)")
  {
    auto res = right.transform_left(transformExtra, extraArg);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("right with unwrapped in extra args (is_right)")
  {
    auto res = right.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_right());
  }

  SECTION("right with unwrapped in extra args (value)")
  {
    auto res = right.transform_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.right() == expectedRightValue);
  }
}

TEST_CASE("transform_left (void left)", "[either]")
{
  auto transformVoidLeft = []() -> std::string { return expectedTransformedValue; };
  auto transformVoidToVoid = []() -> void {};
  auto transformVoidExtra = [](int) -> std::string { return expectedTransformedValue; };
  auto transformVoidToVoidExtra = [](int) -> void {};

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};

  SECTION("void left to non-void left without extra args (is_left)")
  {
    auto res = left.transform_left(transformVoidLeft);
    REQUIRE(res.is_left());
  }

  SECTION("void left to non-void left without extra args (value)")
  {
    auto res = left.transform_left(transformVoidLeft);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void left to void left without extra args (is_left)")
  {
    auto res = left.transform_left(transformVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("void left to non-void left with extra args (is_left)")
  {
    auto res = left.transform_left(transformVoidExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("void left to non-void left with extra args (value)")
  {
    auto res = left.transform_left(transformVoidExtra, extraArg);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void left to void left with extra args (is_left)")
  {
    auto res = left.transform_left(transformVoidToVoidExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("right without extra args (is_right)")
  {
    auto res = right.transform_left(transformVoidLeft);
    REQUIRE(res.is_right());
  }

  SECTION("right without extra args (value)")
  {
    auto res = right.transform_left(transformVoidLeft);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    auto res = right.transform_left(transformVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args (is_right)")
  {
    auto res = right.transform_left(transformVoidExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args (value)")
  {
    auto res = right.transform_left(transformVoidExtra, extraArg);
    REQUIRE(res.right() == expectedRightValue);
  }
}

TEST_CASE("transform_left (void right)", "[either]")
{
  auto transformVoidRight = [](const std::vector<int> &) -> std::string { return expectedTransformedValue; };
  auto transformVoidVoidToNonVoid = []() -> std::string { return expectedTransformedValue; };
  auto transformVoidVoidToVoid = []() -> void {};

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};
  const EitherVoidVoid voidVoidLeft{frt::in_place_left};
  const EitherVoidVoid voidVoidRight{frt::in_place_right};

  SECTION("non-void left with void right (is_left)")
  {
    auto res = left.transform_left(transformVoidRight);
    REQUIRE(res.is_left());
  }

  SECTION("non-void left with void right (value)")
  {
    auto res = left.transform_left(transformVoidRight);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void right from non-void left containing right (is_right)")
  {
    auto res = right.transform_left(transformVoidRight);
    REQUIRE(res.is_right());
  }

  SECTION("void left with void right to non-void left (is_left)")
  {
    auto res = voidVoidLeft.transform_left(transformVoidVoidToNonVoid);
    REQUIRE(res.is_left());
  }

  SECTION("void left with void right to non-void left (value)")
  {
    auto res = voidVoidLeft.transform_left(transformVoidVoidToNonVoid);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void left with void right to void left (is_left)")
  {
    auto res = voidVoidLeft.transform_left(transformVoidVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("void right from void left containing right (is_right)")
  {
    auto res = voidVoidRight.transform_left(transformVoidVoidToNonVoid);
    REQUIRE(res.is_right());
  }
}
