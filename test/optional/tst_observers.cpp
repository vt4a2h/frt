#include <catch2/catch_all.hpp>

#include <utility>

#include <frt/optional.hpp>

namespace
{

struct Foo
{
  [[nodiscard]] constexpr int getValue() const { return value; }

  constexpr int increment(int inc)
  {
    value += inc;
    return value;
  }

  int value{};
};

inline constexpr int defaultValue{42};
inline constexpr int modifiedValue{84};

} // namespace

TEST_CASE("Observers", "[optional]")
{
  using Opt = frt::optional<int>;

  SECTION("Check has_value (nullopt)")
  {
    const Opt opt;
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("Check operator bool (nullopt)")
  {
    const Opt opt;
    REQUIRE_FALSE(static_cast<bool>(opt));
  }

  SECTION("Check has_value (value)")
  {
    const Opt opt{defaultValue};
    REQUIRE(opt.has_value());
  }

  SECTION("Check operator bool (value)")
  {
    const Opt opt{defaultValue};
    REQUIRE(static_cast<bool>(opt));
  }

  SECTION("Access value by const &")
  {
    const Opt optVal{defaultValue};
    REQUIRE(optVal.value() == defaultValue);
  }

  SECTION("Access value by &")
  {
    Opt optVal{defaultValue};
    REQUIRE(optVal.value() == defaultValue);
  }

  SECTION("Access value by const &&")
  {
    const Opt optVal{defaultValue};
    const auto &&actualValue = static_cast<const Opt &&>(optVal).value();
    REQUIRE(actualValue == defaultValue);
  }

  SECTION("Access value by &&")
  {
    Opt optVal{defaultValue};
    auto &&actualValue = std::move(optVal).value();
    REQUIRE(actualValue == defaultValue);
  }

  SECTION("Access value by const & (operator*)")
  {
    const Opt optVal{defaultValue};
    REQUIRE(*optVal == defaultValue);
  }

  SECTION("Access value by & (operator*)")
  {
    Opt optVal{defaultValue};
    REQUIRE(*optVal == defaultValue);
  }

  SECTION("Access value by const && (operator*)")
  {
    const Opt optVal{defaultValue};
    const auto &&actualValue = *static_cast<const Opt &&>(optVal);
    REQUIRE(actualValue == defaultValue);
  }

  SECTION("Access value by && (operator*)")
  {
    Opt optVal{defaultValue};
    auto &&actualValue = *std::move(optVal);
    REQUIRE(actualValue == defaultValue);
  }

  SECTION("Access value by ->")
  {
    frt::optional optFoo{Foo{.value = defaultValue}};
    const auto actual = optFoo->increment(defaultValue);
    REQUIRE(actual == modifiedValue);
  }

  SECTION("Access value by const ->")
  {
    const frt::optional optFoo{Foo{.value = defaultValue}};
    const auto actual = optFoo->getValue();
    REQUIRE(actual == defaultValue);
  }

  SECTION("Get an actual value if present (value_or)")
  {
    const Opt opt{modifiedValue};
    const auto actual = opt.value_or(defaultValue);
    REQUIRE(actual == modifiedValue);
  }

  SECTION("Get default if not present (value_or)")
  {
    const Opt opt;
    const auto actual = opt.value_or(defaultValue);
    REQUIRE(actual == defaultValue);
  }

  SECTION("Get an actual value if present (value_or from tmp)")
  {
    Opt opt{modifiedValue};
    const auto actual = std::move(opt).value_or(defaultValue);
    REQUIRE(actual == modifiedValue);
  }

  SECTION("Get default if not present (value_or from tmp)")
  {
    Opt opt;
    const auto actual = std::move(opt).value_or(defaultValue);
    REQUIRE(actual == defaultValue);
  }

  SECTION("Get an actual value if present (value_or convertible type)")
  {
    const Opt opt{0};
    constexpr bool defaultBool = true;
    const auto actual = opt.value_or(defaultBool);
    REQUIRE(actual == 0);
  }

  SECTION("Get default if not present (value_or convertible type)")
  {
    const Opt opt;
    constexpr bool defaultBool = true;
    const auto actual = opt.value_or(defaultBool);
    REQUIRE(actual == 1);
  }

  SECTION("asEither const & (value)")
  {
    const Opt opt{defaultValue};
    REQUIRE(opt.asEither().is_right());
  }

  SECTION("asEither & (value)")
  {
    Opt opt{defaultValue};
    REQUIRE(opt.asEither().is_right());
  }

  SECTION("asEither const && (value)")
  {
    const Opt opt{defaultValue};
    REQUIRE(static_cast<const Opt &&>(opt).asEither().is_right());
  }

  SECTION("asEither && (value)")
  {
    Opt opt{defaultValue};
    REQUIRE(std::move(opt).asEither().is_right());
  }

  SECTION("asEither const & (nullopt)")
  {
    const Opt opt;
    REQUIRE(opt.asEither().is_left());
  }

  SECTION("asEither & (nullopt)")
  {
    Opt opt;
    REQUIRE(opt.asEither().is_left());
  }

  SECTION("asEither const && (nullopt)")
  {
    const Opt opt;
    REQUIRE(static_cast<const Opt &&>(opt).asEither().is_left());
  }

  SECTION("asEither && (nullopt)")
  {
    Opt opt;
    REQUIRE(std::move(opt).asEither().is_left());
  }
}
