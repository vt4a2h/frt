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
constexpr auto makeExpectedFoldValueResult()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit"};
}
constexpr auto makeExpectedFoldErrorResult()
{
  return std::string{"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua"};
}

struct NothrowValueFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept { return makeExpectedFoldValueResult(); }
};

struct ThrowingValueFn
{
  constexpr std::string operator()(const std::vector<int> &) const noexcept(false)
  {
    return makeExpectedFoldValueResult();
  }
};

struct NothrowErrorFn
{
  constexpr std::string operator()(const std::string &) const noexcept { return makeExpectedFoldErrorResult(); }
};

struct ThrowingErrorFn
{
  constexpr std::string operator()(const std::string &) const noexcept(false) { return makeExpectedFoldErrorResult(); }
};

struct NothrowVoidValueFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedFoldValueResult(); }
};

struct ThrowingVoidValueFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedFoldValueResult(); }
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

TEST_CASE("fold constexpr", "[expected][constexpr]")
{
  auto foldVal = [](const std::vector<int> &) -> std::string { return makeExpectedFoldValueResult(); };
  auto foldErr = [](const std::string &) -> std::string { return makeExpectedFoldErrorResult(); };

  SECTION("value alternative invokes value function and returns its result")
  {
    constexpr auto valid = [foldVal, foldErr] {
      const Expected val{makeExpectedValue()};
      return val.fold(foldVal, foldErr) == makeExpectedFoldValueResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative does not invoke error function")
  {
    constexpr auto valid = [foldVal] {
      const Expected val{makeExpectedValue()};
      bool errorInvoked = false;
      [[maybe_unused]] const auto res = val.fold(foldVal, [&](const std::string &) {
        errorInvoked = true;
        return makeExpectedFoldErrorResult();
      });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative invokes error function and returns its result")
  {
    constexpr auto valid = [foldVal, foldErr] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      return err.fold(foldVal, foldErr) == makeExpectedFoldErrorResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative does not invoke value function")
  {
    constexpr auto valid = [foldErr] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      [[maybe_unused]] const auto res = err.fold(
          [&](const std::vector<int> &) {
            valueInvoked = true;
            return makeExpectedFoldValueResult();
          },
          foldErr);
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    constexpr auto valid = [] {
      const Expected val{makeExpectedValue()};
      bool valueInvoked = false;
      val.fold([&](const std::vector<int> &) { valueInvoked = true; }, [](const std::string &) {});
      return valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function does not invoke error function")
  {
    constexpr auto valid = [] {
      const Expected val{makeExpectedValue()};
      bool errorInvoked = false;
      val.fold([](const std::vector<int> &) {}, [&](const std::string &) { errorInvoked = true; });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function invokes error function")
  {
    constexpr auto valid = [] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      bool errorInvoked = false;
      err.fold([](const std::vector<int> &) {}, [&](const std::string &) { errorInvoked = true; });
      return errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function does not invoke value function")
  {
    constexpr auto valid = [] {
      const Expected err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      err.fold([&](const std::vector<int> &) { valueInvoked = true; }, [](const std::string &) {});
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      [[maybe_unused]] const auto res = expected.fold(
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
      [[maybe_unused]] const auto res = expected.fold([](std::vector<int> &) { return 0; },
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
      return expected.fold([](const std::vector<int> &v) { return v == makeExpectedValue(); },
                           [](const std::string &) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{frt::unexpected{makeExpectedError()}};
      return expected.fold([](const std::vector<int> &) { return false; },
                           [](const std::string &s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Expected expected{makeExpectedValue()};
      const auto res = std::move(expected).fold([](std::vector<int> &&v) { return std::move(v); },
                                                [](std::string &&) { return std::vector<int>{}; });
      return res == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      const auto res = std::move(expected).fold([](std::vector<int> &&) { return std::string{}; },
                                                [](std::string &&s) { return std::move(s); });
      return res == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{makeExpectedValue()};
      return static_cast<const Expected &&>(expected).fold(
          [](const std::vector<int> &&v) { return v == makeExpectedValue(); },
          [](const std::string &&) { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Expected expected{frt::unexpected{makeExpectedError()}};
      return static_cast<const Expected &&>(expected).fold(
          [](const std::vector<int> &&) { return false; },
          [](const std::string &&s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().fold(NothrowValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when value function throws")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().fold(ThrowingValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when error function throws")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().fold(NothrowValueFn{}, ThrowingErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when both functions throw")
  {
    constexpr auto valid = noexcept(std::declval<Expected>().fold(ThrowingValueFn{}, ThrowingErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<Expected>().fold(NothrowVoidReturnValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when value function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Expected>().fold(ThrowingVoidReturnValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when error function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Expected>().fold(NothrowVoidReturnValueFn{}, ThrowingVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("fold (void value) constexpr", "[expected][constexpr]")
{
  auto foldVoidVal = []() -> std::string { return makeExpectedFoldValueResult(); };
  auto foldErr = [](const std::string &) -> std::string { return makeExpectedFoldErrorResult(); };

  SECTION("value alternative invokes value function with zero arguments and returns its result")
  {
    constexpr auto valid = [foldVoidVal, foldErr] {
      const ExpectedVoid val;
      return val.fold(foldVoidVal, foldErr) == makeExpectedFoldValueResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative does not invoke error function")
  {
    constexpr auto valid = [foldVoidVal] {
      const ExpectedVoid val;
      bool errorInvoked = false;
      [[maybe_unused]] const auto res = val.fold(foldVoidVal, [&](const std::string &) {
        errorInvoked = true;
        return makeExpectedFoldErrorResult();
      });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative invokes error function and returns its result")
  {
    constexpr auto valid = [foldVoidVal, foldErr] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      return err.fold(foldVoidVal, foldErr) == makeExpectedFoldErrorResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative does not invoke value function")
  {
    constexpr auto valid = [foldErr] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      [[maybe_unused]] const auto res = err.fold(
          [&] {
            valueInvoked = true;
            return makeExpectedFoldValueResult();
          },
          foldErr);
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid val;
      bool valueInvoked = false;
      val.fold([&] { valueInvoked = true; }, [](const std::string &) {});
      return valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function does not invoke error function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid val;
      bool errorInvoked = false;
      val.fold([] {}, [&](const std::string &) { errorInvoked = true; });
      return !errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function invokes error function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      bool errorInvoked = false;
      err.fold([] {}, [&](const std::string &) { errorInvoked = true; });
      return errorInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error alternative with void-returning function does not invoke value function")
  {
    constexpr auto valid = [] {
      const ExpectedVoid err{frt::unexpected{makeExpectedError()}};
      bool valueInvoked = false;
      err.fold([&] { valueInvoked = true; }, [](const std::string &) {});
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      [[maybe_unused]] const auto res = expected.fold([] { return 0; },
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
      return expected.fold([] { return false; }, [](const std::string &s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      const auto res =
          std::move(expected).fold([] { return std::string{}; }, [](std::string &&s) { return std::move(s); });
      return res == makeExpectedError();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("error with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      return static_cast<const ExpectedVoid &&>(expected).fold(
          [] { return false; }, [](const std::string &&s) { return s == makeExpectedError(); });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when value function throws")
  {
    constexpr auto valid = noexcept(std::declval<ExpectedVoid>().fold(ThrowingVoidValueFn{}, NothrowErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when error function throws")
  {
    constexpr auto valid = noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidValueFn{}, ThrowingErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid =
        noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidReturnVoidValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when value function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<ExpectedVoid>().fold(ThrowingVoidReturnVoidValueFn{}, NothrowVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when error function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<ExpectedVoid>().fold(NothrowVoidReturnVoidValueFn{}, ThrowingVoidReturnErrorFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("Fold with generic function for both value and error constexpr", "[expected][constexpr]")
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

      return expectedWindow.fold(square) == expectedWidget.fold(square);
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

      expectedWindow.fold(scaleHeight);
      expectedWidget.fold(scaleHeight);

      return expectedWindow.error().height == expectedWidget.value().height;
    }();

    STATIC_REQUIRE(valid);
  }
}
