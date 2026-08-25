#include "test/helpers/types.hpp"

#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/expected.hpp>

namespace
{

using Expected = frt::expected<std::vector<int>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

inline const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline const std::string expectedError =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline const std::string expectedFoldValueResult = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
inline const std::string expectedFoldErrorResult = "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua";

struct NothrowValueFn
{
  std::string operator()(const std::vector<int> &) const noexcept { return expectedFoldValueResult; }
};

struct ThrowingValueFn
{
  std::string operator()(const std::vector<int> &) const noexcept(false) { return expectedFoldValueResult; }
};

struct NothrowErrorFn
{
  std::string operator()(const std::string &) const noexcept { return expectedFoldErrorResult; }
};

struct ThrowingErrorFn
{
  std::string operator()(const std::string &) const noexcept(false) { return expectedFoldErrorResult; }
};

struct NothrowVoidValueFn
{
  std::string operator()() const noexcept { return expectedFoldValueResult; }
};

struct ThrowingVoidValueFn
{
  std::string operator()() const noexcept(false) { return expectedFoldValueResult; }
};

struct NothrowVoidReturnValueFn
{
  void operator()(const std::vector<int> &) const noexcept {}
};

struct ThrowingVoidReturnValueFn
{
  void operator()(const std::vector<int> &) const noexcept(false) {}
};

struct NothrowVoidReturnErrorFn
{
  void operator()(const std::string &) const noexcept {}
};

struct ThrowingVoidReturnErrorFn
{
  void operator()(const std::string &) const noexcept(false) {}
};

struct NothrowVoidReturnVoidValueFn
{
  void operator()() const noexcept {}
};

struct ThrowingVoidReturnVoidValueFn
{
  void operator()() const noexcept(false) {}
};

} // namespace

