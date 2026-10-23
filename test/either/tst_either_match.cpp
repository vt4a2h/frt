#include "test/helpers/types.hpp"

#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

namespace
{

using Either = frt::either<std::vector<int>, std::string>;
using EitherVoidLeft = frt::either<void, std::string>;
using EitherVoidRight = frt::either<std::vector<int>, void>;
using EitherVoidVoid = frt::either<void, void>;

inline const std::vector expectedLeftValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline const std::string expectedRightValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline const std::string expectedMatchLeftResult = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
inline const std::string expectedMatchRightResult = "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";

struct NothrowLeftFn
{
  std::string operator()(const std::vector<int> &) const noexcept { return expectedMatchLeftResult; }
};

struct ThrowingLeftFn
{
  std::string operator()(const std::vector<int> &) const noexcept(false) { return expectedMatchLeftResult; }
};

struct NothrowRightFn
{
  std::string operator()(const std::string &) const noexcept { return expectedMatchRightResult; }
};

struct ThrowingRightFn
{
  std::string operator()(const std::string &) const noexcept(false) { return expectedMatchRightResult; }
};

struct NothrowVoidLeftFn
{
  std::string operator()() const noexcept { return expectedMatchLeftResult; }
};

struct ThrowingVoidLeftFn
{
  std::string operator()() const noexcept(false) { return expectedMatchLeftResult; }
};

struct NothrowVoidRightFn
{
  std::string operator()() const noexcept { return expectedMatchRightResult; }
};

struct ThrowingVoidRightFn
{
  std::string operator()() const noexcept(false) { return expectedMatchRightResult; }
};

struct NothrowVoidReturnLeftFn
{
  void operator()(const std::vector<int> &) const noexcept {}
};

struct ThrowingVoidReturnLeftFn
{
  void operator()(const std::vector<int> &) const noexcept(false) {}
};

struct NothrowVoidReturnRightFn
{
  void operator()(const std::string &) const noexcept {}
};

struct ThrowingVoidReturnRightFn
{
  void operator()(const std::string &) const noexcept(false) {}
};

struct NothrowVoidReturnVoidLeftFn
{
  void operator()() const noexcept {}
};

struct ThrowingVoidReturnVoidLeftFn
{
  void operator()() const noexcept(false) {}
};

struct NothrowVoidReturnVoidRightFn
{
  void operator()() const noexcept {}
};

struct ThrowingVoidReturnVoidRightFn
{
  void operator()() const noexcept(false) {}
};

} // namespace

