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
constexpr auto makeExpectedFoldLeftResult()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit"};
}
constexpr auto makeExpectedFoldRightResult()
{
  return std::string{"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}

struct NothrowLeftFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept { return makeExpectedFoldLeftResult(); }
};

struct ThrowingLeftFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept(false)
  {
    return makeExpectedFoldLeftResult();
  }
};

struct NothrowRightFn
{
  constexpr std::string operator()(const std::string &) const noexcept { return makeExpectedFoldRightResult(); }
};

struct ThrowingRightFn
{
  constexpr std::string operator()(const std::string &) const noexcept(false) { return makeExpectedFoldRightResult(); }
};

struct NothrowVoidLeftFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedFoldLeftResult(); }
};

struct ThrowingVoidLeftFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedFoldLeftResult(); }
};

struct NothrowVoidRightFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedFoldRightResult(); }
};

struct ThrowingVoidRightFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedFoldRightResult(); }
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

TEST_CASE("fold constexpr", "[either][constexpr]")
{
  auto foldLeft = [](const std::vector<int> &) -> std::string { return makeExpectedFoldLeftResult(); };
  auto foldRight = [](const std::string &) -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("left alternative invokes left function and returns its result")
  {
    constexpr auto valid = [foldLeft, foldRight] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      return left.fold(foldLeft, foldRight) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [foldLeft] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.fold(foldLeft, [&](const std::string &) {
        rightInvoked = true;
        return makeExpectedFoldRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    constexpr auto valid = [foldLeft, foldRight] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      return right.fold(foldLeft, foldRight) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [foldRight] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.fold(
          [&](const std::vector<int> &) {
            leftInvoked = true;
            return makeExpectedFoldLeftResult();
          },
          foldRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool leftInvoked = false;
      left.fold([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      left.fold([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool rightInvoked = false;
      right.fold([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      right.fold([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      [[maybe_unused]] auto res = either.fold(
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
      [[maybe_unused]] auto res = either.fold([](std::vector<int> &) { return 0; },
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
      return either.fold([](const std::vector<int> &v) { return v == makeExpectedLeftValue(); },
                         [](const std::string &) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      return either.fold([](const std::vector<int> &) { return false; },
                         [](const std::string &s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      auto res = std::move(either).fold([](std::vector<int> &&v) { return std::move(v); },
                                        [](std::string &&) { return std::vector<int>{}; });
      return res == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      auto res = std::move(either).fold([](std::vector<int> &&) { return std::string{}; },
                                        [](std::string &&s) { return std::move(s); });
      return res == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_left, makeExpectedLeftValue()};
      return static_cast<const Either &&>(either).fold(
          [](const std::vector<int> &&v) { return v == makeExpectedLeftValue(); },
          [](const std::string &&) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      return static_cast<const Either &&>(either).fold(
          [](const std::vector<int> &&) { return false; },
          [](const std::string &&s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Either>().fold(NothrowLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<Either>().fold(ThrowingLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<Either>().fold(NothrowLeftFn{}, ThrowingRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when both functions throw")
  {
    constexpr auto valid = noexcept(std::declval<Either>().fold(ThrowingLeftFn{}, ThrowingRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Either>().fold(NothrowVoidReturnLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Either>().fold(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Either>().fold(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("fold (void left) constexpr", "[either][constexpr]")
{
  auto foldVoidLeft = []() -> std::string { return makeExpectedFoldLeftResult(); };
  auto foldRight = [](const std::string &) -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    constexpr auto valid = [foldVoidLeft, foldRight] {
      const EitherVoidLeft left{frt::in_place_left};
      return left.fold(foldVoidLeft, foldRight) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [foldVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.fold(foldVoidLeft, [&](const std::string &) {
        rightInvoked = true;
        return makeExpectedFoldRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    constexpr auto valid = [foldVoidLeft, foldRight] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      return right.fold(foldVoidLeft, foldRight) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [foldRight] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.fold(
          [&] {
            leftInvoked = true;
            return makeExpectedFoldLeftResult();
          },
          foldRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft left{frt::in_place_left};
      bool leftInvoked = false;
      left.fold([&] { leftInvoked = true; }, [](const std::string &) {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft left{frt::in_place_left};
      bool rightInvoked = false;
      left.fold([] {}, [&](const std::string &) { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool rightInvoked = false;
      right.fold([] {}, [&](const std::string &) { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      right.fold([&] { leftInvoked = true; }, [](const std::string &) {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      [[maybe_unused]] auto res = either.fold([] { return 0; },
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
      return either.fold([] { return false; }, [](const std::string &s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      auto res = std::move(either).fold([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
      return res == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft either{frt::in_place_right, makeExpectedRightValue()};
      return static_cast<const EitherVoidLeft &&>(either).fold(
          [] { return false; }, [](const std::string &&s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidLeft>().fold(ThrowingVoidLeftFn{}, NothrowRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidLeftFn{}, ThrowingRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidLeft>().fold(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidLeft>().fold(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("fold (void right) constexpr", "[either][constexpr]")
{
  auto foldLeft = [](const std::vector<int> &) -> std::string { return makeExpectedFoldLeftResult(); };
  auto foldVoidRight = []() -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("left alternative invokes left function and returns its result")
  {
    constexpr auto valid = [foldLeft, foldVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      return left.fold(foldLeft, foldVoidRight) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [foldLeft] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.fold(foldLeft, [&] {
        rightInvoked = true;
        return makeExpectedFoldRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    constexpr auto valid = [foldLeft, foldVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      return right.fold(foldLeft, foldVoidRight) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [foldVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.fold(
          [&](const std::vector<int> &) {
            leftInvoked = true;
            return makeExpectedFoldLeftResult();
          },
          foldVoidRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool leftInvoked = false;
      left.fold([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      left.fold([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight right{frt::in_place_right};
      bool rightInvoked = false;
      right.fold([](const std::vector<int> &) {}, [&] { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const EitherVoidRight right{frt::in_place_right};
      bool leftInvoked = false;
      right.fold([&](const std::vector<int> &) { leftInvoked = true; }, [] {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      [[maybe_unused]] auto res = either.fold(
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
      return either.fold([](const std::vector<int> &v) { return v == makeExpectedLeftValue(); }, [] { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      auto res =
          std::move(either).fold([](std::vector<int> &&v) { return std::move(v); }, [] { return std::vector<int>{}; });
      return res == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const EitherVoidRight either{frt::in_place_left, makeExpectedLeftValue()};
      return static_cast<const EitherVoidRight &&>(either).fold(
          [](const std::vector<int> &&v) { return v == makeExpectedLeftValue(); }, [] { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidRight>().fold(NothrowLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidRight>().fold(ThrowingLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidRight>().fold(NothrowLeftFn{}, ThrowingVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidRight>().fold(NothrowVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidRight>().fold(ThrowingVoidReturnLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidRight>().fold(NothrowVoidReturnLeftFn{}, ThrowingVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("fold (void void) constexpr", "[either][constexpr]")
{
  auto foldVoidLeft = []() -> std::string { return makeExpectedFoldLeftResult(); };
  auto foldVoidRight = []() -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("left alternative invokes left function with zero arguments and returns its result")
  {
    constexpr auto valid = [foldVoidLeft, foldVoidRight] {
      const EitherVoidVoid left{frt::in_place_left};
      return left.fold(foldVoidLeft, foldVoidRight) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [foldVoidLeft] {
      const EitherVoidVoid left{frt::in_place_left};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left.fold(foldVoidLeft, [&] {
        rightInvoked = true;
        return makeExpectedFoldRightResult();
      });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative invokes right function with zero arguments and returns its result")
  {
    constexpr auto valid = [foldVoidLeft, foldVoidRight] {
      const EitherVoidVoid right{frt::in_place_right};
      return right.fold(foldVoidLeft, foldVoidRight) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [foldVoidRight] {
      const EitherVoidVoid right{frt::in_place_right};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right.fold(
          [&] {
            leftInvoked = true;
            return makeExpectedFoldLeftResult();
          },
          foldVoidRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid left{frt::in_place_left};
      bool leftInvoked = false;
      left.fold([&] { leftInvoked = true; }, [] {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid left{frt::in_place_left};
      bool rightInvoked = false;
      left.fold([] {}, [&] { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid right{frt::in_place_right};
      bool rightInvoked = false;
      right.fold([] {}, [&] { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid right{frt::in_place_right};
      bool leftInvoked = false;
      right.fold([&] { leftInvoked = true; }, [] {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when left function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidVoid>().fold(ThrowingVoidLeftFn{}, NothrowVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when right function throws")
  {
    constexpr auto valid = noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidLeftFn{}, ThrowingVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when left function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidVoid>().fold(ThrowingVoidReturnVoidLeftFn{}, NothrowVoidReturnVoidRightFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when right function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<EitherVoidVoid>().fold(NothrowVoidReturnVoidLeftFn{}, ThrowingVoidReturnVoidRightFn{}));
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

      return eitherWindow.fold(square) == eitherWidget.fold(square);
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

      eitherWindow.fold(scaleHeight);
      eitherWidget.fold(scaleHeight);

      return eitherWindow.right().height == eitherWidget.left().height;
    }();

    STATIC_REQUIRE(valid);
  }
}
