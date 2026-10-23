#include <catch2/catch_all.hpp>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <frt/algorithm/transform_left.hpp>
#include <frt/either.hpp>

#include <test/helpers/data.hpp>
#include <test/helpers/types.hpp>

namespace
{

using Either = frt::either<std::vector<int>, std::string>;
using EitherVoidLeft = frt::either<void, std::string>;
using EitherVoidRight = frt::either<std::vector<int>, void>;
using EitherVoidVoid = frt::either<void, void>;

constexpr auto makeExpectedTransformedValue()
{
  return std::string{"consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}

using test::helpers::CopyMoveTracker;
using test::helpers::NonMovable;
using QualAwareCallable = test::helpers::QualAwareCallable<int>;
using MoveOnlyCallable = test::helpers::MoveOnlyCallable<int>;

constexpr int freeFunctionAdd(int a, int b) { return a + b; }

} // namespace

TEST_CASE("transform_left partial application constexpr", "[algorithm][transform_left][constexpr]")
{
  const auto transform = [](const std::vector<int> &) -> std::string { return makeExpectedTransformedValue(); };
  const auto transformToVoid = [](const std::vector<int> &) -> void {};
  const auto transformExtra = [](const std::vector<int> &, int) -> std::string {
    return makeExpectedTransformedValue();
  };
  const auto transformUnwrapped = [](int, const std::vector<int> &) -> std::string {
    return makeExpectedTransformedValue();
  };

  SECTION("left to different left type without extra args (is_left)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transform);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transform);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to void left without extra args (is_left)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with extra args without unwrapped (is_left)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformExtra, test::helpers::expectedIntValue);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformExtra, test::helpers::expectedIntValue);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with unwrapped in extra args (is_left)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left to different left type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with non-const lvalue result forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto fn = [](std::vector<int> &v) -> bool {
        v.clear();
        return true;
      };
      [[maybe_unused]] const auto res = either | frt::transform_left(fn);
      return either.left().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with const lvalue result forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto fn = [](const std::vector<int> &v) -> bool { return v == test::helpers::makeExpectedVector(); };
      const auto res = either | frt::transform_left(fn);
      return res.left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with rvalue result forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
      const auto res = std::move(either) | frt::transform_left(fn);
      return res.left() == test::helpers::makeExpectedVector();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (is_right)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transform);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transform);
      return res.right() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args without unwrapped (is_right)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformExtra, test::helpers::expectedIntValue);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformExtra, test::helpers::expectedIntValue);
      return res.right() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with unwrapped in extra args (is_right)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.right() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("transform_left partial application (void left) constexpr", "[algorithm][transform_left][constexpr]")
{
  const auto transformVoidLeft = []() -> std::string { return makeExpectedTransformedValue(); };
  const auto transformVoidToVoid = []() -> void {};
  const auto transformVoidExtra = [](int) -> std::string { return makeExpectedTransformedValue(); };
  const auto transformVoidToVoidExtra = [](int) -> void {};

  SECTION("void left to non-void left without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::transform_left(transformVoidLeft);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to non-void left without extra args (value)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::transform_left(transformVoidLeft);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to void left without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::transform_left(transformVoidToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to non-void left with extra args (is_left)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::transform_left(transformVoidExtra, test::helpers::expectedIntValue);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to non-void left with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::transform_left(transformVoidExtra, test::helpers::expectedIntValue);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left to void left with extra args (is_left)")
  {
    constexpr auto valid = [transformVoidToVoidExtra] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::transform_left(transformVoidToVoidExtra, test::helpers::expectedIntValue);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformVoidLeft);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right without extra args (value)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformVoidLeft);
      return res.right() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with void left return type without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformVoidToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args (is_right)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformVoidExtra, test::helpers::expectedIntValue);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedString()};
      const auto res = right | frt::transform_left(transformVoidExtra, test::helpers::expectedIntValue);
      return res.right() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("transform_left partial application (void right) constexpr", "[algorithm][transform_left][constexpr]")
{
  const auto transformVoidRight = [](const std::vector<int> &) -> std::string {
    return makeExpectedTransformedValue();
  };
  const auto transformVoidVoidToNonVoid = []() -> std::string { return makeExpectedTransformedValue(); };
  const auto transformVoidVoidToVoid = []() -> void {};

  SECTION("non-void left with void right (is_left)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformVoidRight);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("non-void left with void right (value)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedVector()};
      const auto res = left | frt::transform_left(transformVoidRight);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right from non-void left containing right (is_right)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::transform_left(transformVoidRight);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left with void right to non-void left (is_left)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid voidVoidLeft{frt::in_place_left};
      const auto res = voidVoidLeft | frt::transform_left(transformVoidVoidToNonVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left with void right to non-void left (value)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid voidVoidLeft{frt::in_place_left};
      const auto res = voidVoidLeft | frt::transform_left(transformVoidVoidToNonVoid);
      return res.left() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left with void right to void left (is_left)")
  {
    constexpr auto valid = [transformVoidVoidToVoid] {
      const EitherVoidVoid voidVoidLeft{frt::in_place_left};
      const auto res = voidVoidLeft | frt::transform_left(transformVoidVoidToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right from void left containing right (is_right)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid voidVoidRight{frt::in_place_right};
      const auto res = voidVoidRight | frt::transform_left(transformVoidVoidToNonVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }
}

namespace
{
inline constexpr int initialValue = 10;
inline constexpr int step1Delta = 5;
inline constexpr int step2Multiplier = 2;
} // namespace

TEST_CASE("transform_left partial application closure semantics and efficiency constexpr",
          "[algorithm][transform_left][constexpr]")
{
  using Result = frt::either<int, std::string>;

  SECTION("chaining multiple partial applications")
  {
    constexpr auto valid = [] {
      const Result start{frt::in_place_left, initialValue};
      const auto res = start | frt::transform_left([](int v, int d) -> int { return v + d; }, step1Delta) |
                       frt::transform_left([](int m, int v) -> int { return v * m; }, step2Multiplier, frt::unwrapped);

      constexpr int expected = (initialValue + step1Delta) * step2Multiplier;
      return res.left() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("chaining short-circuits on right")
  {
    constexpr auto valid = [] {
      int secondCallCount = 0;
      const Result start{frt::in_place_right, test::helpers::makeExpectedString()};
      [[maybe_unused]] const auto res = start | frt::transform_left([](int v) -> int { return v + 1; }) |
                                        frt::transform_left([&secondCallCount](int v) -> int {
                                          ++secondCallCount;
                                          return v;
                                        });

      return secondCallCount == 0;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("std::ref extra argument avoids copies and allows mutation")
  {
    constexpr auto valid = [] {
      int accumulator = initialValue;
      const Result lObj{frt::in_place_left, step1Delta};

      [[maybe_unused]] const auto res = lObj | frt::transform_left(
                                                   [](int v, int &acc) -> int {
                                                     acc += v;
                                                     return acc;
                                                   },
                                                   std::ref(accumulator));

      constexpr int expected = initialValue + step1Delta;
      return accumulator == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("std::cref extra argument avoids copies")
  {
    constexpr auto valid = [] {
      int copies = 0;
      int moves = 0;
      const CopyMoveTracker tracker{initialValue, &copies, &moves};
      const Result lObj{frt::in_place_left, step1Delta};

      const auto res = lObj | frt::transform_left([](int v, const CopyMoveTracker &t) -> int { return v + t.value; },
                                                  std::cref(tracker));

      return copies == 0 && moves == 0 && res.left() == initialValue + step1Delta;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("lvalue closure can be reused multiple times without moving captured args")
  {
    constexpr auto valid = [] {
      const auto closure = frt::transform_left(
          [](int v, const std::string &suffix) -> std::string {
            return std::string(static_cast<std::size_t>(v), 'a') + suffix;
          },
          test::helpers::makeExpectedString());

      const frt::either<int, int> l1{frt::in_place_left, initialValue};
      const frt::either<int, int> l2{frt::in_place_left, step1Delta};

      const auto res1 = l1 | closure;
      const auto res2 = l2 | closure;

      const std::string expected1 =
          std::string(static_cast<std::size_t>(initialValue), 'a') + test::helpers::makeExpectedString();
      const std::string expected2 =
          std::string(static_cast<std::size_t>(step1Delta), 'a') + test::helpers::makeExpectedString();
      return res1.left() == expected1 && res2.left() == expected2;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("lvalue closure does not copy captured args on pipe invocation")
  {
    constexpr auto valid = [] {
      int copies = 0;
      int moves = 0;
      const auto closure = frt::transform_left([](int v, const CopyMoveTracker &t) -> int { return v + t.value; },
                                               CopyMoveTracker{initialValue, &copies, &moves});

      const int copiesAfterCapture = copies;
      const Result lObj{frt::in_place_left, step1Delta};
      [[maybe_unused]] const auto res = lObj | closure;

      return copies == copiesAfterCapture;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("rvalue closure incurs zero copies of rvalue extra argument")
  {
    constexpr auto valid = [] {
      int copies = 0;
      int moves = 0;
      const Result lObj{frt::in_place_left, step1Delta};

      [[maybe_unused]] const auto res =
          lObj | frt::transform_left([](int v, CopyMoveTracker &&t) -> int { return v + t.value; },
                                     CopyMoveTracker{initialValue, &copies, &moves});

      return copies == 0;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move-only callable and move-only extra argument with rvalue closure")
  {
    constexpr auto valid = [] {
      const Result lObj{frt::in_place_left, initialValue};
      auto closure = frt::transform_left([](int v, std::unique_ptr<int> ptr) -> int { return v + *ptr; },
                                         std::make_unique<int>(step1Delta));

      const auto res = lObj | std::move(closure);
      constexpr int expected = initialValue + step1Delta;
      return res.left() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move-only callable object with rvalue closure")
  {
    constexpr auto valid = [] {
      const Result lObj{frt::in_place_left, initialValue};
      const auto res = lObj | frt::transform_left(MoveOnlyCallable{step2Multiplier});

      constexpr int expected = initialValue * step2Multiplier;
      return res.left() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&)")
  {
    constexpr auto valid = [] {
      const Result lObj{frt::in_place_left, initialValue};
      auto closure = frt::transform_left(QualAwareCallable{});
      const auto constClosure = frt::transform_left(QualAwareCallable{});

      const auto resLval = lObj | closure;
      const auto resConstLval = lObj | constClosure;
      const auto resRval = lObj | std::move(closure);
      const auto resConstRval = lObj | std::move(constClosure);

      return resLval.left() == initialValue + 1 && resConstLval.left() == initialValue + 2 &&
             resRval.left() == initialValue + 3 && resConstRval.left() == initialValue + 4;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("function reference and array extra arguments decay properly")
  {
    constexpr auto valid = [] {
      const Result lObj{frt::in_place_left, initialValue};
      const auto res =
          lObj | frt::transform_left(freeFunctionAdd, step1Delta) |
          frt::transform_left(
              [](int v, const char *str) -> int { return v + static_cast<int>(std::char_traits<char>::length(str)); },
              "abc");

      constexpr int expected = initialValue + step1Delta + 3;
      return res.left() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("constraints reject non-movable arguments")
  {
    using Fn = int (*)(int, NonMovable);
    STATIC_REQUIRE_FALSE(std::invocable<frt::detail::transform_left_alg_::transform_left_fn, Fn, NonMovable>);
  }
}
