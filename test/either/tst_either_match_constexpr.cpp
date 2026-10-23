#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

#include <test/helpers/types.hpp>

namespace
{

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
constexpr auto makeExpectedMatchLeftResult()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit"};
}
constexpr auto makeExpectedMatchRightResult()
{
  return std::string{"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}

struct NothrowLeftFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept { return makeExpectedMatchLeftResult(); }
};

struct ThrowingLeftFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept(false)
  {
    return makeExpectedMatchLeftResult();
  }
};

struct NothrowRightFn
{
  constexpr std::string operator()(const std::string &) const noexcept { return makeExpectedMatchRightResult(); }
};

struct ThrowingRightFn
{
  constexpr std::string operator()(const std::string &) const noexcept(false) { return makeExpectedMatchRightResult(); }
};

struct NothrowVoidLeftFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedMatchLeftResult(); }
};

struct ThrowingVoidLeftFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedMatchLeftResult(); }
};

struct NothrowVoidRightFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedMatchRightResult(); }
};

struct ThrowingVoidRightFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedMatchRightResult(); }
};

struct NothrowVoidReturnLeftFn
{
  constexpr void operator()(const std::vector<int> &) const noexcept {}
};

struct ThrowingVoidReturnLeftFn
{
  constexpr void operator()(const std::vector<int> &) const noexcept(false) {}
};

struct NothrowVoidReturnRightFn
{
  constexpr void operator()(const std::string &) const noexcept {}
};

struct ThrowingVoidReturnRightFn
{
  constexpr void operator()(const std::string &) const noexcept(false) {}
};

struct NothrowVoidReturnVoidLeftFn
{
  constexpr void operator()() const noexcept {}
};

struct ThrowingVoidReturnVoidLeftFn
{
  constexpr void operator()() const noexcept(false) {}
};

struct NothrowVoidReturnVoidRightFn
{
  constexpr void operator()() const noexcept {}
};

struct ThrowingVoidReturnVoidRightFn
{
  constexpr void operator()() const noexcept(false) {}
};

} // namespace

