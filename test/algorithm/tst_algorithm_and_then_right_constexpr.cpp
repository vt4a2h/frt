#include <catch2/catch_all.hpp>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <frt/algorithm/and_then_right.hpp>
#include <frt/either.hpp>

#include <test/helpers/data.hpp>

namespace
{

using Either = frt::either<std::string, std::vector<int>>;
using EitherVoidRight = frt::either<std::string, void>;
using EitherVoidLeft = frt::either<void, std::vector<int>>;
using EitherVoidVoid = frt::either<void, void>;

constexpr auto makeExpectedTransformedValue()
{
  return std::string{"consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}

struct CopyMoveTracker
{
  int value{};
  int *copies{};
  int *moves{};

  constexpr CopyMoveTracker(int v, int *c, int *m) noexcept : value{v}, copies{c}, moves{m} {}

  constexpr CopyMoveTracker(const CopyMoveTracker &other) noexcept
      : value{other.value}, copies{other.copies}, moves{other.moves}
  {
    if (copies != nullptr) {
      ++(*copies);
    }
  }

  constexpr CopyMoveTracker(CopyMoveTracker &&other) noexcept
      : value{other.value}, copies{other.copies}, moves{other.moves}
  {
    if (moves != nullptr) {
      ++(*moves);
    }
  }

  constexpr CopyMoveTracker &operator=(const CopyMoveTracker &) noexcept = default;
  constexpr CopyMoveTracker &operator=(CopyMoveTracker &&) noexcept = default;
};

struct QualAwareCallable
{
  using Result = frt::either<std::string, int>;

  constexpr Result operator()(int v) & { return {frt::in_place_right, v + 1}; }
  constexpr Result operator()(int v) const & { return {frt::in_place_right, v + 2}; }
  constexpr Result operator()(int v) && { return {frt::in_place_right, v + 3}; }
  constexpr Result operator()(int v) const && { return {frt::in_place_right, v + 4}; }
};

struct MoveOnlyCallable
{
  using Result = frt::either<std::string, int>;

  std::unique_ptr<int> factor;

  constexpr explicit MoveOnlyCallable(int f) : factor{std::make_unique<int>(f)} {}
  MoveOnlyCallable(const MoveOnlyCallable &) = delete;
  MoveOnlyCallable &operator=(const MoveOnlyCallable &) = delete;
  constexpr MoveOnlyCallable(MoveOnlyCallable &&) noexcept = default;
  constexpr MoveOnlyCallable &operator=(MoveOnlyCallable &&) noexcept = default;

  constexpr Result operator()(int v) && { return {frt::in_place_right, v * (*factor)}; }
};

struct NonMovable
{
  NonMovable() = default;
  NonMovable(const NonMovable &) = delete;
  NonMovable(NonMovable &&) = delete;
};

constexpr frt::either<std::string, int> freeFunctionAdd(int a, int b) { return {frt::in_place_right, a + b}; }

} // namespace

TEST_CASE("and_then_right partial application constexpr", "[algorithm][and_then_right][constexpr]")
{
  const auto transform = [](const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  const auto transformToVoid = [](const std::vector<int> &) -> frt::either<std::string, void> {
    return {frt::in_place_right};
  };
  const auto transformExtra = [](const std::vector<int> &, int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  const auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };

  SECTION("right to different right type without extra args (is_right)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transform);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transform);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to void right without extra args (is_right)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with extra args without unwrapped (is_right)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with unwrapped in extra args (is_right)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right to different right type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with non-const lvalue result forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto fn = [](std::vector<int> &v) -> frt::either<std::string, bool> {
        v.clear();
        return {frt::in_place_right, true};
      };
      [[maybe_unused]] const auto res = either | frt::and_then_right(fn);
      return either.right().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with const lvalue result forwarding")
  {
    constexpr auto valid = [] {
      const Either either{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto fn = [](const std::vector<int> &v) -> frt::either<std::string, bool> {
        return {frt::in_place_right, v == test::helpers::makeExpectedVector()};
      };
      const auto res = either | frt::and_then_right(fn);
      return res.right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("right with rvalue result forwarding")
  {
    constexpr auto valid = [] {
      Either either{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto fn = [](std::vector<int> &&v) -> frt::either<std::string, std::vector<int>> {
        return {frt::in_place_right, std::move(v)};
      };
      const auto res = std::move(either) | frt::and_then_right(fn);
      return res.right() == test::helpers::makeExpectedVector();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (is_left)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transform);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transform);
      return res.left() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    constexpr auto valid = [transformToVoid] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args without unwrapped (is_left)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
      return res.left() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with unwrapped in extra args (is_left)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Either left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
      return res.left() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("and_then_right partial application (void right) constexpr", "[algorithm][and_then_right][constexpr]")
{
  const auto transformVoidRight = []() -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  const auto transformVoidToVoid = []() -> frt::either<std::string, void> { return {frt::in_place_right}; };
  const auto transformVoidExtra = [](int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  const auto transformVoidToVoidExtra = [](int) -> frt::either<std::string, void> { return {frt::in_place_right}; };

  SECTION("void right to non-void right without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidRight);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to non-void right without extra args (value)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidRight);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to void right without extra args (is_right)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to non-void right with extra args (is_right)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to non-void right with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right to void right with extra args (is_right)")
  {
    constexpr auto valid = [transformVoidToVoidExtra] {
      const EitherVoidRight right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidToVoidExtra, test::helpers::expectedIntValue);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformVoidRight);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left without extra args (value)")
  {
    constexpr auto valid = [transformVoidRight] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformVoidRight);
      return res.left() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    constexpr auto valid = [transformVoidToVoid] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformVoidToVoid);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args (is_left)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("left with extra args (value)")
  {
    constexpr auto valid = [transformVoidExtra] {
      const EitherVoidRight left{frt::in_place_left, test::helpers::makeExpectedString()};
      const auto res = left | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
      return res.left() == test::helpers::makeExpectedString();
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("and_then_right partial application (void left) constexpr", "[algorithm][and_then_right][constexpr]")
{
  const auto transformVoidLeft = [](const std::vector<int> &) -> frt::either<void, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  const auto transformVoidVoidToNonVoid = []() -> frt::either<void, std::string> {
    return {frt::in_place_right, makeExpectedTransformedValue()};
  };
  const auto transformVoidVoidToVoid = []() -> frt::either<void, void> { return {frt::in_place_right}; };

  SECTION("non-void right with void left (is_right)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformVoidLeft);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("non-void right with void left (value)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft right{frt::in_place_right, test::helpers::makeExpectedVector()};
      const auto res = right | frt::and_then_right(transformVoidLeft);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left from non-void right containing left (is_left)")
  {
    constexpr auto valid = [transformVoidLeft] {
      const EitherVoidLeft left{frt::in_place_left};
      const auto res = left | frt::and_then_right(transformVoidLeft);
      return res.is_left();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right with void left to non-void right (is_right)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidVoidToNonVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right with void left to non-void right (value)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidVoidToNonVoid);
      return res.right() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void right with void left to void right (is_right)")
  {
    constexpr auto valid = [transformVoidVoidToVoid] {
      const EitherVoidVoid right{frt::in_place_right};
      const auto res = right | frt::and_then_right(transformVoidVoidToVoid);
      return res.is_right();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("void left from void right containing left (is_left)")
  {
    constexpr auto valid = [transformVoidVoidToNonVoid] {
      const EitherVoidVoid left{frt::in_place_left};
      const auto res = left | frt::and_then_right(transformVoidVoidToNonVoid);
      return res.is_left();
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

TEST_CASE("and_then_right partial application closure semantics and efficiency constexpr",
          "[algorithm][and_then_right][constexpr]")
{
  using Result = frt::either<std::string, int>;

  SECTION("chaining multiple partial applications")
  {
    constexpr auto valid = [] {
      const Result start{frt::in_place_right, initialValue};
      const auto res =
          start | frt::and_then_right([](int v, int d) -> Result { return {frt::in_place_right, v + d}; }, step1Delta) |
          frt::and_then_right([](int m, int v) -> Result { return {frt::in_place_right, v * m}; }, step2Multiplier,
                              frt::unwrapped);

      constexpr int expected = (initialValue + step1Delta) * step2Multiplier;
      return res.right() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("chaining short-circuits on left")
  {
    constexpr auto valid = [] {
      int secondCallCount = 0;
      const Result start{frt::in_place_right, initialValue};
      [[maybe_unused]] const auto res =
          start |
          frt::and_then_right([](int) -> Result { return {frt::in_place_left, test::helpers::makeExpectedString()}; }) |
          frt::and_then_right([&secondCallCount](int v) -> Result {
            ++secondCallCount;
            return {frt::in_place_right, v};
          });

      return secondCallCount == 0;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("std::ref extra argument avoids copies and allows mutation")
  {
    constexpr auto valid = [] {
      int accumulator = initialValue;
      const Result rObj{frt::in_place_right, step1Delta};

      [[maybe_unused]] const auto res = rObj | frt::and_then_right(
                                                   [](int v, int &acc) -> Result {
                                                     acc += v;
                                                     return {frt::in_place_right, acc};
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
      const Result rObj{frt::in_place_right, step1Delta};

      const auto res =
          rObj | frt::and_then_right(
                     [](int v, const CopyMoveTracker &t) -> Result { return {frt::in_place_right, v + t.value}; },
                     std::cref(tracker));

      return copies == 0 && moves == 0 && res.right() == initialValue + step1Delta;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("lvalue closure can be reused multiple times without moving captured args")
  {
    constexpr auto valid = [] {
      const auto closure = frt::and_then_right(
          [](int v, const std::string &suffix) -> frt::either<int, std::string> {
            return {frt::in_place_right, std::string(static_cast<std::size_t>(v), 'a') + suffix};
          },
          test::helpers::makeExpectedString());

      const frt::either<int, int> r1{frt::in_place_right, initialValue};
      const frt::either<int, int> r2{frt::in_place_right, step1Delta};

      const auto res1 = r1 | closure;
      const auto res2 = r2 | closure;

      const std::string expected1 =
          std::string(static_cast<std::size_t>(initialValue), 'a') + test::helpers::makeExpectedString();
      const std::string expected2 =
          std::string(static_cast<std::size_t>(step1Delta), 'a') + test::helpers::makeExpectedString();
      return res1.right() == expected1 && res2.right() == expected2;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("lvalue closure does not copy captured args on pipe invocation")
  {
    constexpr auto valid = [] {
      int copies = 0;
      int moves = 0;
      const auto closure = frt::and_then_right(
          [](int v, const CopyMoveTracker &t) -> Result { return {frt::in_place_right, v + t.value}; },
          CopyMoveTracker{initialValue, &copies, &moves});

      const int copiesAfterCapture = copies;
      const Result rObj{frt::in_place_right, step1Delta};
      [[maybe_unused]] const auto res = rObj | closure;

      return copies == copiesAfterCapture;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("rvalue closure incurs zero copies of rvalue extra argument")
  {
    constexpr auto valid = [] {
      int copies = 0;
      int moves = 0;
      const Result rObj{frt::in_place_right, step1Delta};

      [[maybe_unused]] const auto res =
          rObj |
          frt::and_then_right([](int v, CopyMoveTracker &&t) -> Result { return {frt::in_place_right, v + t.value}; },
                              CopyMoveTracker{initialValue, &copies, &moves});

      return copies == 0;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move-only callable and move-only extra argument with rvalue closure")
  {
    constexpr auto valid = [] {
      const Result rObj{frt::in_place_right, initialValue};
      auto closure =
          frt::and_then_right([](int v, std::unique_ptr<int> ptr) -> Result { return {frt::in_place_right, v + *ptr}; },
                              std::make_unique<int>(step1Delta));

      const auto res = rObj | std::move(closure);
      constexpr int expected = initialValue + step1Delta;
      return res.right() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("move-only callable object with rvalue closure")
  {
    constexpr auto valid = [] {
      const Result rObj{frt::in_place_right, initialValue};
      const auto res = rObj | frt::and_then_right(MoveOnlyCallable{step2Multiplier});

      constexpr int expected = initialValue * step2Multiplier;
      return res.right() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&)")
  {
    constexpr auto valid = [] {
      const Result rObj{frt::in_place_right, initialValue};
      auto closure = frt::and_then_right(QualAwareCallable{});
      const auto constClosure = frt::and_then_right(QualAwareCallable{});

      const auto resLval = rObj | closure;
      const auto resConstLval = rObj | constClosure;
      const auto resRval = rObj | std::move(closure);
      const auto resConstRval = rObj | std::move(constClosure);

      return resLval.right() == initialValue + 1 && resConstLval.right() == initialValue + 2 &&
             resRval.right() == initialValue + 3 && resConstRval.right() == initialValue + 4;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("function reference and array extra arguments decay properly")
  {
    constexpr auto valid = [] {
      const Result rObj{frt::in_place_right, initialValue};
      const auto res = rObj | frt::and_then_right(freeFunctionAdd, step1Delta) |
                       frt::and_then_right(
                           [](int v, const char *str) -> Result {
                             return {frt::in_place_right, v + static_cast<int>(std::char_traits<char>::length(str))};
                           },
                           "abc");

      constexpr int expected = initialValue + step1Delta + 3;
      return res.right() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("constraints reject non-movable arguments")
  {
    using Fn = frt::either<std::string, int> (*)(int, NonMovable);
    STATIC_REQUIRE_FALSE(std::invocable<frt::detail::and_then_right_alg_::and_then_right_fn, Fn, NonMovable>);
  }
}
