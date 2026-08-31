#include <catch2/catch_all.hpp>

#include <string>
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

TEST_CASE("Observers constexpr", "[optional][constexpr]")
{
  using Opt = frt::optional<int>;

  SECTION("Check has_value (nullopt)")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return !opt.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Check operator bool (nullopt)")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return !static_cast<bool>(opt);
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Check has_value (value)")
  {
    constexpr auto valid = [] {
      const Opt opt{defaultValue};
      return opt.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Check operator bool (value)")
  {
    constexpr auto valid = [] {
      const Opt opt{defaultValue};
      return static_cast<bool>(opt);
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by const &")
  {
    constexpr auto valid = [] {
      const Opt optVal{defaultValue};
      return optVal.value() == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by &")
  {
    constexpr auto valid = [] {
      Opt optVal{defaultValue};
      return optVal.value() == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by const &&")
  {
    constexpr auto valid = [] {
      const Opt optVal{defaultValue};
      return static_cast<const Opt &&>(optVal).value() == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by &&")
  {
    constexpr auto valid = [] {
      Opt optVal{defaultValue};
      return std::move(optVal).value() == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by const & (operator*)")
  {
    constexpr auto valid = [] {
      const Opt optVal{defaultValue};
      return *optVal == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by & (operator*)")
  {
    constexpr auto valid = [] {
      Opt optVal{defaultValue};
      return *optVal == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by const && (operator*)")
  {
    constexpr auto valid = [] {
      const Opt optVal{defaultValue};
      return *static_cast<const Opt &&>(optVal) == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by && (operator*)")
  {
    constexpr auto valid = [] {
      Opt optVal{defaultValue};
      return *std::move(optVal) == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by ->")
  {
    constexpr auto valid = [] {
      frt::optional optFoo{Foo{.value = defaultValue}};
      const auto actual = optFoo->increment(defaultValue);
      return actual == modifiedValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Access value by const ->")
  {
    constexpr auto valid = [] {
      const frt::optional optFoo{Foo{.value = defaultValue}};
      const auto actual = optFoo->getValue();
      return actual == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Get an actual value if present (value_or)")
  {
    constexpr auto valid = [] {
      const Opt opt{modifiedValue};
      return opt.value_or(defaultValue) == modifiedValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Get default if not present (value_or)")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return opt.value_or(defaultValue) == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Get an actual value if present (value_or from tmp)")
  {
    constexpr auto valid = [] {
      Opt opt{modifiedValue};
      return std::move(opt).value_or(defaultValue) == modifiedValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Get default if not present (value_or from tmp)")
  {
    constexpr auto valid = [] {
      Opt opt;
      return std::move(opt).value_or(defaultValue) == defaultValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Get an actual value if present (value_or convertible type)")
  {
    constexpr auto valid = [] {
      const Opt opt{0};
      constexpr bool defaultBool = true;
      return opt.value_or(defaultBool) == 0;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Get default if not present (value_or convertible type)")
  {
    constexpr auto valid = [] {
      const Opt opt;
      constexpr bool defaultBool = true;
      return opt.value_or(defaultBool) == 1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither const & (value)")
  {
    constexpr auto valid = [] {
      const Opt opt{defaultValue};
      return opt.asEither().is_right();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither & (value)")
  {
    constexpr auto valid = [] {
      Opt opt{defaultValue};
      return opt.asEither().is_right();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither const && (value)")
  {
    constexpr auto valid = [] {
      const Opt opt{defaultValue};
      return static_cast<const Opt &&>(opt).asEither().is_right();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither && (value)")
  {
    constexpr auto valid = [] {
      Opt opt{defaultValue};
      return std::move(opt).asEither().is_right();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither const & (nullopt)")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return opt.asEither().is_left();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither & (nullopt)")
  {
    constexpr auto valid = [] {
      Opt opt;
      return opt.asEither().is_left();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither const && (nullopt)")
  {
    constexpr auto valid = [] {
      const Opt opt;
      return static_cast<const Opt &&>(opt).asEither().is_left();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("asEither && (nullopt)")
  {
    constexpr auto valid = [] {
      Opt opt;
      return std::move(opt).asEither().is_left();
    }();
    STATIC_REQUIRE(valid);
  }
}
