#include <catch2/catch_all.hpp>

#include <expected>
#include <string>
#include <utility>
#include <vector>

#include <frt/either.hpp>

namespace
{

struct FourInt64
{
  std::int64_t first{};
  std::int64_t second{};
  std::int64_t third{};
  std::int64_t fourths{};
};

using Either = frt::either<FourInt64, std::string>;

constexpr Either addOneEither(FourInt64 &&v) noexcept
{
  return {frt::in_place_left, v.first + 1, v.second + 1, v.third + 1, v.fourths + 1};
}

constexpr Either addOneBenchEither() noexcept
{
  return Either{frt::in_place_left}
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither)
      .and_then_left(&addOneEither);
}

using Expected = std::expected<FourInt64, std::string>;

constexpr Expected addOneExpected(FourInt64 &&v) noexcept
{
  return Expected{std::in_place, v.first + 1, v.second + 1, v.third + 1, v.fourths + 1};
}

constexpr Expected addOneBenchExpected() noexcept
{
  return Expected{std::in_place}
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected)
      .and_then(&addOneExpected);
}

} // namespace

TEST_CASE("and_then_left", "[either]")
{
  BENCHMARK("either::and_then_left simple chain") { return addOneBenchEither(); };
  BENCHMARK("expected::and_then simple chain") { return addOneBenchExpected(); };
}
