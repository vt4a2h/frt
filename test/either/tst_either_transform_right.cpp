#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

using Either = frt::either<std::string, std::vector<int>>;
using EitherVoidRight = frt::either<std::string, void>;
using EitherVoidLeft = frt::either<void, std::vector<int>>;
using EitherVoidVoid = frt::either<void, void>;

inline static const std::string expectedLeftValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline static const std::vector expectedRightValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::string expectedTransformedValue =
    "consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";
inline static constexpr int extraArg = 42;

TEST_CASE("transform_right", "[either]")
{
  auto transform = [](const std::vector<int> &) -> std::string { return expectedTransformedValue; };
  auto transformToVoid = [](const std::vector<int> &) -> void {};
  auto transformExtra = [](const std::vector<int> &, int) -> std::string { return expectedTransformedValue; };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::string { return expectedTransformedValue; };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("right to different right type without extra args (is_right)")
  {
    auto res = right.transform_right(transform);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type without extra args (value)")
  {
    auto res = right.transform_right(transform);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to void right without extra args (is_right)")
  {
    auto res = right.transform_right(transformToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (is_right)")
  {
    auto res = right.transform_right(transformExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (value)")
  {
    auto res = right.transform_right(transformExtra, extraArg);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to different right type with unwrapped in extra args (is_right)")
  {
    auto res = right.transform_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with unwrapped in extra args (value)")
  {
    auto res = right.transform_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right with non-const lvalue forwarding")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto fn = [](std::vector<int> &v) -> bool {
      v.clear();
      return true;
    };
    [[maybe_unused]] auto res = either.transform_right(fn);
    REQUIRE(either.right().empty());
  }

  SECTION("right with const lvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto fn = [](const std::vector<int> &v) -> bool { return v == expectedRightValue; };
    auto res = either.transform_right(fn);
    REQUIRE(res.right());
  }

  SECTION("right with rvalue forwarding")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
    auto res = std::move(either).transform_right(fn);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("left without extra args (is_left)")
  {
    auto res = left.transform_right(transform);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    auto res = left.transform_right(transform);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    auto res = left.transform_right(transformToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (is_left)")
  {
    auto res = left.transform_right(transformExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (value)")
  {
    auto res = left.transform_right(transformExtra, extraArg);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("left with unwrapped in extra args (is_left)")
  {
    auto res = left.transform_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_left());
  }

  SECTION("left with unwrapped in extra args (value)")
  {
    auto res = left.transform_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.left() == expectedLeftValue);
  }
}

TEST_CASE("transform_right (void right)", "[either]")
{
  auto transformVoidRight = []() -> std::string { return expectedTransformedValue; };
  auto transformVoidToVoid = []() -> void {};
  auto transformVoidExtra = [](int) -> std::string { return expectedTransformedValue; };
  auto transformVoidToVoidExtra = [](int) -> void {};

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("void right to non-void right without extra args (is_right)")
  {
    auto res = right.transform_right(transformVoidRight);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right without extra args (value)")
  {
    auto res = right.transform_right(transformVoidRight);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right without extra args (is_right)")
  {
    auto res = right.transform_right(transformVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (is_right)")
  {
    auto res = right.transform_right(transformVoidExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (value)")
  {
    auto res = right.transform_right(transformVoidExtra, extraArg);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right with extra args (is_right)")
  {
    auto res = right.transform_right(transformVoidToVoidExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("left without extra args (is_left)")
  {
    auto res = left.transform_right(transformVoidRight);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    auto res = left.transform_right(transformVoidRight);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    auto res = left.transform_right(transformVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (is_left)")
  {
    auto res = left.transform_right(transformVoidExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (value)")
  {
    auto res = left.transform_right(transformVoidExtra, extraArg);
    REQUIRE(res.left() == expectedLeftValue);
  }
}

TEST_CASE("transform_right (void left)", "[either]")
{
  auto transformVoidLeft = [](const std::vector<int> &) -> std::string { return expectedTransformedValue; };
  auto transformVoidVoidToNonVoid = []() -> std::string { return expectedTransformedValue; };
  auto transformVoidVoidToVoid = []() -> void {};

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};
  const EitherVoidVoid voidVoidLeft{frt::in_place_left};
  const EitherVoidVoid voidVoidRight{frt::in_place_right};

  SECTION("non-void right with void left (is_right)")
  {
    auto res = right.transform_right(transformVoidLeft);
    REQUIRE(res.is_right());
  }

  SECTION("non-void right with void left (value)")
  {
    auto res = right.transform_right(transformVoidLeft);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void left from non-void right containing left (is_left)")
  {
    auto res = left.transform_right(transformVoidLeft);
    REQUIRE(res.is_left());
  }

  SECTION("void right with void left to non-void right (is_right)")
  {
    auto res = voidVoidRight.transform_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right with void left to non-void right (value)")
  {
    auto res = voidVoidRight.transform_right(transformVoidVoidToNonVoid);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right with void left to void right (is_right)")
  {
    auto res = voidVoidRight.transform_right(transformVoidVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void left from void right containing left (is_left)")
  {
    auto res = voidVoidLeft.transform_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_left());
  }
}
