#include <catch2/catch_all.hpp>

#include <format>
#include <iterator>
#include <string>

#include <frt/format/std_fmt_either.hpp>

namespace
{

struct CustomFormattable
{
  int value{};
};

inline const std::string expectedLongString =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";
inline constexpr int expectedInt = 42;
inline constexpr double expectedDouble = 3.14;

inline const std::string expectedLeftVoid = "either{left: void}";
inline const std::string expectedRightVoid = "either{right: void}";
inline const std::string expectedLeftInt = "either{left: 42}";
inline const std::string expectedRightInt = "either{right: 42}";
inline const std::string expectedLeftLongString = "either{left: " + expectedLongString + "}";
inline const std::string expectedRightLongString = "either{right: " + expectedLongString + "}";

inline const std::string expectedNestedLeftLeft = "either{left: either{left: 42}}";
inline const std::string expectedNestedLeftRight = "either{left: either{right: 3.14}}";

inline const std::string expectedCustomLeft = "either{left: custom: 42}";
inline const std::string expectedCustomRight = "either{right: custom: 42}";

inline const std::string expectedLeftAligned = "either{left: 42}    ";
inline const std::string expectedRightAligned = "    either{left: 42}";
inline const std::string expectedCenterAligned = "  either{left: 42}  ";
inline const std::string expectedFilledCenter = "**either{left: 42}**";
inline const std::string expectedTruncated = "either{left";

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

TEST_CASE("format either<L, R>", "[either]")
{
  using Either = frt::either<int, std::string>;
  using EitherReversed = frt::either<std::string, int>;

  const Either left{frt::in_place_left, expectedInt};
  const Either right{frt::in_place_right, expectedLongString};
  const EitherReversed reversedLeft{frt::in_place_left, expectedLongString};
  const EitherReversed reversedRight{frt::in_place_right, expectedInt};

  SECTION("left alternative") { REQUIRE(std::format("{}", left) == expectedLeftInt); }

  SECTION("right alternative") { REQUIRE(std::format("{}", right) == expectedRightLongString); }

  SECTION("reversed left alternative") { REQUIRE(std::format("{}", reversedLeft) == expectedLeftLongString); }

  SECTION("reversed right alternative") { REQUIRE(std::format("{}", reversedRight) == expectedRightInt); }
}

TEST_CASE("format either<void, R>", "[either]")
{
  using EitherVoidLeft = frt::either<void, std::string>;

  const EitherVoidLeft left{frt::in_place_left};
  const EitherVoidLeft right{frt::in_place_right, expectedLongString};

  SECTION("left alternative") { REQUIRE(std::format("{}", left) == expectedLeftVoid); }

  SECTION("right alternative") { REQUIRE(std::format("{}", right) == expectedRightLongString); }
}

TEST_CASE("format either<L, void>", "[either]")
{
  using EitherVoidRight = frt::either<std::string, void>;

  const EitherVoidRight left{frt::in_place_left, expectedLongString};
  const EitherVoidRight right{frt::in_place_right};

  SECTION("left alternative") { REQUIRE(std::format("{}", left) == expectedLeftLongString); }

  SECTION("right alternative") { REQUIRE(std::format("{}", right) == expectedRightVoid); }
}

TEST_CASE("format either<void, void>", "[either]")
{
  using EitherVoidVoid = frt::either<void, void>;

  const EitherVoidVoid left{frt::in_place_left};
  const EitherVoidVoid right{frt::in_place_right};

  SECTION("left alternative") { REQUIRE(std::format("{}", left) == expectedLeftVoid); }

  SECTION("right alternative") { REQUIRE(std::format("{}", right) == expectedRightVoid); }
}

TEST_CASE("format nested either", "[either]")
{
  using NestedEither = frt::either<frt::either<int, double>, std::string>;

  const NestedEither leftLeft{frt::in_place_left, frt::in_place_left, expectedInt};
  const NestedEither leftRight{frt::in_place_left, frt::in_place_right, expectedDouble};
  const NestedEither right{frt::in_place_right, expectedLongString};

  SECTION("left-left alternative") { REQUIRE(std::format("{}", leftLeft) == expectedNestedLeftLeft); }

  SECTION("left-right alternative") { REQUIRE(std::format("{}", leftRight) == expectedNestedLeftRight); }

  SECTION("right alternative") { REQUIRE(std::format("{}", right) == expectedRightLongString); }
}

TEST_CASE("format custom formattable type", "[either]")
{
  const frt::either<CustomFormattable, std::string> left{frt::in_place_left, CustomFormattable{expectedInt}};
  const frt::either<std::string, CustomFormattable> right{frt::in_place_right, CustomFormattable{expectedInt}};

  SECTION("custom type as left alternative") { REQUIRE(std::format("{}", left) == expectedCustomLeft); }

  SECTION("custom type as right alternative") { REQUIRE(std::format("{}", right) == expectedCustomRight); }
}

TEST_CASE("format string specifiers", "[either]")
{
  using Either = frt::either<int, void>;

  const Either left{frt::in_place_left, expectedInt};

  SECTION("left alignment") { REQUIRE(std::format("{:<20}", left) == expectedLeftAligned); }

  SECTION("right alignment") { REQUIRE(std::format("{:>20}", left) == expectedRightAligned); }

  SECTION("center alignment") { REQUIRE(std::format("{:^20}", left) == expectedCenterAligned); }

  SECTION("fill and center alignment") { REQUIRE(std::format("{:*^20}", left) == expectedFilledCenter); }

  SECTION("precision truncation") { REQUIRE(std::format("{:.11}", left) == expectedTruncated); }
}

TEST_CASE("format_to", "[either]")
{
  using Either = frt::either<int, std::string>;

  const Either left{frt::in_place_left, expectedInt};

  SECTION("format_to with back_inserter")
  {
    std::string buffer;
    std::format_to(std::back_inserter(buffer), "{}", left);
    REQUIRE(buffer == expectedLeftInt);
  }
}

TEST_CASE("format error on invalid specifier", "[either]")
{
  using Either = frt::either<int, std::string>;

  const Either left{frt::in_place_left, expectedInt};

  SECTION("invalid format specifier throws format_error")
  {
    REQUIRE_THROWS_AS(std::vformat("{:d}", std::make_format_args(left)), std::format_error);
  }
}
