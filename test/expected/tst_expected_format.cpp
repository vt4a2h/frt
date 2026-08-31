#include <catch2/catch_all.hpp>

#include <format>
#include <iterator>
#include <string>

#include <frt/format/expected-fmt.hpp>

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
inline constexpr double expectedDouble = 3.14;

inline const std::string expectedValueVoid = "expected{value: void}";
inline const std::string expectedValueInt = "expected{value: 42}";
inline const std::string expectedErrorInt = "expected{error: 42}";
inline const std::string expectedValueLongString = "expected{value: " + expectedLongString + "}";
inline const std::string expectedErrorLongString = "expected{error: " + expectedLongString + "}";

inline const std::string expectedNestedValue = "expected{value: expected{value: 42}}";
inline const std::string expectedNestedError = "expected{value: expected{error: 3.14}}";

inline const std::string expectedCustomValue = "expected{value: custom: 42}";
inline const std::string expectedCustomError = "expected{error: custom: 42}";

inline const std::string expectedLeftAligned = "expected{value: 42}  ";
inline const std::string expectedRightAligned = "  expected{value: 42}";
inline const std::string expectedCenterAligned = " expected{value: 42} ";
inline const std::string expectedFilledCenter = "*expected{value: 42}*";
inline const std::string expectedTruncated = "expected{va";

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

TEST_CASE("format expected<T, E>", "[expected]")
{
  using Expected = frt::expected<int, std::string>;
  using ExpectedReversed = frt::expected<std::string, int>;

  const Expected val{expectedInt};
  const Expected err{frt::unexpected{expectedLongString}};
  const ExpectedReversed reversedVal{expectedLongString};
  const ExpectedReversed reversedErr{frt::unexpected{expectedInt}};

  SECTION("value alternative") { REQUIRE(std::format("{}", val) == expectedValueInt); }

  SECTION("error alternative") { REQUIRE(std::format("{}", err) == expectedErrorLongString); }

  SECTION("reversed value alternative") { REQUIRE(std::format("{}", reversedVal) == expectedValueLongString); }

  SECTION("reversed error alternative") { REQUIRE(std::format("{}", reversedErr) == expectedErrorInt); }
}

TEST_CASE("format expected<void, E>", "[expected]")
{
  using ExpectedVoid = frt::expected<void, std::string>;

  const ExpectedVoid val{};
  const ExpectedVoid err{frt::unexpected{expectedLongString}};

  SECTION("void value alternative") { REQUIRE(std::format("{}", val) == expectedValueVoid); }

  SECTION("error alternative") { REQUIRE(std::format("{}", err) == expectedErrorLongString); }
}

TEST_CASE("format nested expected", "[expected]")
{
  using NestedExpected = frt::expected<frt::expected<int, double>, std::string>;

  const NestedExpected valVal{frt::expected<int, double>{expectedInt}};
  const NestedExpected valErr{frt::expected<int, double>{frt::unexpected{expectedDouble}}};
  const NestedExpected err{frt::unexpected{expectedLongString}};

  SECTION("nested value-value alternative") { REQUIRE(std::format("{}", valVal) == expectedNestedValue); }

  SECTION("nested value-error alternative") { REQUIRE(std::format("{}", valErr) == expectedNestedError); }

  SECTION("error alternative") { REQUIRE(std::format("{}", err) == expectedErrorLongString); }
}

TEST_CASE("format custom formattable type", "[expected]")
{
  const frt::expected<CustomFormattable, std::string> val{CustomFormattable{expectedInt}};
  const frt::expected<std::string, CustomFormattable> err{frt::unexpected{CustomFormattable{expectedInt}}};

  SECTION("custom type as value alternative") { REQUIRE(std::format("{}", val) == expectedCustomValue); }

  SECTION("custom type as error alternative") { REQUIRE(std::format("{}", err) == expectedCustomError); }
}

TEST_CASE("format string specifiers", "[expected]")
{
  using Expected = frt::expected<int, std::string>;

  const Expected val{expectedInt};

  SECTION("left alignment") { REQUIRE(std::format("{:<21}", val) == expectedLeftAligned); }

  SECTION("right alignment") { REQUIRE(std::format("{:>21}", val) == expectedRightAligned); }

  SECTION("center alignment") { REQUIRE(std::format("{:^21}", val) == expectedCenterAligned); }

  SECTION("fill and center alignment") { REQUIRE(std::format("{:*^21}", val) == expectedFilledCenter); }

  SECTION("precision truncation") { REQUIRE(std::format("{:.11}", val) == expectedTruncated); }
}

TEST_CASE("format_to", "[expected]")
{
  using Expected = frt::expected<int, std::string>;

  const Expected val{expectedInt};

  SECTION("format_to with back_inserter")
  {
    std::string buffer;
    std::format_to(std::back_inserter(buffer), "{}", val);
    REQUIRE(buffer == expectedValueInt);
  }
}

TEST_CASE("format error on invalid specifier", "[expected]")
{
  using Expected = frt::expected<int, std::string>;

  const Expected val{expectedInt};

  SECTION("invalid format specifier throws format_error")
  {
    REQUIRE_THROWS_AS(std::vformat("{:d}", std::make_format_args(val)), std::format_error);
  }
}
