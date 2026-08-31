#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::vector<int>>;

constexpr auto makeExpectedValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr int extraArg = 42;

struct NothrowOrElseFn
{
  constexpr Opt operator()() const noexcept { return Opt{makeExpectedValue()}; }
};

struct ThrowingOrElseFn
{
  constexpr Opt operator()() const noexcept(false) { return Opt{makeExpectedValue()}; }
};

} // namespace

TEST_CASE("or_else constexpr", "[optional][constexpr]")
{
  auto orElsePopulated = []() -> Opt { return Opt{makeExpectedValue()}; };
  auto orElseEmpty = []() -> Opt { return Opt{}; };
  auto orElseExtra = [](int) -> Opt { return Opt{makeExpectedValue()}; };

  SECTION("nullopt without extra args returning populated optional (has_value)")
  {
    constexpr auto valid = [orElsePopulated] {
      const Opt nullOpt;
      const auto res = nullOpt.or_else(orElsePopulated);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt without extra args returning populated optional (value)")
  {
    constexpr auto valid = [orElsePopulated] {
      const Opt nullOpt;
      const auto res = nullOpt.or_else(orElsePopulated);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt without extra args returning empty optional (has_value)")
  {
    constexpr auto valid = [orElseEmpty] {
      const Opt nullOpt;
      const auto res = nullOpt.or_else(orElseEmpty);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with extra args (has_value)")
  {
    constexpr auto valid = [orElseExtra] {
      const Opt nullOpt;
      const auto res = nullOpt.or_else(orElseExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with extra args (value)")
  {
    constexpr auto valid = [orElseExtra] {
      const Opt nullOpt;
      const auto res = nullOpt.or_else(orElseExtra, extraArg);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value without extra args (has_value)")
  {
    constexpr auto valid = [orElsePopulated] {
      const Opt val{makeExpectedValue()};
      const auto res = val.or_else(orElsePopulated);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value without extra args (value)")
  {
    constexpr auto valid = [orElsePopulated] {
      const Opt val{makeExpectedValue()};
      const auto res = val.or_else(orElsePopulated);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    constexpr auto valid = [orElsePopulated] {
      Opt opt{makeExpectedValue()};
      const auto res = opt.or_else(orElsePopulated);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [orElsePopulated] {
      const Opt opt{makeExpectedValue()};
      const auto res = opt.or_else(orElsePopulated);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding")
  {
    constexpr auto valid = [orElsePopulated] {
      Opt opt{makeExpectedValue()};
      const auto res = std::move(opt).or_else(orElsePopulated);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const rvalue forwarding")
  {
    constexpr auto valid = [orElsePopulated] {
      const Opt opt{makeExpectedValue()};
      const auto res = static_cast<const Opt &&>(opt).or_else(orElsePopulated);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when function is noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().or_else(NothrowOrElseFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when function throws")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().or_else(ThrowingOrElseFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}
