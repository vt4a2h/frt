#include <catch2/catch_all.hpp>

#include <compare>
#include <functional>
#include <string_view>

#include <frt/optional.hpp>

TEST_CASE("Compare", "[optional]")
{
  using Opt = frt::optional<int>;

  SECTION("Same optionals")
  {
    using Comp = std::function<bool(const frt::optional<int> &, const frt::optional<int> &)>;

    const auto [left, right, info, cmpOp, expectedResult] = GENERATE(table<Opt, Opt, std::string_view, Comp, bool>({
        {Opt{1}, Opt{1}, "equal", [](const auto &l, const auto &r) { return l == r; }, true},
        {Opt{frt::nullopt}, Opt{frt::nullopt}, "equal-nullopt", [](const auto &l, const auto &r) { return l == r; },
         true},
        {Opt{frt::nullopt}, Opt{1}, "equal-nullopt-val", [](const auto &l, const auto &r) { return l == r; }, false},
        {Opt{1}, Opt{frt::nullopt}, "equal-val-nullopt", [](const auto &l, const auto &r) { return l == r; }, false},
        {Opt{1}, Opt{3}, "not-equal", [](const auto &l, const auto &r) { return l != r; }, true},
        {Opt{1}, Opt{3}, "less", [](const auto &l, const auto &r) { return l < r; }, true},
        {Opt{3}, Opt{3}, "less-eq", [](const auto &l, const auto &r) { return l <= r; }, true},
        {Opt{3}, Opt{1}, "greater", [](const auto &l, const auto &r) { return l > r; }, true},
        {Opt{3}, Opt{3}, "greater-eq", [](const auto &l, const auto &r) { return l >= r; }, true},
    }));

    const auto actualResult = std::invoke(cmpOp, left, right);

    INFO(info);
    REQUIRE(actualResult == expectedResult);
  }

  SECTION("Optional and value")
  {
    using Comp = std::function<bool(const frt::optional<int> &, int)>;

    const auto [left, right, info, cmpOp, expectedResult] = GENERATE(table<Opt, int, std::string_view, Comp, bool>({
        {Opt{1}, 1, "equal", [](const auto &l, const auto &r) { return l == r; }, true},
        {Opt{frt::nullopt}, 1, "equal-nullopt", [](const auto &l, const auto &r) { return l == r; }, false},
        {Opt{1}, 3, "not-equal", [](const auto &l, const auto &r) { return l != r; }, true},
        {Opt{frt::nullopt}, 3, "not-equal-nullopt", [](const auto &l, const auto &r) { return l != r; }, true},
        {Opt{1}, 3, "less", [](const auto &l, const auto &r) { return l < r; }, true},
        {Opt{3}, 3, "less-eq", [](const auto &l, const auto &r) { return l <= r; }, true},
        {Opt{3}, 1, "greater", [](const auto &l, const auto &r) { return l > r; }, true},
        {Opt{3}, 3, "greater-eq", [](const auto &l, const auto &r) { return l >= r; }, true},
    }));

    const auto actualResult = std::invoke(cmpOp, left, right);

    INFO(info);
    REQUIRE(actualResult == expectedResult);
  }

  SECTION("Value and optional")
  {
    using Comp = std::function<bool(int, const frt::optional<int> &)>;

    const auto [left, right, info, cmpOp, expectedResult] = GENERATE(table<int, Opt, std::string_view, Comp, bool>({
        {1, Opt{1}, "equal", [](const auto &l, const auto &r) { return l == r; }, true},
        {1, Opt{frt::nullopt}, "equal-nullopt", [](const auto &l, const auto &r) { return l == r; }, false},
        {3, Opt{1}, "not-equal", [](const auto &l, const auto &r) { return l != r; }, true},
        {3, Opt{frt::nullopt}, "not-equal-nullopt", [](const auto &l, const auto &r) { return l != r; }, true},
        {1, Opt{3}, "less", [](const auto &l, const auto &r) { return l < r; }, true},
        {3, Opt{3}, "less-eq", [](const auto &l, const auto &r) { return l <= r; }, true},
        {3, Opt{1}, "greater", [](const auto &l, const auto &r) { return l > r; }, true},
        {3, Opt{3}, "greater-eq", [](const auto &l, const auto &r) { return l >= r; }, true},
    }));

    const auto actualResult = std::invoke(cmpOp, left, right);

    INFO(info);
    REQUIRE(actualResult == expectedResult);
  }

  SECTION("Optional and frt::nullopt")
  {
    using Comp = std::function<bool(const frt::optional<int> &, frt::nullopt_t)>;

    const auto [left, right, info, cmpOp,
                expectedResult] = GENERATE(table<Opt, frt::nullopt_t, std::string_view, Comp, bool>({
        {Opt{1}, frt::nullopt, "equal", [](const auto &l, const auto &r) { return l == r; }, false},
        {Opt{frt::nullopt}, frt::nullopt, "equal-nullopt", [](const auto &l, const auto &r) { return l == r; }, true},
        {Opt{1}, frt::nullopt, "not-equal", [](const auto &l, const auto &r) { return l != r; }, true},
        {Opt{frt::nullopt}, frt::nullopt, "not-equal-nullopt", [](const auto &l, const auto &r) { return l != r; },
         false},
        {Opt{1}, frt::nullopt, "less", [](const auto &l, const auto &r) { return l < r; }, false},
        {Opt{3}, frt::nullopt, "less-eq", [](const auto &l, const auto &r) { return l <= r; }, false},
        {Opt{3}, frt::nullopt, "greater", [](const auto &l, const auto &r) { return l > r; }, true},
        {Opt{3}, frt::nullopt, "greater-eq", [](const auto &l, const auto &r) { return l >= r; }, true},
    }));

    const auto actualResult = std::invoke(cmpOp, left, right);

    INFO(info);
    REQUIRE(actualResult == expectedResult);
  }

  SECTION("frt::nullopt and optional")
  {
    using Comp = std::function<bool(frt::nullopt_t, const frt::optional<int> &)>;

    const auto [left, right, info, cmpOp,
                expectedResult] = GENERATE(table<frt::nullopt_t, Opt, std::string_view, Comp, bool>({
        {frt::nullopt, Opt{1}, "equal", [](const auto &l, const auto &r) { return l == r; }, false},
        {frt::nullopt, Opt{frt::nullopt}, "equal-nullopt", [](const auto &l, const auto &r) { return l == r; }, true},
        {frt::nullopt, Opt{1}, "not-equal", [](const auto &l, const auto &r) { return l != r; }, true},
        {frt::nullopt, Opt{frt::nullopt}, "not-equal-nullopt", [](const auto &l, const auto &r) { return l != r; },
         false},
        {frt::nullopt, Opt{1}, "less", [](const auto &l, const auto &r) { return l < r; }, true},
        {frt::nullopt, Opt{3}, "less-eq", [](const auto &l, const auto &r) { return l <= r; }, true},
        {frt::nullopt, Opt{3}, "greater", [](const auto &l, const auto &r) { return l > r; }, false},
        {frt::nullopt, Opt{3}, "greater-eq", [](const auto &l, const auto &r) { return l >= r; }, false},
    }));

    const auto actualResult = std::invoke(cmpOp, left, right);

    INFO(info);
    REQUIRE(actualResult == expectedResult);
  }

  SECTION("Three-way comparison (same optionals)")
  {
    const auto [left, right, expectedResult] =
        GENERATE(table<Opt, Opt, std::partial_ordering>({{Opt{1}, Opt{1}, std::partial_ordering::equivalent},
                                                         {Opt{1}, Opt{3}, std::partial_ordering::less},
                                                         {Opt{3}, Opt{1}, std::partial_ordering::greater},
                                                         {Opt{}, Opt{}, std::partial_ordering::equivalent},
                                                         {Opt{}, Opt{1}, std::partial_ordering::unordered},
                                                         {Opt{1}, Opt{}, std::partial_ordering::unordered}}));

    REQUIRE((left <=> right) == expectedResult);
  }

  SECTION("Three-way comparison (optional and nullopt)")
  {
    const auto [left, right, expectedResult] = GENERATE(table<Opt, frt::nullopt_t, std::strong_ordering>(
        {{Opt{}, frt::nullopt, std::strong_ordering::equal}, {Opt{1}, frt::nullopt, std::strong_ordering::greater}}));

    REQUIRE((left <=> right) == expectedResult);
  }

  SECTION("Three-way comparison (nullopt and optional)")
  {
    const auto [left, right, expectedResult] = GENERATE(table<frt::nullopt_t, Opt, std::strong_ordering>(
        {{frt::nullopt, Opt{}, std::strong_ordering::equal}, {frt::nullopt, Opt{1}, std::strong_ordering::less}}));

    REQUIRE((left <=> right) == expectedResult);
  }

  SECTION("Three-way comparison (optional and value)")
  {
    const auto [left, right, expectedResult] = GENERATE(table<Opt, int, std::strong_ordering>({
        {Opt{}, 1, std::strong_ordering::less},
        {Opt{1}, 1, std::strong_ordering::equal},
        {Opt{1}, 3, std::strong_ordering::less},
        {Opt{3}, 1, std::strong_ordering::greater},
    }));

    REQUIRE((left <=> right) == expectedResult);
  }

  SECTION("Three-way comparison (value and optional)")
  {
    const auto [left, right, expectedResult] = GENERATE(table<int, Opt, std::strong_ordering>({
        {1, Opt{}, std::strong_ordering::greater},
        {1, Opt{1}, std::strong_ordering::equal},
        {1, Opt{3}, std::strong_ordering::less},
        {3, Opt{1}, std::strong_ordering::greater},
    }));

    REQUIRE((left <=> right) == expectedResult);
  }

  SECTION("Cross-type comparison")
  {
    const frt::optional<int> optInt{1};
    const frt::optional<double> optDouble{1.0};
    REQUIRE(optInt == optDouble);
  }
}
