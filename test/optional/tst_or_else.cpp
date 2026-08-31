#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::vector<int>>;

inline const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline constexpr int extraArg = 42;

struct NothrowOrElseFn
{
  Opt operator()() const noexcept { return Opt{expectedValue}; }
};

struct ThrowingOrElseFn
{
  Opt operator()() const noexcept(false) { return Opt{expectedValue}; }
};

} // namespace

TEST_CASE("or_else", "[optional]")
{
  auto orElsePopulated = []() -> Opt { return Opt{expectedValue}; };
  auto orElseEmpty = []() -> Opt { return Opt{}; };
  auto orElseExtra = [](int) -> Opt { return Opt{expectedValue}; };

  const Opt val{expectedValue};
  const Opt nullOpt;

  SECTION("nullopt without extra args returning populated optional (has_value)")
  {
    const auto res = nullOpt.or_else(orElsePopulated);
    REQUIRE(res.has_value());
  }

  SECTION("nullopt without extra args returning populated optional (value)")
  {
    const auto res = nullOpt.or_else(orElsePopulated);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("nullopt without extra args returning empty optional (has_value)")
  {
    const auto res = nullOpt.or_else(orElseEmpty);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("nullopt with extra args (has_value)")
  {
    const auto res = nullOpt.or_else(orElseExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("nullopt with extra args (value)")
  {
    const auto res = nullOpt.or_else(orElseExtra, extraArg);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value without extra args (has_value)")
  {
    const auto res = val.or_else(orElsePopulated);
    REQUIRE(res.has_value());
  }

  SECTION("value without extra args (value)")
  {
    const auto res = val.or_else(orElsePopulated);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    Opt opt{expectedValue};
    const auto res = opt.or_else(orElsePopulated);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with const lvalue forwarding")
  {
    const Opt opt{expectedValue};
    const auto res = opt.or_else(orElsePopulated);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with rvalue forwarding")
  {
    Opt opt{expectedValue};
    const auto res = std::move(opt).or_else(orElsePopulated);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with const rvalue forwarding")
  {
    const Opt opt{expectedValue};
    const auto res = static_cast<const Opt &&>(opt).or_else(orElsePopulated);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("noexcept when function is noexcept") { REQUIRE(noexcept(std::declval<Opt>().or_else(NothrowOrElseFn{}))); }

  SECTION("not noexcept when function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().or_else(ThrowingOrElseFn{})));
  }
}