TEST_CASE("fold", "[expected]")
{
  auto foldVal = [](const std::vector<int> &) -> std::string { return expectedFoldValueResult; };
  auto foldErr = [](const std::string &) -> std::string { return expectedFoldErrorResult; };

  const Expected val{expectedValue};
  const Expected err{frt::unexpected{expectedError}};

  SECTION("value alternative invokes value function and returns its result")
  {
    REQUIRE(val.fold(foldVal, foldErr) == expectedFoldValueResult);
  }

  SECTION("value alternative does not invoke error function")
  {
    bool errorInvoked = false;
    [[maybe_unused]] const auto res = val.fold(foldVal, [&](const std::string &) {
      errorInvoked = true;
      return expectedFoldErrorResult;
    });
    REQUIRE_FALSE(errorInvoked);
  }

  SECTION("error alternative invokes error function and returns its result")
  {
    REQUIRE(err.fold(foldVal, foldErr) == expectedFoldErrorResult);
  }

  SECTION("error alternative does not invoke value function")
  {
    bool valueInvoked = false;
    [[maybe_unused]] const auto res = err.fold(
        [&](const std::vector<int> &) {
          valueInvoked = true;
          return expectedFoldValueResult;
        },
        foldErr);
    REQUIRE_FALSE(valueInvoked);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    bool valueInvoked = false;
    val.fold([&](const std::vector<int> &) { valueInvoked = true; }, [](const std::string &) {});
    REQUIRE(valueInvoked);
  }

  SECTION("value alternative with void-returning function does not invoke error function")
  {
    bool errorInvoked = false;
    val.fold([](const std::vector<int> &) {}, [&](const std::string &) { errorInvoked = true; });
    REQUIRE_FALSE(errorInvoked);
  }

  SECTION("error alternative with void-returning function invokes error function")
  {
    bool errorInvoked = false;
    err.fold([](const std::vector<int> &) {}, [&](const std::string &) { errorInvoked = true; });
    REQUIRE(errorInvoked);
  }

  SECTION("error alternative with void-returning function does not invoke value function")
  {
    bool valueInvoked = false;
    err.fold([&](const std::vector<int> &) { valueInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(valueInvoked);
  }

  SECTION("value with non-const lvalue forwarding allows modification")
  {
    Expected expected{expectedValue};
    [[maybe_unused]] const auto res = expected.fold(
        [](std::vector<int> &v) {
          v.clear();
          return 0;
        },
        [](std::string &) { return 0; });
    REQUIRE(expected.value().empty());
  }

  SECTION("error with non-const lvalue forwarding allows modification")
  {
    Expected expected{frt::unexpected{expectedError}};
    [[maybe_unused]] const auto res = expected.fold([](std::vector<int> &) { return 0; },
                                                    [](std::string &s) {
                                                      s.clear();
                                                      return 0;
                                                    });
    REQUIRE(expected.error().empty());
  }

  SECTION("value with const lvalue forwarding")
  {
    const Expected expected{expectedValue};
    const auto res = expected.fold([](const std::vector<int> &v) { return v == expectedValue; },
                                   [](const std::string &) { return false; });
    REQUIRE(res);
  }

  SECTION("error with const lvalue forwarding")
  {
    const Expected expected{frt::unexpected{expectedError}};
    const auto res = expected.fold([](const std::vector<int> &) { return false; },
                                   [](const std::string &s) { return s == expectedError; });
    REQUIRE(res);
  }

  SECTION("value with rvalue forwarding moves value")
  {
    Expected expected{expectedValue};
    const auto res = std::move(expected).fold([](std::vector<int> &&v) { return std::move(v); },
                                              [](std::string &&) { return std::vector<int>{}; });
    REQUIRE(res == expectedValue);
  }

  SECTION("error with rvalue forwarding moves value")
  {
    Expected expected{frt::unexpected{expectedError}};
    const auto res = std::move(expected).fold([](std::vector<int> &&) { return std::string{}; },
                                              [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedError);
  }

  SECTION("value with const rvalue forwarding")
  {
    const Expected expected{expectedValue};
    const auto res = static_cast<const Expected &&>(expected).fold(
        [](const std::vector<int> &&v) { return v == expectedValue; }, [](const std::string &&) { return false; });
    REQUIRE(res);
  }

  SECTION("error with const rvalue forwarding")
  {
    const Expected expected{frt::unexpected{expectedError}};
    const auto res = static_cast<const Expected &&>(expected).fold(
        [](const std::vector<int> &&) { return false; }, [](const std::string &&s) { return s == expectedError; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Expected>().fold(NothrowValueFn{}, NothrowErrorFn{})));
  }

  SECTION("not noexcept when value function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Expected>().fold(ThrowingValueFn{}, NothrowErrorFn{})));
  }

  SECTION("not noexcept when error function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Expected>().fold(NothrowValueFn{}, ThrowingErrorFn{})));
  }

  SECTION("not noexcept when both functions throw")
  {
    REQUIRE_FALSE(noexcept(std::declval<Expected>().fold(ThrowingValueFn{}, ThrowingErrorFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Expected>().fold(NothrowVoidReturnValueFn{}, NothrowVoidReturnErrorFn{})));
  }

  SECTION("void return not noexcept when value function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Expected>().fold(ThrowingVoidReturnValueFn{}, NothrowVoidReturnErrorFn{})));
  }

  SECTION("void return not noexcept when error function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Expected>().fold(NothrowVoidReturnValueFn{}, ThrowingVoidReturnErrorFn{})));
  }
}

TEST_CASE("fold (void value)", "[expected]")
{
  auto foldVoidVal = []() -> std::string { return expectedFoldValueResult; };
  auto foldErr = [](const std::string &) -> std::string { return expectedFoldErrorResult; };

  const ExpectedVoid val;
  const ExpectedVoid err{frt::unexpected{expectedError}};

  SECTION("value alternative invokes value function with zero arguments and returns its result")
  {
    REQUIRE(val.fold(foldVoidVal, foldErr) == expectedFoldValueResult);
  }

  SECTION("value alternative does not invoke error function")
  {
    bool errorInvoked = false;
    [[maybe_unused]] const auto res = val.fold(foldVoidVal, [&](const std::string &) {
      errorInvoked = true;
      return expectedFoldErrorResult;
    });
    REQUIRE_FALSE(errorInvoked);
  }

  SECTION("error alternative invokes error function and returns its result")
  {
    REQUIRE(err.fold(foldVoidVal, foldErr) == expectedFoldErrorResult);
  }

  SECTION("error alternative does not invoke value function")
  {
    bool valueInvoked = false;
    [[maybe_unused]] const auto res = err.fold(
        [&] {
          valueInvoked = true;
          return expectedFoldValueResult;
        },
        foldErr);
    REQUIRE_FALSE(valueInvoked);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    bool valueInvoked = false;
    val.fold([&] { valueInvoked = true; }, [](const std::string &) {});
    REQUIRE(valueInvoked);
  }

  SECTION("value alternative with void-returning function does not invoke error function")
  {
    bool errorInvoked = false;
    val.fold([] {}, [&](const std::string &) { errorInvoked = true; });
    REQUIRE_FALSE(errorInvoked);
  }

  SECTION("error alternative with void-returning function invokes error function")
  {
    bool errorInvoked = false;
    err.fold([] {}, [&](const std::string &) { errorInvoked = true; });
    REQUIRE(errorInvoked);
  }

  SECTION("error alternative with void-returning function does not invoke value function")
  {
    bool valueInvoked = false;
    err.fold([&] { valueInvoked = true; }, [](const std::string &) {});
    REQUIRE_FALSE(valueInvoked);
  }

  SECTION("error with non-const lvalue forwarding allows modification")
  {
    ExpectedVoid expected{frt::unexpected{expectedError}};
    [[maybe_unused]] const auto res = expected.fold([] { return 0; },
                                                    [](std::string &s) {
                                                      s.clear();
                                                      return 0;
                                                    });
    REQUIRE(expected.error().empty());
  }

  SECTION("error with const lvalue forwarding")
  {
    const ExpectedVoid expected{frt::unexpected{expectedError}};
    const auto res = expected.fold([] { return false; }, [](const std::string &s) { return s == expectedError; });
    REQUIRE(res);
  }

  SECTION("error with rvalue forwarding moves value")
  {
    ExpectedVoid expected{frt::unexpected{expectedError}};
    const auto res =
        std::move(expected).fold([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
    REQUIRE(res == expectedError);
  }

  SECTION("error with const rvalue forwarding")
  {
    const ExpectedVoid expected{frt::unexpected{expectedError}};
    const auto res = static_cast<const ExpectedVoid &&>(expected).fold(
        [] { return false; }, [](const std::string &&s) { return s == expectedError; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidValueFn{}, NothrowErrorFn{})));
  }

  SECTION("not noexcept when value function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<ExpectedVoid>().fold(ThrowingVoidValueFn{}, NothrowErrorFn{})));
  }

  SECTION("not noexcept when error function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidValueFn{}, ThrowingErrorFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidReturnVoidValueFn{}, NothrowVoidReturnErrorFn{})));
  }

  SECTION("void return not noexcept when value function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<ExpectedVoid>().fold(ThrowingVoidReturnVoidValueFn{}, NothrowVoidReturnErrorFn{})));
  }

  SECTION("void return not noexcept when error function throws")
  {
    REQUIRE_FALSE(
        noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidReturnVoidValueFn{}, ThrowingVoidReturnErrorFn{})));
  }
}

TEST_CASE("Fold with generic function for both value and error", "[expected]")
{
  using namespace test::helpers;

  constexpr auto width = 10;
  constexpr auto height = 20;

  frt::expected<Widget, Window> expectedWindow{frt::unexpected{Window{.width = width, .height = height}}};
  frt::expected<Widget, Window> expectedWidget{Widget{.width = width, .height = height}};

  SECTION("generic square function")
  {
    auto square = [](const HasWidthAndHeight auto &obj) { return obj.width * obj.height; };

    REQUIRE(expectedWindow.fold(square) == expectedWidget.fold(square));
  }

  SECTION("generic height modifier")
  {
    constexpr auto scaleFactor = 2;
    auto scaleHeight = [](HasWidthAndHeight auto &obj) { obj.height *= scaleFactor; };

    expectedWindow.fold(scaleHeight);
    expectedWidget.fold(scaleHeight);

    REQUIRE(expectedWindow.error().height == expectedWidget.value().height);
  }
}
