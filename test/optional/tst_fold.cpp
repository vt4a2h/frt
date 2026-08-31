#include "test/helpers/types.hpp"

#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::vector<int>>;

inline const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline const std::string expectedFoldValueResult =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
    "magna aliqua";
inline const std::string expectedFoldNullOptResult =
    "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium";

struct NothrowValueFn
{
  std::string operator()(const std::vector<int> &) const noexcept { return expectedFoldValueResult; }
};

struct ThrowingValueFn
{
  std::string operator()(const std::vector<int> &) const noexcept(false) { return expectedFoldValueResult; }
};

struct NothrowNullOptFn
{
  std::string operator()() const noexcept { return expectedFoldNullOptResult; }
};

struct ThrowingNullOptFn
{
  std::string operator()() const noexcept(false) { return expectedFoldNullOptResult; }
};

struct NothrowVoidReturnValueFn
{
  void operator()(const std::vector<int> &) const noexcept {}
};

struct ThrowingVoidReturnValueFn
{
  void operator()(const std::vector<int> &) const noexcept(false) {}
};

struct NothrowVoidReturnNullOptFn
{
  void operator()() const noexcept {}
};

struct ThrowingVoidReturnNullOptFn
{
  void operator()() const noexcept(false) {}
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

TEST_CASE("fold", "[optional]")
{
  auto foldVal = [](const std::vector<int> &) -> std::string { return expectedFoldValueResult; };
  auto foldNullOpt = []() -> std::string { return expectedFoldNullOptResult; };

  const Opt val{expectedValue};
  const Opt nullOpt;

  SECTION("value alternative invokes value function and returns its result")
  {
    REQUIRE(val.fold(foldVal, foldNullOpt) == expectedFoldValueResult);
  }

  SECTION("value alternative does not invoke nullopt function")
  {
    bool nullOptInvoked = false;
    [[maybe_unused]] const auto res = val.fold(foldVal, [&] {
      nullOptInvoked = true;
      return expectedFoldNullOptResult;
    });
    REQUIRE_FALSE(nullOptInvoked);
  }

  SECTION("nullopt alternative invokes nullopt function and returns its result")
  {
    REQUIRE(nullOpt.fold(foldVal, foldNullOpt) == expectedFoldNullOptResult);
  }

  SECTION("nullopt alternative does not invoke value function")
  {
    bool valueInvoked = false;
    [[maybe_unused]] const auto res = nullOpt.fold(
        [&](const std::vector<int> &) {
          valueInvoked = true;
          return expectedFoldValueResult;
        },
        foldNullOpt);
    REQUIRE_FALSE(valueInvoked);
  }

  SECTION("value alternative with void-returning function invokes value function")
  {
    bool valueInvoked = false;
    val.fold([&](const std::vector<int> &) { valueInvoked = true; }, [] {});
    REQUIRE(valueInvoked);
  }

  SECTION("value alternative with void-returning function does not invoke nullopt function")
  {
    bool nullOptInvoked = false;
    val.fold([](const std::vector<int> &) {}, [&] { nullOptInvoked = true; });
    REQUIRE_FALSE(nullOptInvoked);
  }

  SECTION("nullopt alternative with void-returning function invokes nullopt function")
  {
    bool nullOptInvoked = false;
    nullOpt.fold([](const std::vector<int> &) {}, [&] { nullOptInvoked = true; });
    REQUIRE(nullOptInvoked);
  }

  SECTION("nullopt alternative with void-returning function does not invoke value function")
  {
    bool valueInvoked = false;
    nullOpt.fold([&](const std::vector<int> &) { valueInvoked = true; }, [] {});
    REQUIRE_FALSE(valueInvoked);
  }

  SECTION("value with non-const lvalue forwarding allows modification")
  {
    Opt opt{expectedValue};
    [[maybe_unused]] const auto res = opt.fold(
        [](std::vector<int> &v) {
          v.clear();
          return 0;
        },
        [] { return 0; });
    REQUIRE(opt.value().empty());
  }

  SECTION("value with const lvalue forwarding")
  {
    const Opt opt{expectedValue};
    const auto res = opt.fold([](const std::vector<int> &v) { return v == expectedValue; }, [] { return false; });
    REQUIRE(res);
  }

  SECTION("nullopt with const lvalue forwarding")
  {
    const Opt opt;
    const auto res = opt.fold([](const std::vector<int> &) { return false; }, [] { return true; });
    REQUIRE(res);
  }

  SECTION("value with rvalue forwarding moves value")
  {
    Opt opt{expectedValue};
    const auto res =
        std::move(opt).fold([](std::vector<int> &&v) { return std::move(v); }, [] { return std::vector<int>{}; });
    REQUIRE(res == expectedValue);
  }

  SECTION("nullopt with rvalue forwarding")
  {
    Opt opt;
    const auto res = std::move(opt).fold([](std::vector<int> &&) { return false; }, [] { return true; });
    REQUIRE(res);
  }

  SECTION("value with const rvalue forwarding")
  {
    const Opt opt{expectedValue};
    const auto res = static_cast<const Opt &&>(opt).fold([](const std::vector<int> &&v) { return v == expectedValue; },
                                                         [] { return false; });
    REQUIRE(res);
  }

  SECTION("nullopt with const rvalue forwarding")
  {
    const Opt opt;
    const auto res =
        static_cast<const Opt &&>(opt).fold([](const std::vector<int> &&) { return false; }, [] { return true; });
    REQUIRE(res);
  }

  SECTION("noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Opt>().fold(NothrowValueFn{}, NothrowNullOptFn{})));
  }

  SECTION("not noexcept when value function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().fold(ThrowingValueFn{}, NothrowNullOptFn{})));
  }

  SECTION("not noexcept when nullopt function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().fold(NothrowValueFn{}, ThrowingNullOptFn{})));
  }

  SECTION("not noexcept when both functions throw")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().fold(ThrowingValueFn{}, ThrowingNullOptFn{})));
  }

  SECTION("void return noexcept when both functions are noexcept")
  {
    REQUIRE(noexcept(std::declval<Opt>().fold(NothrowVoidReturnValueFn{}, NothrowVoidReturnNullOptFn{})));
  }

  SECTION("void return not noexcept when value function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().fold(ThrowingVoidReturnValueFn{}, NothrowVoidReturnNullOptFn{})));
  }

  SECTION("void return not noexcept when nullopt function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().fold(NothrowVoidReturnValueFn{}, ThrowingVoidReturnNullOptFn{})));
  }
}

TEST_CASE("Fold with single generic function", "[optional]")
{
  using namespace test::helpers;

  constexpr auto initialWidth = 10;
  constexpr auto initialHeight = 20;
  constexpr auto defaultArea = 0;

  const frt::optional optVal{Widget{.width = initialWidth, .height = initialHeight}};
  const frt::optional<Widget> optNullOpt;

  SECTION("generic area function for value")
  {
    const GenericAreaOrZero areaOrZero{defaultArea};
    REQUIRE(optVal.fold(areaOrZero) == initialWidth * initialHeight);
  }

  SECTION("generic area function for nullopt")
  {
    const GenericAreaOrZero areaOrZero{defaultArea};
    REQUIRE(optNullOpt.fold(areaOrZero) == defaultArea);
  }

  SECTION("generic height modifier for value")
  {
    constexpr auto scaleFactor = 2;
    frt::optional opt{Widget{.width = initialWidth, .height = initialHeight}};
    const GenericHeightModifier modifyHeight{scaleFactor};

    opt.fold(modifyHeight);

    REQUIRE(opt.value().height == initialHeight * scaleFactor);
  }
}