TEST_CASE("match constexpr", "[either][constexpr]")
{
  auto matchLeft = [](const std::vector<int> &) -> std::string { return makeExpectedMatchLeftResult(); };
  auto matchRight = [](const std::string &) -> std::string { return makeExpectedMatchRightResult(); };

  SECTION("left alternative invokes left function and returns its result")
  {
    constexpr auto valid = [matchLeft, matchRight] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      return left.match(matchLeft, matchRight) == makeExpectedMatchLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [matchLeft] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.match(matchLeft, [&](const std::string &) {
        rightInvoked = true;
        return makeExpectedMatchRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    constexpr auto valid = [matchLeft, matchRight] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      return right.match(matchLeft, matchRight) == makeExpectedMatchRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [matchRight] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.match(
          [&](const std::vector<int> &) {
            leftInvoked = true;
            return makeExpectedMatchLeftResult();
          },
          matchRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool leftInvoked = false;
      left.match([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      left.match([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool rightInvoked = false;
      right.match([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      right.match([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      [[maybe_unused]] auto res = either.match(
          [](std::vector<int> &v) {
            v.clear();
            return 0;
          },
          [](std::string &) { return 0; });
      return either.left().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      [[maybe_unused]] auto res = either.match([](std::vector<int> &) { return 0; },
                                               [](std::string &s) {
                                                 s.clear();
                                                 return 0;
                                               });
      return either.right().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_left, makeExpectedLeftValue()};
      return either.match([](const std::vector<int> &v) { return v == makeExpectedLeftValue(); },
                          [](const std::string &) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      return either.match([](const std::vector<int> &) { return false; },
                          [](const std::string &s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      auto res = std::move(either).match([](std::vector<int> &&v) { return std::move(v); },
                                         [](std::string &&) { return std::vector<int>{}; });
      return res == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      auto res = std::move(either).match([](std::vector<int> &&) { return std::string{}; },
                                         [](std::string &&s) { return std::move(s); });
      return res == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_left, makeExpectedLeftValue()};
      return static_cast<const Either &&>(either).match(
          [](const std::vector<int> &&v) { return v == makeExpectedLeftValue(); },
          [](const std::string &&) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      return static_cast<const Either &&>(either).match(
          [](const std::vector<int> &&) { return false; },
          [](const std::string &&s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Either>().match(NothrowLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<Either>().match(ThrowingLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<Either>().match(NothrowLeftFn{}, ThrowingRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when both functions throw")
  {
    constexpr auto valid = noexcept(std::declval<Either>().match(ThrowingLeftFn{}, ThrowingRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<Either>().match(NothrowVoidReturnLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Either>().match(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Either>().match(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("match (void left) constexpr", "[either][constexpr]")
{
  auto matchVoidLeft = []() -> std::string { return makeExpectedMatchLeftResult(); };
  auto matchRight = [](const std::string &) -> std::string { return makeExpectedMatchRightResult(); };

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    constexpr auto valid = [matchVoidLeft, matchRight] {
      const EitherVoidLeft left{frt::in_place_left};
      return left.match(matchVoidLeft, matchRight) == makeExpectedMatchLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [matchVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.match(matchVoidLeft, [&](const std::string &) {
        rightInvoked = true;
        return makeExpectedMatchRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    constexpr auto valid = [matchVoidLeft, matchRight] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      return right.match(matchVoidLeft, matchRight) == makeExpectedMatchRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [matchRight] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.match(
          [&] {
            leftInvoked = true;
            return makeExpectedMatchLeftResult();
          },
          matchRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft left{frt::in_place_left};
      bool leftInvoked = false;
      left.match([&] { leftInvoked = true; }, [](const std::string &) {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft left{frt::in_place_left};
      bool rightInvoked = false;
      left.match([] {}, [&](const std::string &) { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool rightInvoked = false;
      right.match([] {}, [&](const std::string &) { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      right.match([&] { leftInvoked = true; }, [](const std::string &) {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      [[maybe_unused]] auto res = either.match([] { return 0; },
                                               [](std::string &s) {
                                                 s.clear();
                                                 return 0;
                                               });
      return either.right().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      return either.match([] { return false; }, [](const std::string &s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      auto res = std::move(either).match([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
      return res == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      return static_cast<const EitherVoidLeft &&>(either).match(
          [] { return false; }, [](const std::string &&s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidLeft>().match(ThrowingVoidLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidLeftFn{}, ThrowingRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidLeft>().match(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidLeft>().match(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("match (void right) constexpr", "[either][constexpr]")
{
  auto matchLeft = [](const std::vector<int> &) -> std::string { return makeExpectedMatchLeftResult(); };
  auto matchVoidRight = []() -> std::string { return makeExpectedMatchRightResult(); };

  SECTION("left alternative invokes left function and returns its result")
  {
    constexpr auto valid = [matchLeft, matchVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      return left.match(matchLeft, matchVoidRight) == makeExpectedMatchLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [matchLeft] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.match(matchLeft, [&] {
        rightInvoked = true;
        return makeExpectedMatchRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    constexpr auto valid = [matchLeft, matchVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      return right.match(matchLeft, matchVoidRight) == makeExpectedMatchRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [matchVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.match(
          [&](const std::vector<int> &) {
            leftInvoked = true;
            return makeExpectedMatchLeftResult();
          },
          matchVoidRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool leftInvoked = false;
      left.match([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      left.match([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight right{frt::in_place_right};
      bool rightInvoked = false;
      right.match([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight right{frt::in_place_right};
      bool leftInvoked = false;
      right.match([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      [[maybe_unused]] auto res = either.match(
          [](std::vector<int> &v) {
            v.clear();
            return 0;
          },
          [] { return 0; });
      return either.left().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      return either.match([](const std::vector<int> &v) { return v == makeExpectedLeftValue(); }, [] { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      auto res =
          std::move(either).match([](std::vector<int> &&v) { return std::move(v); }, [] { return std::vector<int>{}; });
      return res == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      return static_cast<const EitherVoidRight &&>(either).match(
          [](const std::vector<int> &&v) { return v == makeExpectedLeftValue(); }, [] { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidRight>().match(NothrowLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidRight>().match(ThrowingLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidRight>().match(NothrowLeftFn{}, ThrowingVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidRight>().match(NothrowVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidRight>().match(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidRight>().match(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("match (void void) constexpr", "[either][constexpr]")
{
  auto matchVoidLeft = []() -> std::string { return makeExpectedMatchLeftResult(); };
  auto matchVoidRight = []() -> std::string { return makeExpectedMatchRightResult(); };

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    constexpr auto valid = [matchVoidLeft, matchVoidRight] {
      const EitherVoidVoid left{frt::in_place_left};
      return left.match(matchVoidLeft, matchVoidRight) == makeExpectedMatchLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [matchVoidLeft] {
      const EitherVoidVoid left{frt::in_place_left};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.match(matchVoidLeft, [&] {
        rightInvoked = true;
        return makeExpectedMatchRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    constexpr auto valid = [matchVoidLeft, matchVoidRight] {
      const EitherVoidVoid right{frt::in_place_right};
      return right.match(matchVoidLeft, matchVoidRight) == makeExpectedMatchRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [matchVoidRight] {
      const EitherVoidVoid right{frt::in_place_right};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.match(
          [&] {
            leftInvoked = true;
            return makeExpectedMatchLeftResult();
          },
          matchVoidRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid left{frt::in_place_left};
      bool leftInvoked = false;
      left.match([&] { leftInvoked = true; }, [] {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid left{frt::in_place_left};
      bool rightInvoked = false;
      left.match([] {}, [&] { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid right{frt::in_place_right};
      bool rightInvoked = false;
      right.match([] {}, [&] { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid right{frt::in_place_right};
      bool leftInvoked = false;
      right.match([&] { leftInvoked = true; }, [] {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidVoid>().match(ThrowingVoidLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidLeftFn{}, ThrowingVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidVoid>().match(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidVoid>().match(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("Fold with generic function for both alternatives constexpr", "[either][constexpr]")
{
  using namespace test::helpers;

  constexpr auto width = 10;
  constexpr auto height = 20;

  SECTION("generic square function")
  {
    constexpr auto valid = [] {
      const frt::either<Widget, Window> eitherWindow{frt::in_place_right, Window{.width = width, .height = height}};
      const frt::either<Widget, Window> eitherWidget{frt::in_place_left, Widget{.width = width, .height = height}};

      auto square = [](const HasWidthAndHeight auto &obj) { return obj.width * obj.height; };

      return eitherWindow.match(square) == eitherWidget.match(square);
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("generic height modifier")
  {
    constexpr auto valid = [] {
      constexpr auto scaleFactor = 2;

      frt::either<Widget, Window> eitherWindow{frt::in_place_right, Window{.width = width, .height = height}};
      frt::either<Widget, Window> eitherWidget{frt::in_place_left, Widget{.width = width, .height = height}};

      auto scaleHeight = [](HasWidthAndHeight auto &obj) { obj.height *= scaleFactor; };

      eitherWindow.match(scaleHeight);
      eitherWidget.match(scaleHeight);

      return eitherWindow.right().height == eitherWidget.left().height;
    }();

    STATIC_REQUIRE(valid);
  }
}
