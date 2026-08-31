#include <catch2/catch_all.hpp>

#include <compare>

#include <frt/optional.hpp>

TEST_CASE("Compare constexpr", "[optional][constexpr]")
{
  using Opt = frt::optional<int>;

  SECTION("Same optionals")
  {
    constexpr auto valid = [] {
      if ((Opt{1} == Opt{1}) != true)
        return false;
      if ((Opt{frt::nullopt} == Opt{frt::nullopt}) != true)
        return false;
      if ((Opt{frt::nullopt} == Opt{1}) != false)
        return false;
      if ((Opt{1} == Opt{frt::nullopt}) != false)
        return false;
      if ((Opt{1} != Opt{3}) != true)
        return false;
      if ((Opt{1} < Opt{3}) != true)
        return false;
      if ((Opt{3} <= Opt{3}) != true)
        return false;
      if ((Opt{3} > Opt{1}) != true)
        return false;
      if ((Opt{3} >= Opt{3}) != true)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Optional and value")
  {
    constexpr auto valid = [] {
      if ((Opt{1} == 1) != true)
        return false;
      if ((Opt{frt::nullopt} == 1) != false)
        return false;
      if ((Opt{1} != 3) != true)
        return false;
      if ((Opt{frt::nullopt} != 3) != true)
        return false;
      if ((Opt{1} < 3) != true)
        return false;
      if ((Opt{3} <= 3) != true)
        return false;
      if ((Opt{3} > 1) != true)
        return false;
      if ((Opt{3} >= 3) != true)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Value and optional")
  {
    constexpr auto valid = [] {
      if ((1 == Opt{1}) != true)
        return false;
      if ((1 == Opt{frt::nullopt}) != false)
        return false;
      if ((3 != Opt{1}) != true)
        return false;
      if ((3 != Opt{frt::nullopt}) != true)
        return false;
      if ((1 < Opt{3}) != true)
        return false;
      if ((3 <= Opt{3}) != true)
        return false;
      if ((3 > Opt{1}) != true)
        return false;
      if ((3 >= Opt{3}) != true)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Optional and frt::nullopt")
  {
    constexpr auto valid = [] {
      if ((Opt{1} == frt::nullopt) != false)
        return false;
      if ((Opt{frt::nullopt} == frt::nullopt) != true)
        return false;
      if ((Opt{1} != frt::nullopt) != true)
        return false;
      if ((Opt{frt::nullopt} != frt::nullopt) != false)
        return false;
      if ((Opt{1} < frt::nullopt) != false)
        return false;
      if ((Opt{3} <= frt::nullopt) != false)
        return false;
      if ((Opt{3} > frt::nullopt) != true)
        return false;
      if ((Opt{3} >= frt::nullopt) != true)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("frt::nullopt and optional")
  {
    constexpr auto valid = [] {
      if ((frt::nullopt == Opt{1}) != false)
        return false;
      if ((frt::nullopt == Opt{frt::nullopt}) != true)
        return false;
      if ((frt::nullopt != Opt{1}) != true)
        return false;
      if ((frt::nullopt != Opt{frt::nullopt}) != false)
        return false;
      if ((frt::nullopt < Opt{1}) != true)
        return false;
      if ((frt::nullopt <= Opt{3}) != true)
        return false;
      if ((frt::nullopt > Opt{3}) != false)
        return false;
      if ((frt::nullopt >= Opt{3}) != false)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Three-way comparison (same optionals)")
  {
    constexpr auto valid = [] {
      if ((Opt{1} <=> Opt{1}) != std::partial_ordering::equivalent)
        return false;
      if ((Opt{1} <=> Opt{3}) != std::partial_ordering::less)
        return false;
      if ((Opt{3} <=> Opt{1}) != std::partial_ordering::greater)
        return false;
      if ((Opt{} <=> Opt{}) != std::partial_ordering::equivalent)
        return false;
      if ((Opt{} <=> Opt{1}) != std::partial_ordering::unordered)
        return false;
      if ((Opt{1} <=> Opt{}) != std::partial_ordering::unordered)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Three-way comparison (optional and nullopt)")
  {
    constexpr auto valid = [] {
      if ((Opt{} <=> frt::nullopt) != std::strong_ordering::equal)
        return false;
      if ((Opt{1} <=> frt::nullopt) != std::strong_ordering::greater)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Three-way comparison (nullopt and optional)")
  {
    constexpr auto valid = [] {
      if ((frt::nullopt <=> Opt{}) != std::strong_ordering::equal)
        return false;
      if ((frt::nullopt <=> Opt{1}) != std::strong_ordering::less)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Three-way comparison (optional and value)")
  {
    constexpr auto valid = [] {
      if ((Opt{} <=> 1) != std::strong_ordering::less)
        return false;
      if ((Opt{1} <=> 1) != std::strong_ordering::equal)
        return false;
      if ((Opt{1} <=> 3) != std::strong_ordering::less)
        return false;
      if ((Opt{3} <=> 1) != std::strong_ordering::greater)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Three-way comparison (value and optional)")
  {
    constexpr auto valid = [] {
      if ((1 <=> Opt{}) != std::strong_ordering::greater)
        return false;
      if ((1 <=> Opt{1}) != std::strong_ordering::equal)
        return false;
      if ((1 <=> Opt{3}) != std::strong_ordering::less)
        return false;
      if ((3 <=> Opt{1}) != std::strong_ordering::greater)
        return false;
      return true;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Cross-type comparison")
  {
    constexpr auto valid = [] {
      const frt::optional<int> optInt{1};
      const frt::optional<double> optDouble{1.0};
      return optInt == optDouble;
    }();

    STATIC_REQUIRE(valid);
  }
}
