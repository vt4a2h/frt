#include "test/helpers/types.hpp"

#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::vector<int>>;

constexpr auto makeExpectedValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedFoldValueResult()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}
constexpr auto makeExpectedFoldNullOptResult()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium"};
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

struct NothrowNullOptFn
{
  constexpr std::string operator()() const noexcept { return makeExpectedFoldNullOptResult(); }
};

struct ThrowingNullOptFn
{
  constexpr std::string operator()() const noexcept(false) { return makeExpectedFoldNullOptResult(); }
};

struct NothrowVoidReturnValueFn
{
  constexpr void operator()(const std::vector<int> &) const noexcept {}
};

struct ThrowingVoidReturnValueFn
{
  constexpr void operator()(const std::vector<int> &) const noexcept(false) {}
};

struct NothrowVoidReturnNullOptFn
{
  constexpr void operator()() const noexcept {}
};

struct ThrowingVoidReturnNullOptFn
{
  constexpr void operator()() const noexcept(false) {}
};

struct GenericAreaOrZero
{
  int defaultArea{0};
  constexpr int operator()(const test::helpers::Widget &w) const noexcept { return w.width * w.height; }
  constexpr int operator()() const noexcept { return defaultArea; }
};

struct GenericHeightModifier
{
  int scaleFactor{2};
  constexpr void operator()(test::helpers::Widget &w) const noexcept { w.height *= scaleFactor; }
  constexpr void operator()() const noexcept {}
};

} // namespace

TEST_CASE("fold constexpr", "[optional][constexpr]")
{
  auto foldVal = [](const std::vector<int> &) -> std::string { return makeExpectedFoldValueResult(); };
  auto foldNullOpt = []() -> std::string { return makeExpectedFoldNullOptResult(); };

  SECTION("value alternative invokes value function and returns its result")
  {
    constexpr auto valid = [foldVal, foldNullOpt] {
      const Opt val{makeExpectedValue()};
      return val.fold(foldVal, foldNullOpt) == makeExpectedFoldValueResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative does not invoke nullopt function")
  {
    constexpr auto valid = [foldVal] {
      const Opt val{makeExpectedValue()};
      bool nullOptInvoked = false;
      [[maybe_unused]] const auto res = val.fold(foldVal, [&] {
        nullOptInvoked = true;
        return makeExpectedFoldNullOptResult();
      });
      return !nullOptInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt alternative invokes nullopt function and returns its result")
  {
    constexpr auto valid = [foldVal, foldNullOpt] {
      const Opt nullOpt;
      return nullOpt.fold(foldVal, foldNullOpt) == makeExpectedFoldNullOptResult();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt alternative does not invoke value function")
  {
    constexpr auto valid = [foldNullOpt] {
      const Opt nullOpt;
      bool valueInvoked = false;
      [[maybe_unused]] const auto res = nullOpt.fold(
          [&](const std::vector<int> &) {
            valueInvoked = true;
            return makeExpectedFoldValueResult();
          },
          foldNullOpt);
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    constexpr auto valid = [] {
      const Opt val{makeExpectedValue()};
      bool valueInvoked = false;
      val.fold([&](const std::vector<int> &) { valueInvoked = true; }, [] {});
      return valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value alternative with void-returning function does not invoke nullopt function")
  {
    constexpr auto valid = [] {
      const Opt val{makeExpectedValue()};
      bool nullOptInvoked = false;
      val.fold([](const std::vector<int> &) {}, [&] { nullOptInvoked = true; });
      return !nullOptInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt alternative with void-returning function invokes nullopt function")
  {
    constexpr auto valid = [] {
      const Opt nullOpt;
      bool nullOptInvoked = false;
      nullOpt.fold([](const std::vector<int> &) {}, [&] { nullOptInvoked = true; });
      return nullOptInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt alternative with void-returning function does not invoke value function")
  {
    constexpr auto valid = [] {
      const Opt nullOpt;
      bool valueInvoked = false;
      nullOpt.fold([&](const std::vector<int> &) { valueInvoked = true; }, [] {});
      return !valueInvoked;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding allows modification")
  {
    constexpr auto valid = [] {
      Opt opt{makeExpectedValue()};
      [[maybe_unused]] const auto res = opt.fold(
          [](std::vector<int> &v) {
            v.clear();
            return 0;
          },
          [] { return 0; });
      return opt.value().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt{makeExpectedValue()};
      return opt.fold([](const std::vector<int> &v) { return v == makeExpectedValue(); }, [] { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return opt.fold([](const std::vector<int> &) { return false; }, [] { return true; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding moves value")
  {
    constexpr auto valid = [] {
      Opt opt{makeExpectedValue()};
      const auto res =
          std::move(opt).fold([](std::vector<int> &&v) { return std::move(v); }, [] { return std::vector<int>{}; });
      return res == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Opt opt;
      return std::move(opt).fold([](std::vector<int> &&) { return false; }, [] { return true; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt{makeExpectedValue()};
      return static_cast<const Opt &&>(opt).fold([](const std::vector<int> &&v) { return v == makeExpectedValue(); },
                                                 [] { return false; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return static_cast<const Opt &&>(opt).fold([](const std::vector<int> &&) { return false; }, [] { return true; });
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().fold(NothrowValueFn{}, NothrowNullOptFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when value function throws")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().fold(ThrowingValueFn{}, NothrowNullOptFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when nullopt function throws")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().fold(NothrowValueFn{}, ThrowingNullOptFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("not noexcept when both functions throw")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().fold(ThrowingValueFn{}, ThrowingNullOptFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().fold(NothrowVoidReturnValueFn{}, NothrowVoidReturnNullOptFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("void return not noexcept when value function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Opt>().fold(ThrowingVoidReturnValueFn{}, NothrowVoidReturnNullOptFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("void return not noexcept when nullopt function throws")
  {
    constexpr auto valid =
        noexcept(std::declval<Opt>().fold(NothrowVoidReturnValueFn{}, ThrowingVoidReturnNullOptFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}

TEST_CASE("Fold with single generic function constexpr", "[optional][constexpr]")
{
  using namespace test::helpers;

  constexpr auto initialWidth = 10;
  constexpr auto initialHeight = 20;
  constexpr auto defaultArea = 0;

  SECTION("generic area function for value")
  {
    constexpr auto valid = [] {
      const frt::optional<Widget> optVal{Widget{.width = initialWidth, .height = initialHeight}};
      const GenericAreaOrZero areaOrZero{defaultArea};
      return optVal.fold(areaOrZero) == initialWidth * initialHeight;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("generic area function for nullopt")
  {
    constexpr auto valid = [] {
      const frt::optional<Widget> optNullOpt;
      const GenericAreaOrZero areaOrZero{defaultArea};
      return optNullOpt.fold(areaOrZero) == defaultArea;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("generic height modifier for value")
  {
    constexpr auto valid = [] {
      constexpr auto scaleFactor = 2;
      frt::optional<Widget> opt{Widget{.width = initialWidth, .height = initialHeight}};
      const GenericHeightModifier modifyHeight{scaleFactor};

      opt.fold(modifyHeight);

      return opt.value().height == initialHeight * scaleFactor;
    }();

    STATIC_REQUIRE(valid);
  }
}