TEST_CASE("match", "[either]")
{
  auto matchLeft = [](const std::vector<int> &) -> std::string { return expectedMatchLeftResult; };
  auto matchRight = [](const std::string &) -> std::string { return expectedMatchRightResult; };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("left alternative invokes left function and returns its result")
  {
    REQUIRE(left.match(matchLeft, matchRight) == expectedMatchLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.match(matchLeft, [&](const std::string &) {
      rightInvoked = true;
      return expectedMatchRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    REQUIRE(right.match(matchLeft, matchRight) == expectedMatchRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.match(
        [&](const std::vector<int> &) {
          leftInvoked = true;
          return expectedMatchLeftResult;
        },
        matchRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.match([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.match([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.match([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.match([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    [[maybe_unused]] auto res = either.match(
        [](std::vector<int> &v) {
          v.clear();
          return 0;
        },
        [](std::string &) { return 0; });
    REQUIRE(either.left().empty());
  }

  SECTION("right with non-const lvalue forwarding allows modification")
  {
    Either either{frt::in_place_right, expectedRightValue};
    [[maybe_unused]] auto res = either.match([](std::vector<int> &) { return 0; },
                                             [](std::string &s) {
                                               s.clear();
                                               return 0;
                                             });
    REQUIRE(either.right().empty());
  }

  SECTION("left with const lvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto res = either.match([](const std::vector<int> &v) { return v == expectedLeftValue; },
                            [](const std::string &) { return false; });
    REQUIRE(res);
  }

  SECTION("right with const lvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto res = either.match([](const std::vector<int> &) { return false; },
                            [](const std::string &s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto res = std::move(either).match([](std::vector<int> &&v) { return std::move(v); },
                                       [](std::string &&) { return std::vector<int>{}; });
    REQUIRE(res == expectedLeftValue);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto res = std::move(either).match([](std::vector<int> &&) { return std::string{}; },
                                       [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedRightValue);
  }

  SECTION("left with const rvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto res = static_cast<const Either &&>(either).match(
        [](const std::vector<int> &&v) { return v == expectedLeftValue; }, [](const std::string &&) { return false; });
    REQUIRE(res);
  }

  SECTION("right with const rvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto res = static_cast<const Either &&>(either).match(
        [](const std::vector<int> &&) { return false; }, [](const std::string &&s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Either>().match(NothrowLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().match(ThrowingLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().match(NothrowLeftFn{}, ThrowingRightFn{})));
  }

  SECTION("not noexcept when both functions throw")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().match(ThrowingLeftFn{}, ThrowingRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Either>().match(NothrowVoidReturnLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().match(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().match(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnRightFn{})));
  }
}

TEST_CASE("match (void left)", "[either]")
{
  auto matchVoidLeft = []() -> std::string { return expectedMatchLeftResult; };
  auto matchRight = [](const std::string &) -> std::string { return expectedMatchRightResult; };

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    REQUIRE(left.match(matchVoidLeft, matchRight) == expectedMatchLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.match(matchVoidLeft, [&](const std::string &) {
      rightInvoked = true;
      return expectedMatchRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    REQUIRE(right.match(matchVoidLeft, matchRight) == expectedMatchRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.match(
        [&] {
          leftInvoked = true;
          return expectedMatchLeftResult;
        },
        matchRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.match([&] { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.match([] {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.match([] {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.match([&] { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("right with non-const lvalue forwarding allows modification")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    [[maybe_unused]] auto res = either.match([] { return 0; },
                                             [](std::string &s) {
                                               s.clear();
                                               return 0;
                                             });
    REQUIRE(either.right().empty());
  }

  SECTION("right with const lvalue forwarding")
  {
    const EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    auto res = either.match([] { return false; }, [](const std::string &s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    auto res = std::move(either).match([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedRightValue);
  }

  SECTION("right with const rvalue forwarding")
  {
    const EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    auto res = static_cast<const EitherVoidLeft &&>(either).match(
        [] { return false; }, [](const std::string &&s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidLeft>().match(ThrowingVoidLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidLeftFn{}, ThrowingRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidLeft>().match(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnRightFn{})));
  }
}

TEST_CASE("match (void right)", "[either]")
{
  auto matchLeft = [](const std::vector<int> &) -> std::string { return expectedMatchLeftResult; };
  auto matchVoidRight = []() -> std::string { return expectedMatchRightResult; };

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("left alternative invokes left function and returns its result")
  {
    REQUIRE(left.match(matchLeft, matchVoidRight) == expectedMatchLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.match(matchLeft, [&] {
      rightInvoked = true;
      return expectedMatchRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    REQUIRE(right.match(matchLeft, matchVoidRight) == expectedMatchRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.match(
        [&](const std::vector<int> &) {
          leftInvoked = true;
          return expectedMatchLeftResult;
        },
        matchVoidRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.match([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.match([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.match([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.match([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    [[maybe_unused]] auto res = either.match(
        [](std::vector<int> &v) {
          v.clear();
          return 0;
        },
        [] { return 0; });
    REQUIRE(either.left().empty());
  }

  SECTION("left with const lvalue forwarding")
  {
    const EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    auto res = either.match([](const std::vector<int> &v) { return v == expectedLeftValue; }, [] { return false; });
    REQUIRE(res);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    auto res =
        std::move(either).match([](std::vector<int> &&v) { return std::move(v); }, [] { return std::vector<int>{}; });
    REQUIRE(res == expectedLeftValue);
  }

  SECTION("left with const rvalue forwarding")
  {
    const EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    auto res = static_cast<const EitherVoidRight &&>(either).match(
        [](const std::vector<int> &&v) { return v == expectedLeftValue; }, [] { return false; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidRight>().match(NothrowLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidRight>().match(ThrowingLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidRight>().match(NothrowLeftFn{}, ThrowingVoidRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidRight>().match(NothrowVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidRight>().match(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidRight>().match(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnVoidRightFn{})));
  }
}

TEST_CASE("match (void void)", "[either]")
{
  auto matchVoidLeft = []() -> std::string { return expectedMatchLeftResult; };
  auto matchVoidRight = []() -> std::string { return expectedMatchRightResult; };

  const EitherVoidVoid left{frt::in_place_left};
  const EitherVoidVoid right{frt::in_place_right};

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    REQUIRE(left.match(matchVoidLeft, matchVoidRight) == expectedMatchLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.match(matchVoidLeft, [&] {
      rightInvoked = true;
      return expectedMatchRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    REQUIRE(right.match(matchVoidLeft, matchVoidRight) == expectedMatchRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.match(
        [&] {
          leftInvoked = true;
          return expectedMatchLeftResult;
        },
        matchVoidRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.match([&] { leftInvoked = true; }, [] {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.match([] {}, [&] { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.match([] {}, [&] { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.match([&] { leftInvoked = true; }, [] {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidVoid>().match(ThrowingVoidLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidLeftFn{}, ThrowingVoidRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(
        noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidVoid>().match(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnVoidRightFn{})));
  }
}

TEST_CASE("Fold with generic function for both alternatives", "[either]")
{
  using namespace test::helpers;

  constexpr auto width = 10;
  constexpr auto height = 20;

  frt::either<Widget, Window> eitherWindow{frt::in_place_right, Window{.width = width, .height = height}};
  frt::either<Widget, Window> eitherWidget{frt::in_place_left, Widget{.width = width, .height = height}};

  SECTION("generic square function")
  {
    auto square = [](const HasWidthAndHeight auto &obj) { return obj.width * obj.height; };

    REQUIRE(eitherWindow.match(square) == eitherWidget.match(square));
  }

  SECTION("generic height modifier")
  {
    constexpr auto scaleFactor = 2;
    auto scaleHeight = [](HasWidthAndHeight auto &obj) { obj.height *= scaleFactor; };

    eitherWindow.match(scaleHeight);
    eitherWidget.match(scaleHeight);

    REQUIRE(eitherWindow.right().height == eitherWidget.left().height);
  }
}
