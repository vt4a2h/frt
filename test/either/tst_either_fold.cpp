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
inline const std::string expectedFoldLeftResult = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
inline const std::string expectedFoldRightResult = "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";

struct NothrowLeftFn
{
  std::string operator()(const std::vector<int> &) const noexcept { return expectedFoldLeftResult; }
};

struct ThrowingLeftFn
{
  std::string operator()(const std::vector<int> &) const noexcept(false) { return expectedFoldLeftResult; }
};

struct NothrowRightFn
{
  std::string operator()(const std::string &) const noexcept { return expectedFoldRightResult; }
};

struct ThrowingRightFn
{
  std::string operator()(const std::string &) const noexcept(false) { return expectedFoldRightResult; }
};

struct NothrowVoidLeftFn
{
  std::string operator()() const noexcept { return expectedFoldLeftResult; }
};

struct ThrowingVoidLeftFn
{
  std::string operator()() const noexcept(false) { return expectedFoldLeftResult; }
};

struct NothrowVoidRightFn
{
  std::string operator()() const noexcept { return expectedFoldRightResult; }
};

struct ThrowingVoidRightFn
{
  std::string operator()() const noexcept(false) { return expectedFoldRightResult; }
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

TEST_CASE("fold", "[either]")
{
  auto foldLeft = [](const std::vector<int> &) -> std::string { return expectedFoldLeftResult; };
  auto foldRight = [](const std::string &) -> std::string { return expectedFoldRightResult; };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("left alternative invokes left function and returns its result")
  {
    REQUIRE(left.fold(foldLeft, foldRight) == expectedFoldLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.fold(foldLeft, [&](const std::string &) {
      rightInvoked = true;
      return expectedFoldRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    REQUIRE(right.fold(foldLeft, foldRight) == expectedFoldRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.fold(
        [&](const std::vector<int> &) {
          leftInvoked = true;
          return expectedFoldLeftResult;
        },
        foldRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.fold([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.fold([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.fold([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.fold([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    [[maybe_unused]] auto res = either.fold(
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
    [[maybe_unused]] auto res = either.fold([](std::vector<int> &) { return 0; },
                                            [](std::string &s) {
                                              s.clear();
                                              return 0;
                                            });
    REQUIRE(either.right().empty());
  }

  SECTION("left with const lvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto res = either.fold([](const std::vector<int> &v) { return v == expectedLeftValue; },
                           [](const std::string &) { return false; });
    REQUIRE(res);
  }

  SECTION("right with const lvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto res = either.fold([](const std::vector<int> &) { return false; },
                           [](const std::string &s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto res = std::move(either).fold([](std::vector<int> &&v) { return std::move(v); },
                                      [](std::string &&) { return std::vector<int>{}; });
    REQUIRE(res == expectedLeftValue);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto res = std::move(either).fold([](std::vector<int> &&) { return std::string{}; },
                                      [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedRightValue);
  }

  SECTION("left with const rvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto res = static_cast<const Either &&>(either).fold(
        [](const std::vector<int> &&v) { return v == expectedLeftValue; }, [](const std::string &&) { return false; });
    REQUIRE(res);
  }

  SECTION("right with const rvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto res = static_cast<const Either &&>(either).fold([](const std::vector<int> &&) { return false; },
                                                         [](const std::string &&s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Either>().fold(NothrowLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().fold(ThrowingLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().fold(NothrowLeftFn{}, ThrowingRightFn{})));
  }

  SECTION("not noexcept when both functions throw")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().fold(ThrowingLeftFn{}, ThrowingRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Either>().fold(NothrowVoidReturnLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().fold(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Either>().fold(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnRightFn{})));
  }
}

TEST_CASE("fold (void left)", "[either]")
{
  auto foldVoidLeft = []() -> std::string { return expectedFoldLeftResult; };
  auto foldRight = [](const std::string &) -> std::string { return expectedFoldRightResult; };

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    REQUIRE(left.fold(foldVoidLeft, foldRight) == expectedFoldLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.fold(foldVoidLeft, [&](const std::string &) {
      rightInvoked = true;
      return expectedFoldRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    REQUIRE(right.fold(foldVoidLeft, foldRight) == expectedFoldRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.fold(
        [&] {
          leftInvoked = true;
          return expectedFoldLeftResult;
        },
        foldRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.fold([&] { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.fold([] {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.fold([] {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.fold([&] { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("right with non-const lvalue forwarding allows modification")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    [[maybe_unused]] auto res = either.fold([] { return 0; },
                                            [](std::string &s) {
                                              s.clear();
                                              return 0;
                                            });
    REQUIRE(either.right().empty());
  }

  SECTION("right with const lvalue forwarding")
  {
    const EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    auto res = either.fold([] { return false; }, [](const std::string &s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    auto res = std::move(either).fold([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedRightValue);
  }

  SECTION("right with const rvalue forwarding")
  {
    const EitherVoidLeft either{frt::in_place_right, expectedRightValue};
    auto res = static_cast<const EitherVoidLeft &&>(either).fold(
        [] { return false; }, [](const std::string &&s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidLeft>().fold(ThrowingVoidLeftFn{}, NothrowRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidLeftFn{}, ThrowingRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidLeft>().fold(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnRightFn{})));
  }
}

TEST_CASE("fold (void right)", "[either]")
{
  auto foldLeft = [](const std::vector<int> &) -> std::string { return expectedFoldLeftResult; };
  auto foldVoidRight = []() -> std::string { return expectedFoldRightResult; };

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("left alternative invokes left function and returns its result")
  {
    REQUIRE(left.fold(foldLeft, foldVoidRight) == expectedFoldLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.fold(foldLeft, [&] {
      rightInvoked = true;
      return expectedFoldRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    REQUIRE(right.fold(foldLeft, foldVoidRight) == expectedFoldRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.fold(
        [&](const std::vector<int> &) {
          leftInvoked = true;
          return expectedFoldLeftResult;
        },
        foldVoidRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.fold([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.fold([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.fold([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.fold([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    [[maybe_unused]] auto res = either.fold(
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
    auto res = either.fold([](const std::vector<int> &v) { return v == expectedLeftValue; }, [] { return false; });
    REQUIRE(res);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    auto res =
        std::move(either).fold([](std::vector<int> &&v) { return std::move(v); }, [] { return std::vector<int>{}; });
    REQUIRE(res == expectedLeftValue);
  }

  SECTION("left with const rvalue forwarding")
  {
    const EitherVoidRight either{frt::in_place_left, expectedLeftValue};
    auto res = static_cast<const EitherVoidRight &&>(either).fold(
        [](const std::vector<int> &&v) { return v == expectedLeftValue; }, [] { return false; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidRight>().fold(NothrowLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidRight>().fold(ThrowingLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidRight>().fold(NothrowLeftFn{}, ThrowingVoidRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidRight>().fold(NothrowVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidRight>().fold(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidRight>().fold(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnVoidRightFn{})));
  }
}

TEST_CASE("fold (void void)", "[either]")
{
  auto foldVoidLeft = []() -> std::string { return expectedFoldLeftResult; };
  auto foldVoidRight = []() -> std::string { return expectedFoldRightResult; };

  const EitherVoidVoid left{frt::in_place_left};
  const EitherVoidVoid right{frt::in_place_right};

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    REQUIRE(left.fold(foldVoidLeft, foldVoidRight) == expectedFoldLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left.fold(foldVoidLeft, [&] {
      rightInvoked = true;
      return expectedFoldRightResult;
    });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    REQUIRE(right.fold(foldVoidLeft, foldVoidRight) == expectedFoldRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right.fold(
        [&] {
          leftInvoked = true;
          return expectedFoldLeftResult;
        },
        foldVoidRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left.fold([&] { leftInvoked = true; }, [] {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left.fold([] {}, [&] { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right.fold([] {}, [&] { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right.fold([&] { leftInvoked = true; }, [] {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when left function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidVoid>().fold(ThrowingVoidLeftFn{}, NothrowVoidRightFn{})));
  }

  SECTION("not noexcept when right function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidLeftFn{}, ThrowingVoidRightFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(
        noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when left function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidVoid>().fold(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{})));
  }

  SECTION("void return not noexcept when right function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnVoidRightFn{})));
  }
}
