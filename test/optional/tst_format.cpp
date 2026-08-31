#include <catch2/catch_all.hpp>

#include <format>
#include <iterator>
#include <string>

#include <frt/format/optional-fmt.hpp>

namespace
{

struct CustomFormattable
{
  int value{};
};

inline const std::string expectedLongString =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
    "magna aliqua";
inline constexpr int expectedInt = 42;

inline const std::string expectedValueInt = "optional{42}";
inline const std::string expectedValueLongString = "optional{" + expectedLongString + "}";
inline const std::string expectedNullOpt = "optional{nullopt}";

inline const std::string expectedNestedValue = "optional{optional{42}}";
inline const std::string expectedNestedNullOpt = "optional{optional{nullopt}}";

inline const std::string expectedCustomValue = "optional{custom: 42}";

inline const std::string expectedLeftAligned = "optional{42}        ";
inline const std::string expectedRightAligned = "        optional{42}";
inline const std::string expectedCenterAligned = "    optional{42}    ";
inline const std::string expectedFilledCenter = "****optional{42}****";
inline const std::string expectedTruncated = "optional{42";

} // namespace

template <>
struct std::formatter<CustomFormattable, char>
{
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  template <class FormatContext>
  auto format(const CustomFormattable &c, FormatContext &ctx) const
  {
    return std::format_to(ctx.out(), "custom: {}", c.value);
  }
};

TEST_CASE("format optional<T>", "[optional]")
{
  using OptInt = frt::optional<int>;
  using OptString = frt::optional<std::string>;

  const OptInt valInt{expectedInt};
  const OptString valString{expectedLongString};
  const OptInt nullOptInt{};

  SECTION("value alternative (int)") { REQUIRE(std::format("{}", valInt) == expectedValueInt); }

  SECTION("value alternative (long string)") { REQUIRE(std::format("{}", valString) == expectedValueLongString); }

  SECTION("nullopt alternative") { REQUIRE(std::format("{}", nullOptInt) == expectedNullOpt); }
}

TEST_CASE("format nested optional", "[optional]")
{
  using NestedOpt = frt::optional<frt::optional<int>>;

  const NestedOpt valVal{frt::in_place_opt, expectedInt};
  const NestedOpt valNullOpt{frt::in_place_opt, frt::nullopt};
  const NestedOpt nullOpt{};

  SECTION("nested value-value alternative") { REQUIRE(std::format("{}", valVal) == expectedNestedValue); }

  SECTION("nested value-nullopt alternative") { REQUIRE(std::format("{}", valNullOpt) == expectedNestedNullOpt); }

  SECTION("outer nullopt alternative") { REQUIRE(std::format("{}", nullOpt) == expectedNullOpt); }
}

TEST_CASE("format custom formattable type", "[optional]")
{
  const frt::optional<CustomFormattable> val{CustomFormattable{expectedInt}};
  const frt::optional<CustomFormattable> nullOpt{};

  SECTION("custom type as value alternative") { REQUIRE(std::format("{}", val) == expectedCustomValue); }

  SECTION("custom type as nullopt alternative") { REQUIRE(std::format("{}", nullOpt) == expectedNullOpt); }
}

TEST_CASE("format string specifiers", "[optional]")
{
  using Opt = frt::optional<int>;

  const Opt val{expectedInt};

  SECTION("left alignment") { REQUIRE(std::format("{:<20}", val) == expectedLeftAligned); }

  SECTION("right alignment") { REQUIRE(std::format("{:>20}", val) == expectedRightAligned); }

  SECTION("center alignment") { REQUIRE(std::format("{:^20}", val) == expectedCenterAligned); }

  SECTION("fill and center alignment") { REQUIRE(std::format("{:*^20}", val) == expectedFilledCenter); }

  SECTION("precision truncation") { REQUIRE(std::format("{:.11}", val) == expectedTruncated); }
}

TEST_CASE("format_to", "[optional]")
{
  using Opt = frt::optional<int>;

  const Opt val{expectedInt};

  SECTION("format_to with back_inserter")
  {
    std::string buffer;
    std::format_to(std::back_inserter(buffer), "{}", val);
    REQUIRE(buffer == expectedValueInt);
  }
}

TEST_CASE("format error on invalid specifier", "[optional]")
{
  using Opt = frt::optional<int>;

  const Opt val{expectedInt};

  SECTION("invalid format specifier throws format_error")
  {
    REQUIRE_THROWS_AS(std::vformat("{:d}", std::make_format_args(val)), std::format_error);
  }
}
