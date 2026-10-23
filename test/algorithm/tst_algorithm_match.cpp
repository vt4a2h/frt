/*
 * FRT -- Result types for modern C++.
 *
 * Copyright (c) 2026-present Vitaly Fanaskov
 *
 * Use, modification and distribution are subject to the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or a copy at https://www.boost.org/LICENSE_1_0.txt
 *
 * Project home: https://github.com/vt4a2h/frt
 */

#include <catch2/catch_all.hpp>

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <frt/algorithm/match.hpp>
#include <frt/either.hpp>

#include <test/helpers/data.hpp>
#include <test/helpers/types.hpp>

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

inline constexpr int leftValue = 10;
inline constexpr int rightValue = 20;
inline constexpr int multiplier = 3;

using test::helpers::CopyMoveTracker;
using test::helpers::NonMovable;
using QualAwareCallable = test::helpers::QualAwareCallable<int>;
using MoveOnlyCallable = test::helpers::MoveOnlyCallable<int>;

} // namespace

TEST_CASE("match partial application", "[algorithm][match]")
{
  auto matchLeft = [](const std::vector<int> &) -> std::string { return expectedMatchLeftResult; };
  auto matchRight = [](const std::string &) -> std::string { return expectedMatchRightResult; };

  const Either left{frt::in_place_left, expectedLeftValue};
  const Either right{frt::in_place_right, expectedRightValue};

  SECTION("left alternative invokes left function and returns its result")
  {
    REQUIRE((left | frt::match(matchLeft, matchRight)) == expectedMatchLeftResult);
  }

  SECTION("left alternative does not invoke right function")
  {
    bool rightInvoked = false;
    [[maybe_unused]] auto res = left | frt::match(matchLeft, [&](const std::string &) {
                                  rightInvoked = true;
                                  return expectedMatchRightResult;
                                });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative invokes right function and returns its result")
  {
    REQUIRE((right | frt::match(matchLeft, matchRight)) == expectedMatchRightResult);
  }

  SECTION("right alternative does not invoke left function")
  {
    bool leftInvoked = false;
    [[maybe_unused]] auto res = right | frt::match(
                                            [&](const std::vector<int> &) {
                                              leftInvoked = true;
                                              return expectedMatchLeftResult;
                                            },
                                            matchRight);
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("single callable invoked for left alternative")
  {
    auto singleFn = [](const auto &val) -> std::size_t { return std::size(val); };
    REQUIRE((left | frt::match(singleFn)) == expectedLeftValue.size());
  }

  SECTION("single callable invoked for right alternative")
  {
    auto singleFn = [](const auto &val) -> std::size_t { return std::size(val); };
    REQUIRE((right | frt::match(singleFn)) == expectedRightValue.size());
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    bool leftInvoked = false;
    left | frt::match([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE(leftInvoked);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    bool rightInvoked = false;
    left | frt::match([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE_FALSE(rightInvoked);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    bool rightInvoked = false;
    right | frt::match([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    bool leftInvoked = false;
    right | frt::match([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(leftInvoked);
  }

  SECTION("single void-returning function invoked for left")
  {
    bool invoked = false;
    left | frt::match([&](const auto &) { invoked = true; });
    REQUIRE(invoked);
  }

  SECTION("single void-returning function invoked for right")
  {
    bool invoked = false;
    right | frt::match([&](const auto &) { invoked = true; });
    REQUIRE(invoked);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    [[maybe_unused]] auto res = either | frt::match(
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
    [[maybe_unused]] auto res = either | frt::match([](std::vector<int> &) { return 0; },
                                                    [](std::string &s) {
                                                      s.clear();
                                                      return 0;
                                                    });
    REQUIRE(either.right().empty());
  }

  SECTION("left with const lvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto res = either | frt::match([](const std::vector<int> &v) { return v == expectedLeftValue; },
                                   [](const std::string &) { return false; });
    REQUIRE(res);
  }

  SECTION("right with const lvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto res = either | frt::match([](const std::vector<int> &) { return false; },
                                   [](const std::string &s) { return s == expectedRightValue; });
    REQUIRE(res);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    Either either{frt::in_place_left, expectedLeftValue};
    auto res = std::move(either) | frt::match([](std::vector<int> &&v) { return std::move(v); },
                                              [](std::string &&) { return std::vector<int>{}; });
    REQUIRE(res == expectedLeftValue);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    Either either{frt::in_place_right, expectedRightValue};
    auto res = std::move(either) | frt::match([](std::vector<int> &&) { return std::string{}; },
                                              [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedRightValue);
  }

  SECTION("left with const rvalue forwarding")
  {
    const Either either{frt::in_place_left, expectedLeftValue};
    auto res = static_cast<const Either &&>(either) |
               frt::match([](const std::vector<int> &&v) { return v == expectedLeftValue; },
                          [](const std::string &&) { return false; });
    REQUIRE(res);
  }

  SECTION("right with const rvalue forwarding")
  {
    const Either either{frt::in_place_right, expectedRightValue};
    auto res = static_cast<const Either &&>(either) |
               frt::match([](const std::vector<int> &&) { return false; },
                          [](const std::string &&s) { return s == expectedRightValue; });
    REQUIRE(res);
  }
}

TEST_CASE("match partial application (void left)", "[algorithm][match]")
{
  const auto matchVoidLeft = []() -> std::string { return expectedMatchLeftResult; };
  const auto matchRight = [](const std::string &) -> std::string { return expectedMatchRightResult; };

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedRightValue};

  SECTION("void left invokes left function")
  {
    REQUIRE((left | frt::match(matchVoidLeft, matchRight)) == expectedMatchLeftResult);
  }

  SECTION("right invokes right function")
  {
    REQUIRE((right | frt::match(matchVoidLeft, matchRight)) == expectedMatchRightResult);
  }

  SECTION("single callable invoked for void left and right")
  {
    struct Overloaded
    {
      std::size_t operator()() const { return 0; }
      std::size_t operator()(const std::string &s) const { return s.size(); }
    };

    REQUIRE((left | frt::match(Overloaded{})) == 0);
    REQUIRE((right | frt::match(Overloaded{})) == expectedRightValue.size());
  }

  SECTION("void return type with void left")
  {
    bool leftInvoked = false;
    left | frt::match([&]() { leftInvoked = true; }, [](const std::string &) {});
    REQUIRE(leftInvoked);

    bool rightInvoked = false;
    right | frt::match([]() {}, [&](const std::string &) { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }
}

TEST_CASE("match partial application (void right)", "[algorithm][match]")
{
  const auto matchLeft = [](const std::vector<int> &) -> std::string { return expectedMatchLeftResult; };
  const auto matchVoidRight = []() -> std::string { return expectedMatchRightResult; };

  const EitherVoidRight left{frt::in_place_left, expectedLeftValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("left invokes left function")
  {
    REQUIRE((left | frt::match(matchLeft, matchVoidRight)) == expectedMatchLeftResult);
  }

  SECTION("void right invokes right function")
  {
    REQUIRE((right | frt::match(matchLeft, matchVoidRight)) == expectedMatchRightResult);
  }

  SECTION("single callable invoked for left and void right")
  {
    struct Overloaded
    {
      std::size_t operator()(const std::vector<int> &v) const { return v.size(); }
      std::size_t operator()() const { return 0; }
    };

    REQUIRE((left | frt::match(Overloaded{})) == expectedLeftValue.size());
    REQUIRE((right | frt::match(Overloaded{})) == 0);
  }

  SECTION("void return type with void right")
  {
    bool leftInvoked = false;
    left | frt::match([&](const std::vector<int> &) { leftInvoked = true; }, []() {});
    REQUIRE(leftInvoked);

    bool rightInvoked = false;
    right | frt::match([](const std::vector<int> &) {}, [&]() { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }
}

TEST_CASE("match partial application (void void)", "[algorithm][match]")
{
  const auto matchVoidLeft = []() -> std::string { return expectedMatchLeftResult; };
  const auto matchVoidRight = []() -> std::string { return expectedMatchRightResult; };

  const EitherVoidVoid left{frt::in_place_left};
  const EitherVoidVoid right{frt::in_place_right};

  SECTION("void left invokes left function")
  {
    REQUIRE((left | frt::match(matchVoidLeft, matchVoidRight)) == expectedMatchLeftResult);
  }

  SECTION("void right invokes right function")
  {
    REQUIRE((right | frt::match(matchVoidLeft, matchVoidRight)) == expectedMatchRightResult);
  }

  SECTION("single callable invoked for void void")
  {
    int callCount = 0;
    auto singleFn = [&callCount]() -> int {
      ++callCount;
      return callCount;
    };

    REQUIRE((left | frt::match(std::ref(singleFn))) == 1);
    REQUIRE((right | frt::match(std::ref(singleFn))) == 2);
  }

  SECTION("void return type with void void")
  {
    bool leftInvoked = false;
    left | frt::match([&]() { leftInvoked = true; }, []() {});
    REQUIRE(leftInvoked);

    bool rightInvoked = false;
    right | frt::match([]() {}, [&]() { rightInvoked = true; });
    REQUIRE(rightInvoked);
  }
}

TEST_CASE("match partial application closure semantics and efficiency", "[algorithm][match]")
{
  using Result = frt::either<int, int>;

  SECTION("lvalue closure can be reused multiple times")
  {
    const auto closure = frt::match([](int v) -> int { return v * 2; }, [](int v) -> int { return v * 3; });

    const Result lObj{frt::in_place_left, leftValue};
    const Result rObj{frt::in_place_right, rightValue};

    REQUIRE((lObj | closure) == leftValue * 2);
    REQUIRE((rObj | closure) == rightValue * 3);
  }

  SECTION("single callable lvalue closure can be reused multiple times")
  {
    const auto closure = frt::match([](int v) -> int { return v * 5; });

    const Result lObj{frt::in_place_left, leftValue};
    const Result rObj{frt::in_place_right, rightValue};

    REQUIRE((lObj | closure) == leftValue * 5);
    REQUIRE((rObj | closure) == rightValue * 5);
  }

  SECTION("std::ref callable avoids copies and allows mutation")
  {
    int leftCalls = 0;
    int rightCalls = 0;
    auto leftFn = [&leftCalls](int v) -> int {
      ++leftCalls;
      return v;
    };
    auto rightFn = [&rightCalls](int v) -> int {
      ++rightCalls;
      return v;
    };

    const Result lObj{frt::in_place_left, leftValue};
    const Result rObj{frt::in_place_right, rightValue};

    [[maybe_unused]] const auto res1 = lObj | frt::match(std::ref(leftFn), std::ref(rightFn));
    [[maybe_unused]] const auto res2 = rObj | frt::match(std::ref(leftFn), std::ref(rightFn));

    REQUIRE((leftCalls == 1 && rightCalls == 1));
  }

  SECTION("std::cref callable avoids copies")
  {
    auto leftFn = [](int v) -> int { return v; };
    auto rightFn = [](int v) -> int { return v; };

    const Result lObj{frt::in_place_left, leftValue};
    const auto res = lObj | frt::match(std::cref(leftFn), std::cref(rightFn));
    REQUIRE(res == leftValue);
  }

  SECTION("lvalue closure does not copy captured callables on pipe invocation")
  {
    int copies = 0;
    int moves = 0;
    const auto closure =
        frt::match([t = CopyMoveTracker{leftValue, &copies, &moves}](int v) -> int { return v + t.value; });

    const int copiesAfterCapture = copies;
    const Result lObj{frt::in_place_left, rightValue};
    [[maybe_unused]] const auto res = lObj | closure;

    REQUIRE(copies == copiesAfterCapture);
  }

  SECTION("rvalue closure incurs zero copies of rvalue callable")
  {
    int copies = 0;
    int moves = 0;
    const Result lObj{frt::in_place_left, rightValue};

    [[maybe_unused]] const auto res =
        lObj | frt::match([t = CopyMoveTracker{leftValue, &copies, &moves}](int v) -> int { return v + t.value; });

    REQUIRE(copies == 0);
  }

  SECTION("move-only callable object with rvalue closure (two callables)")
  {
    const Result lObj{frt::in_place_left, leftValue};
    const Result rObj{frt::in_place_right, rightValue};

    const auto resLeft = lObj | frt::match(MoveOnlyCallable{multiplier}, MoveOnlyCallable{multiplier});
    const auto resRight = rObj | frt::match(MoveOnlyCallable{multiplier}, MoveOnlyCallable{multiplier});

    REQUIRE((resLeft == leftValue * multiplier && resRight == rightValue * multiplier));
  }

  SECTION("move-only callable object with rvalue closure (single callable)")
  {
    const Result lObj{frt::in_place_left, leftValue};
    const auto res = lObj | frt::match(MoveOnlyCallable{multiplier});

    REQUIRE(res == leftValue * multiplier);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&) with two callables")
  {
    const Result lObj{frt::in_place_left, leftValue};
    auto closure = frt::match(QualAwareCallable{}, QualAwareCallable{});
    const auto constClosure = frt::match(QualAwareCallable{}, QualAwareCallable{});

    const auto resLval = lObj | closure;
    const auto resConstLval = lObj | constClosure;
    const auto resRval = lObj | std::move(closure);
    const auto resConstRval = lObj | std::move(constClosure);

    REQUIRE((resLval == leftValue + 1 && resConstLval == leftValue + 2 && resRval == leftValue + 3 &&
             resConstRval == leftValue + 4));
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&) with single callable")
  {
    const Result lObj{frt::in_place_left, leftValue};
    auto closure = frt::match(QualAwareCallable{});
    const auto constClosure = frt::match(QualAwareCallable{});

    const auto resLval = lObj | closure;
    const auto resConstLval = lObj | constClosure;
    const auto resRval = lObj | std::move(closure);
    const auto resConstRval = lObj | std::move(constClosure);

    REQUIRE((resLval == leftValue + 1 && resConstLval == leftValue + 2 && resRval == leftValue + 3 &&
             resConstRval == leftValue + 4));
  }

  SECTION("constraints reject non-movable arguments")
  {
    STATIC_REQUIRE_FALSE(std::invocable<frt::detail::match_alg_::match_fn, NonMovable>);
    STATIC_REQUIRE_FALSE(std::invocable<frt::detail::match_alg_::match_fn, NonMovable, NonMovable>);
  }
}
