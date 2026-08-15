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

TEST_CASE("and_then_left", "[either]")
{
  auto transform = [](const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_left, expectedTransformedValue};
  };
  auto transformToVoid = [](const std::vector<int> &) -> frt::either<void, std::string> {
    return {frt::in_place_left};
  };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::either<std::string, std::string> {
    return {frt::in_place_left, expectedTransformedValue};
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_left, expectedTransformedValue};
  };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("left to different left type without extra args (is_left)")
  {
    auto res = left.and_then_left(transform);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type without extra args (value)")
  {
    auto res = left.and_then_left(transform);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("left to void left without extra args (is_left)")
  {
    auto res = left.and_then_left(transformToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type with extra args without unwrapped (is_left)")
  {
    auto res = left.and_then_left(transformExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type with extra args without unwrapped (value)")
  {
    auto res = left.and_then_left(transformExtra, extraArg);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("left to different left type with unwrapped in extra args (is_left)")
  {
    auto res = left.and_then_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_left());
  }

  SECTION("left to different left type with unwrapped in extra args (value)")
  {
    auto res = left.and_then_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("left with non-const lvalue forwarding")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto fn = [](std::vector<int> &v) -> frt::either<bool, std::string> {
      v.clear();
      return {frt::in_place_left, true};
    };
    [[maybe_unused]] auto res = either.and_then_left(fn);
    REQUIRE(either.left().empty());
  }

  SECTION("left with const lvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto fn = [](const std::vector<int> &v) -> frt::either<bool, std::string> {
      return {frt::in_place_left, v == expectedLeftValue};
    };
    auto res = either.and_then_left(fn);
    REQUIRE(res.left());
  }

  SECTION("left with rvalue forwarding")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto fn = [](std::vector<int> &&v) -> frt::either<std::vector<int>, std::string> {
      return {frt::in_place_left, std::move(v)};
    };
    auto res = std::move(either).and_then_left(fn);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("right without extra args (is_right)")
  {
    auto res = right.and_then_left(transform);
    REQUIRE(res.is_right());
  }

  SECTION("right without extra args (value)")
  {
    auto res = right.and_then_left(transform);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    auto res = right.and_then_left(transformToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args without unwrapped (is_right)")
  {
    auto res = right.and_then_left(transformExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args without unwrapped (value)")
  {
    auto res = right.and_then_left(transformExtra, extraArg);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("right with unwrapped in extra args (is_right)")
  {
    auto res = right.and_then_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_right());
  }

  SECTION("right with unwrapped in extra args (value)")
  {
    auto res = right.and_then_left(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.right() == expectedRightValue);
  }
}

TEST_CASE("and_then_left (void left)", "[either]")
{
  auto transformVoidLeft = []() -> frt::either<std::string, std::string> {
    return {frt::in_place_left, expectedTransformedValue};
  };
  auto transformVoidToVoid = []() -> frt::either<void, std::string> { return {frt::in_place_left}; };
  auto transformVoidExtra = [](int) -> frt::either<std::string, std::string> {
    return {frt::in_place_left, expectedTransformedValue};
  };
  auto transformVoidToVoidExtra = [](int) -> frt::either<void, std::string> { return {frt::in_place_left}; };

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};

  SECTION("void left to non-void left without extra args (is_left)")
  {
    auto res = left.and_then_left(transformVoidLeft);
    REQUIRE(res.is_left());
  }

  SECTION("void left to non-void left without extra args (value)")
  {
    auto res = left.and_then_left(transformVoidLeft);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void left to void left without extra args (is_left)")
  {
    auto res = left.and_then_left(transformVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("void left to non-void left with extra args (is_left)")
  {
    auto res = left.and_then_left(transformVoidExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("void left to non-void left with extra args (value)")
  {
    auto res = left.and_then_left(transformVoidExtra, extraArg);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void left to void left with extra args (is_left)")
  {
    auto res = left.and_then_left(transformVoidToVoidExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("right without extra args (is_right)")
  {
    auto res = right.and_then_left(transformVoidLeft);
    REQUIRE(res.is_right());
  }

  SECTION("right without extra args (value)")
  {
    auto res = right.and_then_left(transformVoidLeft);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    auto res = right.and_then_left(transformVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args (is_right)")
  {
    auto res = right.and_then_left(transformVoidExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("right with extra args (value)")
  {
    auto res = right.and_then_left(transformVoidExtra, extraArg);
    REQUIRE(res.right() == expectedRightValue);
  }
}

TEST_CASE("and_then_left (void right)", "[either]")
{
  auto transformVoidRight = [](const std::vector<int> &) -> frt::either<std::string, void> {
    return {frt::in_place_left, expectedTransformedValue};
  };
  auto transformVoidVoidToNonVoid = []() -> frt::either<std::string, void> {
    return {frt::in_place_left, expectedTransformedValue};
  };
  auto transformVoidVoidToVoid = []() -> frt::either<void, void> { return {frt::in_place_left}; };

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};
  const EitherVoidVoid voidVoidLeft{frt::in_place_left};
  const EitherVoidVoid voidVoidRight{frt::in_place_right};

  SECTION("non-void left with void right (is_left)")
  {
    auto res = left.and_then_left(transformVoidRight);
    REQUIRE(res.is_left());
  }

  SECTION("non-void left with void right (value)")
  {
    auto res = left.and_then_left(transformVoidRight);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void right from non-void left containing right (is_right)")
  {
    auto res = right.and_then_left(transformVoidRight);
    REQUIRE(res.is_right());
  }

  SECTION("void left with void right to non-void left (is_left)")
  {
    auto res = voidVoidLeft.and_then_left(transformVoidVoidToNonVoid);
    REQUIRE(res.is_left());
  }

  SECTION("void left with void right to non-void left (value)")
  {
    auto res = voidVoidLeft.and_then_left(transformVoidVoidToNonVoid);
    REQUIRE(res.left() == expectedTransformedValue);
  }

  SECTION("void left with void right to void left (is_left)")
  {
    auto res = voidVoidLeft.and_then_left(transformVoidVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("void right from void left containing right (is_right)")
  {
    auto res = voidVoidRight.and_then_left(transformVoidVoidToNonVoid);
    REQUIRE(res.is_right());
  }
}
