#include <catch2/catch_all.hpp>

#include <expected>
#include <string>
#include <utility>
#include <vector>

#include <frt/expected.hpp>

namespace
{

struct FourInt64
{
  std::int64_t first{};
  std::int64_t second{};
  std::int64_t third{};
  std::int64_t fourths{};
};

using FrtExpected = frt::expected<FourInt64, std::string>;

constexpr FrtExpected addOneFrtExpected(FourInt64 &&v) noexcept
{
  return FrtExpected{frt::in_place_expected, v.first + 1, v.second + 1, v.third + 1, v.fourths + 1};
}

constexpr FrtExpected addOneBenchFrtExpected() noexcept
{
  return FrtExpected{frt::in_place_expected}
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected)
      .and_then(&addOneFrtExpected);
}

using StdExpected = std::expected<FourInt64, std::string>;

constexpr StdExpected addOneStdExpected(FourInt64 &&v) noexcept
{
  return StdExpected{std::in_place, v.first + 1, v.second + 1, v.third + 1, v.fourths + 1};
}

constexpr StdExpected addOneBenchStdExpected() noexcept
{
  return StdExpected{std::in_place}
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected)
      .and_then(&addOneStdExpected);
}

} // namespace

TEST_CASE("Benchmark and_then", "[expected]")
{
  BENCHMARK("frt::expected::and_then simple chain") { return addOneBenchFrtExpected(); };
  BENCHMARK("std::expected::and_then simple chain") { return addOneBenchStdExpected(); };
}
