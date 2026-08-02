#include <catch2/catch_all.hpp>
#include <expected>

#include <frt/either.hpp>

#include <expected>
#include <optional>

#include <print>

namespace
{

template <std::size_t Mult>
struct Int64x
{
  std::array<std::int64_t, Mult> data{};
};

using Int128 = Int64x<2>;
using Int256 = Int64x<1 << 2>;
using Int512 = Int64x<1 << 3>;
using Int1024 = Int64x<1 << 4>;

} // namespace

TEMPLATE_TEST_CASE("Sizes of left and right void either are equal", "[either]", std::int8_t, std::int16_t, std::int32_t,
                   std::int64_t, Int128, Int256, Int512, Int1024)
{
  const frt::either<TestType, void> either_left{frt::in_place_left};
  const frt::either<TestType, void> either_right{frt::in_place_right};

  INFO("Size of frt::either<TestType, void> is " << sizeof(either_left) << " bytes");
  INFO("Size of frt::either<TestType> is " << sizeof(either_right) << " bytes");

  REQUIRE(sizeof(either_left) == sizeof(either_right));
}

TEMPLATE_TEST_CASE("Size not more than std::optional", "[either]", std::int8_t, std::int16_t, std::int32_t,
                   std::int64_t, Int128, Int256, Int512, Int1024)
{
  const std::optional<TestType> opt{std::in_place};
  const frt::either<TestType, void> either_left{frt::in_place_left};

  INFO("Size of std::optional is " << sizeof(opt) << " bytes");
  INFO("Size of frt::either<TestType, void> is " << sizeof(either_left) << " bytes");

  REQUIRE(sizeof(either_left) <= sizeof(opt));
}

TEMPLATE_TEST_CASE("Size not more than std::expected<void, T>", "[either]", std::int8_t, std::int16_t, std::int32_t,
                   std::int64_t, Int128, Int256, Int512, Int1024)
{
  const std::expected<void, TestType> exp{std::in_place};
  const frt::either<void, TestType> either_right{frt::in_place_right};

  INFO("Size of std::optional is " << sizeof(exp) << " bytes");
  INFO("Size of frt::either<void, TestType> is " << sizeof(either_right) << " bytes");

  REQUIRE(sizeof(either_right) <= sizeof(exp));
}

TEMPLATE_TEST_CASE("Size not more than std::expected<T, E>", "[either]", std::int8_t, std::int16_t, std::int32_t,
                   std::int64_t, Int128, Int256, Int512, Int1024)
{
  const std::expected<TestType, TestType> exp{std::in_place};
  const frt::either<TestType, TestType> either{frt::in_place_right};

  INFO("Size of std::optional is " << sizeof(exp) << " bytes");
  INFO("Size of frt::either<void, TestType> is " << sizeof(either) << " bytes");

  REQUIRE(sizeof(either) <= sizeof(exp));
}
