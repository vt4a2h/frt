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

TEST_CASE("and_then_right", "[either]")
{
  auto transform = [](const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  auto transformToVoid = [](const std::vector<int> &) -> frt::either<std::string, void> {
    return {frt::in_place_right};
  };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("right to different right type without extra args (is_right)")
  {
    auto res = right.and_then_right(transform);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type without extra args (value)")
  {
    auto res = right.and_then_right(transform);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to void right without extra args (is_right)")
  {
    auto res = right.and_then_right(transformToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (is_right)")
  {
    auto res = right.and_then_right(transformExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (value)")
  {
    auto res = right.and_then_right(transformExtra, extraArg);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to different right type with unwrapped in extra args (is_right)")
  {
    auto res = right.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with unwrapped in extra args (value)")
  {
    auto res = right.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right with non-const lvalue forwarding")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto fn = [](std::vector<int> &v) -> frt::either<std::string, bool> {
      v.clear();
      return {frt::in_place_right, true};
    };
    [[maybe_unused]] auto res = either.and_then_right(fn);
    REQUIRE(either.right().empty());
  }

  SECTION("right with const lvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto fn = [](const std::vector<int> &v) -> frt::either<std::string, bool> {
      return {frt::in_place_right, v == expectedRightValue};
    };
    auto res = either.and_then_right(fn);
    REQUIRE(res.right());
  }

  SECTION("right with rvalue forwarding")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto fn = [](std::vector<int> &&v) -> frt::either<std::string, std::vector<int>> {
      return {frt::in_place_right, std::move(v)};
    };
    auto res = std::move(either).and_then_right(fn);
    REQUIRE(res.right() == expectedRightValue);
  }

  SECTION("left without extra args (is_left)")
  {
    auto res = left.and_then_right(transform);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    auto res = left.and_then_right(transform);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    auto res = left.and_then_right(transformToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (is_left)")
  {
    auto res = left.and_then_right(transformExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (value)")
  {
    auto res = left.and_then_right(transformExtra, extraArg);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("left with unwrapped in extra args (is_left)")
  {
    auto res = left.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.is_left());
  }

  SECTION("left with unwrapped in extra args (value)")
  {
    auto res = left.and_then_right(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.left() == expectedLeftValue);
  }
}

TEST_CASE("and_then_right (void right)", "[either]")
{
  auto transformVoidRight = []() -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  auto transformVoidToVoid = []() -> frt::either<std::string, void> { return {frt::in_place_right}; };
  auto transformVoidExtra = [](int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  auto transformVoidToVoidExtra = [](int) -> frt::either<std::string, void> { return {frt::in_place_right}; };

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("void right to non-void right without extra args (is_right)")
  {
    auto res = right.and_then_right(transformVoidRight);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right without extra args (value)")
  {
    auto res = right.and_then_right(transformVoidRight);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right without extra args (is_right)")
  {
    auto res = right.and_then_right(transformVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (is_right)")
  {
    auto res = right.and_then_right(transformVoidExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (value)")
  {
    auto res = right.and_then_right(transformVoidExtra, extraArg);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right with extra args (is_right)")
  {
    auto res = right.and_then_right(transformVoidToVoidExtra, extraArg);
    REQUIRE(res.is_right());
  }

  SECTION("left without extra args (is_left)")
  {
    auto res = left.and_then_right(transformVoidRight);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    auto res = left.and_then_right(transformVoidRight);
    REQUIRE(res.left() == expectedLeftValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    auto res = left.and_then_right(transformVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (is_left)")
  {
    auto res = left.and_then_right(transformVoidExtra, extraArg);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (value)")
  {
    auto res = left.and_then_right(transformVoidExtra, extraArg);
    REQUIRE(res.left() == expectedLeftValue);
  }
}

TEST_CASE("and_then_right (void left)", "[either]")
{
  auto transformVoidLeft = [](const std::vector<int> &) -> frt::either<void, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  auto transformVoidVoidToNonVoid = []() -> frt::either<void, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  auto transformVoidVoidToVoid = []() -> frt::either<void, void> { return {frt::in_place_right}; };

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};
  const EitherVoidVoid voidVoidLeft{frt::in_place_left};
  const EitherVoidVoid voidVoidRight{frt::in_place_right};

  SECTION("non-void right with void left (is_right)")
  {
    auto res = right.and_then_right(transformVoidLeft);
    REQUIRE(res.is_right());
  }

  SECTION("non-void right with void left (value)")
  {
    auto res = right.and_then_right(transformVoidLeft);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void left from non-void right containing left (is_left)")
  {
    auto res = left.and_then_right(transformVoidLeft);
    REQUIRE(res.is_left());
  }

  SECTION("void right with void left to non-void right (is_right)")
  {
    auto res = voidVoidRight.and_then_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right with void left to non-void right (value)")
  {
    auto res = voidVoidRight.and_then_right(transformVoidVoidToNonVoid);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right with void left to void right (is_right)")
  {
    auto res = voidVoidRight.and_then_right(transformVoidVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void left from void right containing left (is_left)")
  {
    auto res = voidVoidLeft.and_then_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_left());
  }
}
