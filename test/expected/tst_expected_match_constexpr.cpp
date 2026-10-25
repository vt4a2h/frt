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

constexpr auto makeExpectedValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedError()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}
constexpr auto makeExpectedMatchValueResult()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit"};
}
constexpr auto makeExpectedMatchErrorResult()
{
  return std::string{"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}

struct NothrowValueFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept { return makeExpectedMatchValueResult(); }
};

struct ThrowingValueFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept(false)
  {
    return makeExpectedMatchValueResult();
  }
};

struct NothrowErrorFn
{
  constexpr std::string operator()(const std::string &) const noexcept { return makeExpectedMatchErrorResult(); }
};

struct ThrowingErrorFn
{
  constexpr std::string operator()(const std::string &) const noexcept(false) { return makeExpectedMatchErrorResult(); }
};

struct NothrowVoidValueFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedMatchValueResult(); }
};

struct ThrowingVoidValueFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedMatchValueResult(); }
};

struct NothrowVoidReturnValueFn
{
  constexpr void operator()(const std::vector<int> &) const noexcept {}
};

struct ThrowingVoidReturnValueFn
{
  constexpr void operator()(const std::vector<int> &) const noexcept(false) {}
};

struct NothrowVoidReturnErrorFn
{
  constexpr void operator()(const std::string &) const noexcept {}
};

struct ThrowingVoidReturnErrorFn
{
  constexpr void operator()(const std::string &) const noexcept(false) {}
};

struct NothrowVoidReturnVoidValueFn
{
  constexpr void operator()() const noexcept {}
};

struct ThrowingVoidReturnVoidValueFn
{
  constexpr void operator()() const noexcept(false) {}
};

} // namespace

