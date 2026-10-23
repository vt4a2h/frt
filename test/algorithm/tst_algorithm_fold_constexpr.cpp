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

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <frt/algorithm/fold.hpp>
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

inline constexpr int leftValue = 10;
inline constexpr int rightValue = 20;

using test::helpers::CopyMoveTracker;
using test::helpers::NonMovable;
using QualAwareCallable = test::helpers::QualAwareCallable<int>;

} // namespace

TEST_CASE("fold partial application constexpr", "[algorithm][fold][constexpr]")
{
  auto foldLeft = [](const std::vector<int> &) -> std::string { return makeExpectedFoldLeftResult(); };
  auto foldRight = [](const std::string &) -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("left alternative invokes left function and returns its result")
  {
    constexpr auto valid = [foldLeft, foldRight] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      return (left | frt::fold(foldLeft, foldRight)) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative does not invoke right function")
  {
    constexpr auto valid = [foldLeft] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      [[maybe_unused]] auto res = left | frt::fold(foldLeft, [&](const std::string &) {
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
      return (right | frt::fold(foldLeft, foldRight)) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative does not invoke left function")
  {
    constexpr auto valid = [foldRight] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      [[maybe_unused]] auto res = right | frt::fold(
                                              [&](const std::vector<int> &) {
                                                leftInvoked = true;
                                                return makeExpectedFoldLeftResult();
                                              },
                                              foldRight);
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single callable invoked for left alternative")
  {
    constexpr auto valid = [] {
      auto singleFn = [](const auto &val) -> std::size_t { return std::size(val); };
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      return (left | frt::fold(singleFn)) == makeExpectedLeftValue().size();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single callable invoked for right alternative")
  {
    constexpr auto valid = [] {
      auto singleFn = [](const auto &val) -> std::size_t { return std::size(val); };
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      return (right | frt::fold(singleFn)) == makeExpectedRightValue().size();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function invokes left function")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool leftInvoked = false;
      left | frt::fold([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
      return leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left alternative with void-returning function does not invoke right function")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool rightInvoked = false;
      left | frt::fold([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
      return !rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function invokes right function")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool rightInvoked = false;
      right | frt::fold([](const std::vector<int> &) {}, [&](const std::string &) { rightInvoked = true; });
      return rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right alternative with void-returning function does not invoke left function")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool leftInvoked = false;
      right | frt::fold([&](const std::vector<int> &) { leftInvoked = true; }, [](const std::string &) {});
      return !leftInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single void-returning function invoked for left")
  {
    constexpr auto valid = [] {
      const Either left{frt::in_place_left, makeExpectedLeftValue()};
      bool invoked = false;
      left | frt::fold([&](const auto &) { invoked = true; });
      return invoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single void-returning function invoked for right")
  {
    constexpr auto valid = [] {
      const Either right{frt::in_place_right, makeExpectedRightValue()};
      bool invoked = false;
      right | frt::fold([&](const auto &) { invoked = true; });
      return invoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      [[maybe_unused]] auto res = either | frt::fold(
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
      [[maybe_unused]] auto res = either | frt::fold([](std::vector<int> &) { return 0; },
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
      return either | frt::fold([](const std::vector<int> &v) { return v == makeExpectedLeftValue(); },
                                [](const std::string &) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      return either | frt::fold([](const std::vector<int> &) { return false; },
                                [](const std::string &s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, makeExpectedLeftValue()};
      auto res = std::move(either) | frt::fold([](std::vector<int> &&v) { return std::move(v); },
                                               [](std::string &&) { return std::vector<int>{}; });
      return res == makeExpectedLeftValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, makeExpectedRightValue()};
      auto res = std::move(either) | frt::fold([](std::vector<int> &&) { return std::string{}; },
                                               [](std::string &&s) { return std::move(s); });
      return res == makeExpectedRightValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_left, makeExpectedLeftValue()};
      return static_cast<const Either &&>(either) |
             frt::fold([](const std::vector<int> &&v) { return v == makeExpectedLeftValue(); },
                       [](const std::string &&) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, makeExpectedRightValue()};
      return static_cast<const Either &&>(either) |
             frt::fold([](const std::vector<int> &&) { return false; },
                       [](const std::string &&s) { return s == makeExpectedRightValue(); });
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("fold partial application (void left) constexpr", "[algorithm][fold][constexpr]")
{
  const auto foldVoidLeft = []() -> std::string { return makeExpectedFoldLeftResult(); };
  const auto foldRight = [](const std::string &) -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("void left invokes left function")
  {
    constexpr auto valid = [foldVoidLeft, foldRight] {
      const EitherVoidLeft left{frt::in_place_left};
      return (left | frt::fold(foldVoidLeft, foldRight)) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right invokes right function")
  {
    constexpr auto valid = [foldVoidLeft, foldRight] {
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      return (right | frt::fold(foldVoidLeft, foldRight)) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single callable invoked for void left and right")
  {
    struct Overloaded
    {
      constexpr std::size_t operator()() const { return 0; }
      constexpr std::size_t operator()(const std::string &s) const { return s.size(); }
    };

    constexpr auto valid = [] {
      const EitherVoidLeft left{frt::in_place_left};
      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      return (left | frt::fold(Overloaded{})) == 0 &&
             (right | frt::fold(Overloaded{})) == makeExpectedRightValue().size();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void return type with void left")
  {
    constexpr auto valid = [] {
      const EitherVoidLeft left{frt::in_place_left};
      bool leftInvoked = false;
      left | frt::fold([&]() { leftInvoked = true; }, [](const std::string &) {});

      const EitherVoidLeft right{frt::in_place_right, makeExpectedRightValue()};
      bool rightInvoked = false;
      right | frt::fold([]() {}, [&](const std::string &) { rightInvoked = true; });

      return leftInvoked && rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("fold partial application (void right) constexpr", "[algorithm][fold][constexpr]")
{
  const auto foldLeft = [](const std::vector<int> &) -> std::string { return makeExpectedFoldLeftResult(); };
  const auto foldVoidRight = []() -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("left invokes left function")
  {
    constexpr auto valid = [foldLeft, foldVoidRight] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      return (left | frt::fold(foldLeft, foldVoidRight)) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right invokes right function")
  {
    constexpr auto valid = [foldLeft, foldVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      return (right | frt::fold(foldLeft, foldVoidRight)) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single callable invoked for left and void right")
  {
    struct Overloaded
    {
      constexpr std::size_t operator()(const std::vector<int> &v) const { return v.size(); }
      constexpr std::size_t operator()() const { return 0; }
    };

    constexpr auto valid = [] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      const EitherVoidRight right{frt::in_place_right};
      return (left | frt::fold(Overloaded{})) == makeExpectedLeftValue().size() &&
             (right | frt::fold(Overloaded{})) == 0;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void return type with void right")
  {
    constexpr auto valid = [] {
      const EitherVoidRight left{frt::in_place_left, makeExpectedLeftValue()};
      bool leftInvoked = false;
      left | frt::fold([&](const std::vector<int> &) { leftInvoked = true; }, []() {});

      const EitherVoidRight right{frt::in_place_right};
      bool rightInvoked = false;
      right | frt::fold([](const std::vector<int> &) {}, [&]() { rightInvoked = true; });

      return leftInvoked && rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("fold partial application (void void) constexpr", "[algorithm][fold][constexpr]")
{
  const auto foldVoidLeft = []() -> std::string { return makeExpectedFoldLeftResult(); };
  const auto foldVoidRight = []() -> std::string { return makeExpectedFoldRightResult(); };

  SECTION("void left invokes left function")
  {
    constexpr auto valid = [foldVoidLeft, foldVoidRight] {
      const EitherVoidVoid left{frt::in_place_left};
      return (left | frt::fold(foldVoidLeft, foldVoidRight)) == makeExpectedFoldLeftResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right invokes right function")
  {
    constexpr auto valid = [foldVoidLeft, foldVoidRight] {
      const EitherVoidVoid right{frt::in_place_right};
      return (right | frt::fold(foldVoidLeft, foldVoidRight)) == makeExpectedFoldRightResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single callable invoked for void void")
  {
    constexpr auto valid = [] {
      int callCount = 0;
      auto singleFn = [&callCount]() -> int {
        ++callCount;
        return callCount;
      };

      const EitherVoidVoid left{frt::in_place_left};
      const EitherVoidVoid right{frt::in_place_right};

      auto c1 = left | frt::fold(std::ref(singleFn));
      auto c2 = right | frt::fold(std::ref(singleFn));
      return c1 == 1 && c2 == 2;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void return type with void void")
  {
    constexpr auto valid = [] {
      const EitherVoidVoid left{frt::in_place_left};
      bool leftInvoked = false;
      left | frt::fold([&]() { leftInvoked = true; }, []() {});

      const EitherVoidVoid right{frt::in_place_right};
      bool rightInvoked = false;
      right | frt::fold([]() {}, [&]() { rightInvoked = true; });

      return leftInvoked && rightInvoked;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("fold partial application closure semantics and efficiency constexpr", "[algorithm][fold][constexpr]")
{
  using Result = frt::either<int, int>;

  SECTION("lvalue closure can be reused multiple times")
  {
    constexpr auto valid = [] {
      const auto closure = frt::fold([](int v) -> int { return v * 2; }, [](int v) -> int { return v * 3; });

      const Result lObj{frt::in_place_left, leftValue};
      const Result rObj{frt::in_place_right, rightValue};

      return (lObj | closure) == leftValue * 2 && (rObj | closure) == rightValue * 3;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("single callable lvalue closure can be reused multiple times")
  {
    constexpr auto valid = [] {
      const auto closure = frt::fold([](int v) -> int { return v * 5; });

      const Result lObj{frt::in_place_left, leftValue};
      const Result rObj{frt::in_place_right, rightValue};

      return (lObj | closure) == leftValue * 5 && (rObj | closure) == rightValue * 5;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&) with two callables")
  {
    constexpr auto valid = [] {
      const Result lObj{frt::in_place_left, leftValue};
      auto closure = frt::fold(QualAwareCallable{}, QualAwareCallable{});
      const auto constClosure = frt::fold(QualAwareCallable{}, QualAwareCallable{});

      const auto resLval = lObj | closure;
      const auto resConstLval = lObj | constClosure;
      const auto resRval = lObj | std::move(closure);
      const auto resConstRval = lObj | std::move(constClosure);

      return resLval == leftValue + 1 && resConstLval == leftValue + 2 && resRval == leftValue + 3 &&
             resConstRval == leftValue + 4;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&) with single callable")
  {
    constexpr auto valid = [] {
      const Result lObj{frt::in_place_left, leftValue};
      auto closure = frt::fold(QualAwareCallable{});
      const auto constClosure = frt::fold(QualAwareCallable{});

      const auto resLval = lObj | closure;
      const auto resConstLval = lObj | constClosure;
      const auto resRval = lObj | std::move(closure);
      const auto resConstRval = lObj | std::move(constClosure);

      return resLval == leftValue + 1 && resConstLval == leftValue + 2 && resRval == leftValue + 3 &&
             resConstRval == leftValue + 4;
    }();

    STATIC_REQUIRE(valid);
  }
}
