#include <catch2/catch_all.hpp>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <frt/algorithm/transform_right.hpp>
#include <frt/either.hpp>

#include <test/helpers/data.hpp>
#include <test/helpers/types.hpp>

namespace
{

using Either = frt::either<std::string, std::vector<int>>;
using EitherVoidRight = frt::either<std::string, void>;
using EitherVoidLeft = frt::either<void, std::vector<int>>;
using EitherVoidVoid = frt::either<void, void>;

inline const std::string expectedTransformedValue =
    "consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";

using test::helpers::CopyMoveTracker;
using test::helpers::NonMovable;
using QualAwareCallable = test::helpers::QualAwareCallable<int>;
using MoveOnlyCallable = test::helpers::MoveOnlyCallable<int>;

constexpr int freeFunctionAdd(int a, int b) { return a + b; }

} // namespace

TEST_CASE("transform_right partial application", "[algorithm][transform_right]")
{
  const auto transform = [](const std::vector<int> &) -> std::string { return expectedTransformedValue; };
  const auto transformToVoid = [](const std::vector<int> &) -> void {};
  const auto transformExtra = [](const std::vector<int> &, int) -> std::string { return expectedTransformedValue; };
  const auto transformUnwrapped = [](int, const std::vector<int> &) -> std::string { return expectedTransformedValue; };

  const Either left{frt::in_place_left, test::helpers::expectedStringValue};
  const Either right{frt::in_place_right, test::helpers::expectedVectorValue};

  SECTION("right to different right type without extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transform);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type without extra args (value)")
  {
    const auto res = right | frt::transform_right(transform);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to void right without extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transformToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (is_right)")
  {
    const auto res = right | frt::transform_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (value)")
  {
    const auto res = right | frt::transform_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to different right type with unwrapped in extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with unwrapped in extra args (value)")
  {
    const auto res = right | frt::transform_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right with non-const lvalue result forwarding")
  {
    Either either{frt::in_place_right, test::helpers::expectedVectorValue};
    const auto fn = [](std::vector<int> &v) -> bool {
      v.clear();
      return true;
    };
    [[maybe_unused]] const auto res = either | frt::transform_right(fn);
    REQUIRE(either.right().empty());
  }

  SECTION("right with const lvalue result forwarding")
  {
    const Either either{frt::in_place_right, test::helpers::expectedVectorValue};
    const auto fn = [](const std::vector<int> &v) -> bool { return v == test::helpers::expectedVectorValue; };
    const auto res = either | frt::transform_right(fn);
    REQUIRE(res.right());
  }

  SECTION("right with rvalue result forwarding")
  {
    Either either{frt::in_place_right, test::helpers::expectedVectorValue};
    const auto fn = [](std::vector<int> &&v) -> std::vector<int> { return std::move(v); };
    const auto res = std::move(either) | frt::transform_right(fn);
    REQUIRE(res.right() == test::helpers::expectedVectorValue);
  }

  SECTION("left without extra args (is_left)")
  {
    const auto res = left | frt::transform_right(transform);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    const auto res = left | frt::transform_right(transform);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    const auto res = left | frt::transform_right(transformToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (is_left)")
  {
    const auto res = left | frt::transform_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (value)")
  {
    const auto res = left | frt::transform_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }

  SECTION("left with unwrapped in extra args (is_left)")
  {
    const auto res = left | frt::transform_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.is_left());
  }

  SECTION("left with unwrapped in extra args (value)")
  {
    const auto res = left | frt::transform_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }
}

TEST_CASE("transform_right partial application (void right)", "[algorithm][transform_right]")
{
  const auto transformVoidRight = []() -> std::string { return expectedTransformedValue; };
  const auto transformVoidToVoid = []() -> void {};
  const auto transformVoidExtra = [](int) -> std::string { return expectedTransformedValue; };
  const auto transformVoidToVoidExtra = [](int) -> void {};

  const EitherVoidRight left{frt::in_place_left, test::helpers::expectedStringValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("void right to non-void right without extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transformVoidRight);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right without extra args (value)")
  {
    const auto res = right | frt::transform_right(transformVoidRight);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right without extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transformVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (value)")
  {
    const auto res = right | frt::transform_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right with extra args (is_right)")
  {
    const auto res = right | frt::transform_right(transformVoidToVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_right());
  }

  SECTION("left without extra args (is_left)")
  {
    const auto res = left | frt::transform_right(transformVoidRight);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    const auto res = left | frt::transform_right(transformVoidRight);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    const auto res = left | frt::transform_right(transformVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (is_left)")
  {
    const auto res = left | frt::transform_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (value)")
  {
    const auto res = left | frt::transform_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }
}

TEST_CASE("transform_right partial application (void left)", "[algorithm][transform_right]")
{
  const auto transformVoidLeft = [](const std::vector<int> &) -> std::string { return expectedTransformedValue; };
  const auto transformVoidVoidToNonVoid = []() -> std::string { return expectedTransformedValue; };
  const auto transformVoidVoidToVoid = []() -> void {};

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, test::helpers::expectedVectorValue};
  const EitherVoidVoid voidVoidLeft{frt::in_place_left};
  const EitherVoidVoid voidVoidRight{frt::in_place_right};

  SECTION("non-void right with void left (is_right)")
  {
    const auto res = right | frt::transform_right(transformVoidLeft);
    REQUIRE(res.is_right());
  }

  SECTION("non-void right with void left (value)")
  {
    const auto res = right | frt::transform_right(transformVoidLeft);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void left from non-void right containing left (is_left)")
  {
    const auto res = left | frt::transform_right(transformVoidLeft);
    REQUIRE(res.is_left());
  }

  SECTION("void right with void left to non-void right (is_right)")
  {
    const auto res = voidVoidRight | frt::transform_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right with void left to non-void right (value)")
  {
    const auto res = voidVoidRight | frt::transform_right(transformVoidVoidToNonVoid);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right with void left to void right (is_right)")
  {
    const auto res = voidVoidRight | frt::transform_right(transformVoidVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void left from void right containing left (is_left)")
  {
    const auto res = voidVoidLeft | frt::transform_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_left());
  }
}

TEST_CASE("transform_right partial application closure semantics and efficiency", "[algorithm][transform_right]")
{
  using Result = frt::either<std::string, int>;
  constexpr int initialValue = 10;
  constexpr int step1Delta = 5;
  constexpr int step2Multiplier = 2;

  SECTION("chaining multiple partial applications")
  {
    const Result start{frt::in_place_right, initialValue};
    const auto res = start | frt::transform_right([](int v, int d) -> int { return v + d; }, step1Delta) |
                     frt::transform_right([](int m, int v) -> int { return v * m; }, step2Multiplier, frt::unwrapped);

    constexpr int expected = (initialValue + step1Delta) * step2Multiplier;
    REQUIRE(res.right() == expected);
  }

  SECTION("chaining short-circuits on left")
  {
    int secondCallCount = 0;
    const Result start{frt::in_place_left, test::helpers::expectedStringValue};
    [[maybe_unused]] const auto res = start | frt::transform_right([](int v) -> int { return v + 1; }) |
                                      frt::transform_right([&secondCallCount](int v) -> int {
                                        ++secondCallCount;
                                        return v;
                                      });

    REQUIRE(secondCallCount == 0);
  }

  SECTION("std::ref extra argument avoids copies and allows mutation")
  {
    int accumulator = initialValue;
    const Result rObj{frt::in_place_right, step1Delta};

    [[maybe_unused]] const auto res = rObj | frt::transform_right(
                                                 [](int v, int &acc) -> int {
                                                   acc += v;
                                                   return acc;
                                                 },
                                                 std::ref(accumulator));

    constexpr int expected = initialValue + step1Delta;
    REQUIRE(accumulator == expected);
  }

  SECTION("std::cref extra argument avoids copies")
  {
    int copies = 0;
    int moves = 0;
    const CopyMoveTracker tracker{initialValue, &copies, &moves};
    const Result rObj{frt::in_place_right, step1Delta};

    const auto res = rObj | frt::transform_right([](int v, const CopyMoveTracker &t) -> int { return v + t.value; },
                                                 std::cref(tracker));

    REQUIRE((copies == 0 && moves == 0 && res.right() == initialValue + step1Delta));
  }

  SECTION("lvalue closure can be reused multiple times without moving captured args")
  {
    const auto closure = frt::transform_right(
        [](int v, const std::string &suffix) -> std::string {
          return std::string(static_cast<std::size_t>(v), 'a') + suffix;
        },
        test::helpers::expectedStringValue);

    const frt::either<int, int> r1{frt::in_place_right, initialValue};
    const frt::either<int, int> r2{frt::in_place_right, step1Delta};

    const auto res1 = r1 | closure;
    const auto res2 = r2 | closure;

    const std::string expected1 =
        std::string(static_cast<std::size_t>(initialValue), 'a') + test::helpers::expectedStringValue;
    const std::string expected2 =
        std::string(static_cast<std::size_t>(step1Delta), 'a') + test::helpers::expectedStringValue;
    REQUIRE((res1.right() == expected1 && res2.right() == expected2));
  }

  SECTION("lvalue closure does not copy captured args on pipe invocation")
  {
    int copies = 0;
    int moves = 0;
    const auto closure = frt::transform_right([](int v, const CopyMoveTracker &t) -> int { return v + t.value; },
                                              CopyMoveTracker{initialValue, &copies, &moves});

    const int copiesAfterCapture = copies;
    const Result rObj{frt::in_place_right, step1Delta};
    [[maybe_unused]] const auto res = rObj | closure;

    REQUIRE(copies == copiesAfterCapture);
  }

  SECTION("rvalue closure incurs zero copies of rvalue extra argument")
  {
    int copies = 0;
    int moves = 0;
    const Result rObj{frt::in_place_right, step1Delta};

    [[maybe_unused]] const auto res =
        rObj | frt::transform_right([](int v, CopyMoveTracker &&t) -> int { return v + t.value; },
                                    CopyMoveTracker{initialValue, &copies, &moves});

    REQUIRE(copies == 0);
  }

  SECTION("move-only callable and move-only extra argument with rvalue closure")
  {
    const Result rObj{frt::in_place_right, initialValue};
    auto closure = frt::transform_right([](int v, std::unique_ptr<int> ptr) -> int { return v + *ptr; },
                                        std::make_unique<int>(step1Delta));

    const auto res = rObj | std::move(closure);
    constexpr int expected = initialValue + step1Delta;
    REQUIRE(res.right() == expected);
  }

  SECTION("move-only callable object with rvalue closure")
  {
    const Result rObj{frt::in_place_right, initialValue};
    const auto res = rObj | frt::transform_right(MoveOnlyCallable{step2Multiplier});

    constexpr int expected = initialValue * step2Multiplier;
    REQUIRE(res.right() == expected);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&)")
  {
    const Result rObj{frt::in_place_right, initialValue};
    auto closure = frt::transform_right(QualAwareCallable{});
    const auto constClosure = frt::transform_right(QualAwareCallable{});

    const auto resLval = rObj | closure;
    const auto resConstLval = rObj | constClosure;
    const auto resRval = rObj | std::move(closure);
    const auto resConstRval = rObj | std::move(constClosure);

    REQUIRE((resLval.right() == initialValue + 1 && resConstLval.right() == initialValue + 2 &&
             resRval.right() == initialValue + 3 && resConstRval.right() == initialValue + 4));
  }

  SECTION("function reference and array extra arguments decay properly")
  {
    const Result rObj{frt::in_place_right, initialValue};
    const auto res =
        rObj | frt::transform_right(freeFunctionAdd, step1Delta) |
        frt::transform_right(
            [](int v, const char *str) -> int { return v + static_cast<int>(std::char_traits<char>::length(str)); },
            "abc");

    constexpr int expected = initialValue + step1Delta + 3;
    REQUIRE(res.right() == expected);
  }

  SECTION("constraints reject non-movable arguments")
  {
    using Fn = int (*)(int, NonMovable);
    STATIC_REQUIRE_FALSE(std::invocable<frt::detail::transform_right_alg_::transform_right_fn, Fn, NonMovable>);
  }
}