TEST_CASE("match constexpr", "[expected][constexpr]")
{
  auto matchVal = [](const std::vector<int> &) -> std::string { return makeExpectedMatchValueResult(); };
  auto matchErr = [](const std::string &) -> std::string { return makeExpectedMatchErrorResult(); };

  SECTION("value alternative invokes value function and returns its result")
  {
    constexpr auto valid = [matchVal, matchErr] {
      const Expected val{makeExpectedValue()};
      return val.match(matchVal, matchErr) == makeExpectedMatchValueResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative does not invoke error function")
  {
    constexpr auto valid = [matchVal] {
      const Expected val{makeExpectedValue()};
      bool errorInvoked = false;
      [[maybe_unused]] const auto res = val.match(matchVal, [&](const std::string &) {
        errorInvoked = true;
        return makeExpectedMatchErrorResult();
      });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative invokes error function and returns its result")
  {
    constexpr auto valid = [matchVal, matchErr] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      return err.match(matchVal, matchErr) == makeExpectedMatchErrorResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative does not invoke value function")
  {
    constexpr auto valid = [matchErr] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      [[maybe_unused]] const auto res = err.match(
          [&](const std::vector<int> &) {
            valueInvoked = true;
            return makeExpectedMatchValueResult();
          },
          matchErr);
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    constexpr auto valid = [] {
      const Expected val{makeExpectedValue()};
      bool valueInvoked = false;
      val.match([&](const std::vector<int> &) { valueInvoked = true; }, [](const std::string &) {});
      return valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function does not invoke error function")
  {
    constexpr auto valid = [] {
      const Expected val{makeExpectedValue()};
      bool errorInvoked = false;
      val.match([](const std::vector<int> &) {}, [&](const std::string &) { errorInvoked = true; });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function invokes error function")
  {
    constexpr auto valid = [] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      bool errorInvoked = false;
      err.match([](const std::vector<int> &) {}, [&](const std::string &) { errorInvoked = true; });
      return errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function does not invoke value function")
  {
    constexpr auto valid = [] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      err.match([&](const std::vector<int> &) { valueInvoked = true; }, [](const std::string &) {});
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      [[maybe_unused]] const auto res = expected.match(
          [](std::vector<int> &v) {
            v.clear();
            return 0;
          },
          [](std::string &) { return 0; });
      return expected.value().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      [[maybe_unused]] const auto res = expected.match([](std::vector<int> &) { return 0; },
                                                       [](std::string &s) {
                                                         s.clear();
                                                         return 0;
                                                       });
      return expected.error().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{makeExpectedValue()};
      return expected.match([](const std::vector<int> &v) { return v == makeExpectedValue(); },
                            [](const std::string &) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{frt::unexpected{makeExpectedError()}};
      return expected.match([](const std::vector<int> &) { return false; },
                            [](const std::string &s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      const auto res = std::move(expected).match([](std::vector<int> &&v) { return std::move(v); },
                                                 [](std::string &&) { return std::vector<int>{}; });
      return res == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      const auto res = std::move(expected).match([](std::vector<int> &&) { return std::string{}; },
                                                 [](std::string &&s) { return std::move(s); });
      return res == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{makeExpectedValue()};
      return static_cast<const Expected &&>(expected).match(
          [](const std::vector<int> &&v) { return v == makeExpectedValue(); },
          [](const std::string &&) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{frt::unexpected{makeExpectedError()}};
      return static_cast<const Expected &&>(expected).match(
          [](const std::vector<int> &&) { return false; },
          [](const std::string &&s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().match(NothrowValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when value function throws")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().match(ThrowingValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when error function throws")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().match(NothrowValueFn{}, ThrowingErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when both functions throw")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().match(ThrowingValueFn{}, ThrowingErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<Expected>().match(NothrowVoidReturnValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when value function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Expected>().match(ThrowingVoidReturnValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when error function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Expected>().match(NothrowVoidReturnValueFn{}, ThrowingVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("match (void value) constexpr", "[expected][constexpr]")
{
  auto matchVoidVal = []() -> std::string { return makeExpectedMatchValueResult(); };
  auto matchErr = [](const std::string &) -> std::string { return makeExpectedMatchErrorResult(); };

  SECTION("value alternative invokes value function with zero arguments and returns its result")
  {
    constexpr auto valid = [matchVoidVal, matchErr] {
      const ExpectedVoid val;
      return val.match(matchVoidVal, matchErr) == makeExpectedMatchValueResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative does not invoke error function")
  {
    constexpr auto valid = [matchVoidVal] {
      const ExpectedVoid val;
      bool errorInvoked = false;
      [[maybe_unused]] const auto res = val.match(matchVoidVal, [&](const std::string &) {
        errorInvoked = true;
        return makeExpectedMatchErrorResult();
      });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative invokes error function and returns its result")
  {
    constexpr auto valid = [matchVoidVal, matchErr] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      return err.match(matchVoidVal, matchErr) == makeExpectedMatchErrorResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative does not invoke value function")
  {
    constexpr auto valid = [matchErr] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      [[maybe_unused]] const auto res = err.match(
          [&] {
            valueInvoked = true;
            return makeExpectedMatchValueResult();
          },
          matchErr);
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid val;
      bool valueInvoked = false;
      val.match([&] { valueInvoked = true; }, [](const std::string &) {});
      return valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function does not invoke error function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid val;
      bool errorInvoked = false;
      val.match([] {}, [&](const std::string &) { errorInvoked = true; });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function invokes error function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      bool errorInvoked = false;
      err.match([] {}, [&](const std::string &) { errorInvoked = true; });
      return errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function does not invoke value function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      err.match([&] { valueInvoked = true; }, [](const std::string &) {});
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      [[maybe_unused]] const auto res = expected.match([] { return 0; },
                                                       [](std::string &s) {
                                                         s.clear();
                                                         return 0;
                                                       });
      return expected.error().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      return expected.match([] { return false; }, [](const std::string &s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      const auto res =
          std::move(expected).match([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
      return res == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      return static_cast<const ExpectedVoid &&>(expected).match(
          [] { return false; }, [](const std::string &&s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<ExpectedVoid>().match(NothrowVoidValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when value function throws")
  {
    constexpr auto valid = noexcept(std::declval<ExpectedVoid>().match(ThrowingVoidValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when error function throws")
  {
    constexpr auto valid = noexcept(std::declval<ExpectedVoid>().match(NothrowVoidValueFn{}, ThrowingErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<ExpectedVoid>().match(NothrowVoidReturnVoidValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when value function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<ExpectedVoid>().match(ThrowingVoidReturnVoidValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when error function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<ExpectedVoid>().match(NothrowVoidReturnVoidValueFn{}, ThrowingVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("Match with generic function for both value and error constexpr", "[expected][constexpr]")
{
  using namespace test::helpers;

  constexpr auto width = 10;
  constexpr auto height = 20;

  SECTION("generic square function")
  {
    constexpr auto valid = [] {
      const frt::expected<Widget, Window> expectedWindow{frt::unexpected{Window{.width = width, .height = height}}};
      const frt::expected<Widget, Window> expectedWidget{Widget{.width = width, .height = height}};

      auto square = [](const HasWidthAndHeight auto &obj) { return obj.width * obj.height; };

      return expectedWindow.match(square) == expectedWidget.match(square);
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("generic height modifier")
  {
    constexpr auto valid = [] {
      constexpr auto scaleFactor = 2;

      frt::expected<Widget, Window> expectedWindow{frt::unexpected{Window{.width = width, .height = height}}};
      frt::expected<Widget, Window> expectedWidget{Widget{.width = width, .height = height}};

      auto scaleHeight = [](HasWidthAndHeight auto &obj) { obj.height *= scaleFactor; };

      expectedWindow.match(scaleHeight);
      expectedWidget.match(scaleHeight);

      return expectedWindow.error().height == expectedWidget.value().height;
    }();

    STATIC_REQUIRE(valid);
  }
}
