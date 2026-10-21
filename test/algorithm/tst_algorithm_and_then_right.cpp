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

inline const std::string expectedTransformedValue =
    "consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";

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

TEST_CASE("and_then_right partial application", "[algorithm][and_then_right]")
{
  const auto transform = [](const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  const auto transformToVoid = [](const std::vector<int> &) -> frt::either<std::string, void> {
    return {frt::in_place_right};
  };
  const auto transformExtra = [](const std::vector<int> &, int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  const auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };

  const Either left{frt::in_place_left, test::helpers::expectedStringValue};
  const Either right{frt::in_place_right, test::helpers::expectedVectorValue};

  SECTION("right to different right type without extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transform);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type without extra args (value)")
  {
    const auto res = right | frt::and_then_right(transform);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to void right without extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transformToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (is_right)")
  {
    const auto res = right | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with extra args without unwrapped (value)")
  {
    const auto res = right | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right to different right type with unwrapped in extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.is_right());
  }

  SECTION("right to different right type with unwrapped in extra args (value)")
  {
    const auto res = right | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("right with non-const lvalue result forwarding")
  {
    Either either{frt::in_place_right, test::helpers::expectedVectorValue};
    const auto fn = [](std::vector<int> &v) -> frt::either<std::string, bool> {
      v.clear();
      return {frt::in_place_right, true};
    };
    [[maybe_unused]] const auto res = either | frt::and_then_right(fn);
    REQUIRE(either.right().empty());
  }

  SECTION("right with const lvalue result forwarding")
  {
    const Either either{frt::in_place_right, test::helpers::expectedVectorValue};
    const auto fn = [](const std::vector<int> &v) -> frt::either<std::string, bool> {
      return {frt::in_place_right, v == test::helpers::expectedVectorValue};
    };
    const auto res = either | frt::and_then_right(fn);
    REQUIRE(res.right());
  }

  SECTION("right with rvalue result forwarding")
  {
    Either either{frt::in_place_right, test::helpers::expectedVectorValue};
    const auto fn = [](std::vector<int> &&v) -> frt::either<std::string, std::vector<int>> {
      return {frt::in_place_right, std::move(v)};
    };
    const auto res = std::move(either) | frt::and_then_right(fn);
    REQUIRE(res.right() == test::helpers::expectedVectorValue);
  }

  SECTION("left without extra args (is_left)")
  {
    const auto res = left | frt::and_then_right(transform);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    const auto res = left | frt::and_then_right(transform);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    const auto res = left | frt::and_then_right(transformToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (is_left)")
  {
    const auto res = left | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args without unwrapped (value)")
  {
    const auto res = left | frt::and_then_right(transformExtra, test::helpers::expectedIntValue);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }

  SECTION("left with unwrapped in extra args (is_left)")
  {
    const auto res = left | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.is_left());
  }

  SECTION("left with unwrapped in extra args (value)")
  {
    const auto res = left | frt::and_then_right(transformUnwrapped, test::helpers::expectedIntValue, frt::unwrapped);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }
}

TEST_CASE("and_then_right partial application (void right)", "[algorithm][and_then_right]")
{
  const auto transformVoidRight = []() -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  const auto transformVoidToVoid = []() -> frt::either<std::string, void> { return {frt::in_place_right}; };
  const auto transformVoidExtra = [](int) -> frt::either<std::string, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  const auto transformVoidToVoidExtra = [](int) -> frt::either<std::string, void> { return {frt::in_place_right}; };

  const EitherVoidRight left{frt::in_place_left, test::helpers::expectedStringValue};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("void right to non-void right without extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transformVoidRight);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right without extra args (value)")
  {
    const auto res = right | frt::and_then_right(transformVoidRight);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right without extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transformVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_right());
  }

  SECTION("void right to non-void right with extra args (value)")
  {
    const auto res = right | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right to void right with extra args (is_right)")
  {
    const auto res = right | frt::and_then_right(transformVoidToVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_right());
  }

  SECTION("left without extra args (is_left)")
  {
    const auto res = left | frt::and_then_right(transformVoidRight);
    REQUIRE(res.is_left());
  }

  SECTION("left without extra args (value)")
  {
    const auto res = left | frt::and_then_right(transformVoidRight);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }

  SECTION("left with void right return type without extra args (is_left)")
  {
    const auto res = left | frt::and_then_right(transformVoidToVoid);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (is_left)")
  {
    const auto res = left | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.is_left());
  }

  SECTION("left with extra args (value)")
  {
    const auto res = left | frt::and_then_right(transformVoidExtra, test::helpers::expectedIntValue);
    REQUIRE(res.left() == test::helpers::expectedStringValue);
  }
}

TEST_CASE("and_then_right partial application (void left)", "[algorithm][and_then_right]")
{
  const auto transformVoidLeft = [](const std::vector<int> &) -> frt::either<void, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  const auto transformVoidVoidToNonVoid = []() -> frt::either<void, std::string> {
    return {frt::in_place_right, expectedTransformedValue};
  };
  const auto transformVoidVoidToVoid = []() -> frt::either<void, void> { return {frt::in_place_right}; };

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, test::helpers::expectedVectorValue};
  const EitherVoidVoid voidVoidLeft{frt::in_place_left};
  const EitherVoidVoid voidVoidRight{frt::in_place_right};

  SECTION("non-void right with void left (is_right)")
  {
    const auto res = right | frt::and_then_right(transformVoidLeft);
    REQUIRE(res.is_right());
  }

  SECTION("non-void right with void left (value)")
  {
    const auto res = right | frt::and_then_right(transformVoidLeft);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void left from non-void right containing left (is_left)")
  {
    const auto res = left | frt::and_then_right(transformVoidLeft);
    REQUIRE(res.is_left());
  }

  SECTION("void right with void left to non-void right (is_right)")
  {
    const auto res = voidVoidRight | frt::and_then_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void right with void left to non-void right (value)")
  {
    const auto res = voidVoidRight | frt::and_then_right(transformVoidVoidToNonVoid);
    REQUIRE(res.right() == expectedTransformedValue);
  }

  SECTION("void right with void left to void right (is_right)")
  {
    const auto res = voidVoidRight | frt::and_then_right(transformVoidVoidToVoid);
    REQUIRE(res.is_right());
  }

  SECTION("void left from void right containing left (is_left)")
  {
    const auto res = voidVoidLeft | frt::and_then_right(transformVoidVoidToNonVoid);
    REQUIRE(res.is_left());
  }
}

TEST_CASE("and_then_right partial application closure semantics and efficiency", "[algorithm][and_then_right]")
{
  using Result = frt::either<std::string, int>;
  constexpr int initialValue = 10;
  constexpr int step1Delta = 5;
  constexpr int step2Multiplier = 2;

  SECTION("chaining multiple partial applications")
  {
    const Result start{frt::in_place_right, initialValue};
    const auto res =
        start | frt::and_then_right([](int v, int d) -> Result { return {frt::in_place_right, v + d}; }, step1Delta) |
        frt::and_then_right([](int m, int v) -> Result { return {frt::in_place_right, v * m}; }, step2Multiplier,
                            frt::unwrapped);

    constexpr int expected = (initialValue + step1Delta) * step2Multiplier;
    REQUIRE(res.right() == expected);
  }

  SECTION("chaining short-circuits on left")
  {
    int secondCallCount = 0;
    const Result start{frt::in_place_right, initialValue};
    [[maybe_unused]] const auto res =
        start |
        frt::and_then_right([](int) -> Result { return {frt::in_place_left, test::helpers::expectedStringValue}; }) |
        frt::and_then_right([&secondCallCount](int v) -> Result {
          ++secondCallCount;
          return {frt::in_place_right, v};
        });

    REQUIRE(secondCallCount == 0);
  }

  SECTION("std::ref extra argument avoids copies and allows mutation")
  {
    int accumulator = initialValue;
    const Result rObj{frt::in_place_right, step1Delta};

    [[maybe_unused]] const auto res = rObj | frt::and_then_right(
                                                 [](int v, int &acc) -> Result {
                                                   acc += v;
                                                   return {frt::in_place_right, acc};
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

    const auto res =
        rObj | frt::and_then_right(
                   [](int v, const CopyMoveTracker &t) -> Result { return {frt::in_place_right, v + t.value}; },
                   std::cref(tracker));

    REQUIRE((copies == 0 && moves == 0 && res.right() == initialValue + step1Delta));
  }

  SECTION("lvalue closure can be reused multiple times without moving captured args")
  {
    const auto closure = frt::and_then_right(
        [](int v, const std::string &suffix) -> frt::either<int, std::string> {
          return {frt::in_place_right, std::string(static_cast<std::size_t>(v), 'a') + suffix};
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
    const auto closure = frt::and_then_right(
        [](int v, const CopyMoveTracker &t) -> Result { return {frt::in_place_right, v + t.value}; },
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
        rObj |
        frt::and_then_right([](int v, CopyMoveTracker &&t) -> Result { return {frt::in_place_right, v + t.value}; },
                            CopyMoveTracker{initialValue, &copies, &moves});

    REQUIRE(copies == 0);
  }

  SECTION("move-only callable and move-only extra argument with rvalue closure")
  {
    const Result rObj{frt::in_place_right, initialValue};
    auto closure =
        frt::and_then_right([](int v, std::unique_ptr<int> ptr) -> Result { return {frt::in_place_right, v + *ptr}; },
                            std::make_unique<int>(step1Delta));

    const auto res = rObj | std::move(closure);
    constexpr int expected = initialValue + step1Delta;
    REQUIRE(res.right() == expected);
  }

  SECTION("move-only callable object with rvalue closure")
  {
    const Result rObj{frt::in_place_right, initialValue};
    const auto res = rObj | frt::and_then_right(MoveOnlyCallable{step2Multiplier});

    constexpr int expected = initialValue * step2Multiplier;
    REQUIRE(res.right() == expected);
  }

  SECTION("closure value category forwards to callable (&, const&, &&, const&&)")
  {
    const Result rObj{frt::in_place_right, initialValue};
    auto closure = frt::and_then_right(QualAwareCallable{});
    const auto constClosure = frt::and_then_right(QualAwareCallable{});

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
    const auto res = rObj | frt::and_then_right(freeFunctionAdd, step1Delta) |
                     frt::and_then_right(
                         [](int v, const char *str) -> Result {
                           return {frt::in_place_right, v + static_cast<int>(std::char_traits<char>::length(str))};
                         },
                         "abc");

    constexpr int expected = initialValue + step1Delta + 3;
    REQUIRE(res.right() == expected);
  }

  SECTION("constraints reject non-movable arguments")
  {
    using Fn = frt::either<std::string, int> (*)(int, NonMovable);
    STATIC_REQUIRE_FALSE(std::invocable<frt::detail::and_then_right_alg_::and_then_right_fn, Fn, NonMovable>);
  }
}
