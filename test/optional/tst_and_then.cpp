#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::vector<int>>;

inline const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline const std::vector expectedTransformedValue{1.0, 2.0, 3.0, 4.0, 5.0};
inline constexpr int extraArg = 42;

struct NothrowTransformFn
{
  frt::optional<std::vector<double>> operator()(const std::vector<int> &) const noexcept
  {
    return expectedTransformedValue;
  }
};

struct ThrowingTransformFn
{
  frt::optional<std::vector<double>> operator()(const std::vector<int> &) const noexcept(false)
  {
    return expectedTransformedValue;
  }
};

} // namespace

TEST_CASE("and_then", "[optional]")
{
  auto transform = [](const std::vector<int> &) -> frt::optional<std::vector<double>> {
    return expectedTransformedValue;
  };
  auto transformToEmpty = [](const std::vector<int> &) -> frt::optional<std::vector<double>> { return {}; };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::optional<std::vector<double>> {
    return expectedTransformedValue;
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::optional<std::vector<double>> {
    return expectedTransformedValue;
  };

  const Opt val{expectedValue};
  const Opt nullOpt;

  SECTION("value to different value type without extra args (has_value)")
  {
    const auto res = val.and_then(transform);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type without extra args (value)")
  {
    const auto res = val.and_then(transform);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to empty optional without extra args (has_value)")
  {
    const auto res = val.and_then(transformToEmpty);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    const auto res = val.and_then(transformExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    const auto res = val.and_then(transformExtra, extraArg);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    Opt opt{expectedValue};
    auto fn = [](std::vector<int> &v) -> frt::optional<bool> {
      v.clear();
      return true;
    };
    [[maybe_unused]] const auto res = opt.and_then(fn);
    REQUIRE(opt.value().empty());
  }

  SECTION("value with const lvalue forwarding")
  {
    const Opt opt{expectedValue};
    auto fn = [](const std::vector<int> &v) -> frt::optional<bool> { return v == expectedValue; };
    const auto res = opt.and_then(fn);
    REQUIRE(res.value());
  }

  SECTION("value with rvalue forwarding")
  {
    Opt opt{expectedValue};
    auto fn = [](std::vector<int> &&v) -> frt::optional<std::vector<int>> { return std::move(v); };
    const auto res = std::move(opt).and_then(fn);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with const rvalue forwarding")
  {
    const Opt opt{expectedValue};
    auto fn = [](const std::vector<int> &&v) -> frt::optional<bool> { return v == expectedValue; };
    const auto res = static_cast<const Opt &&>(opt).and_then(fn);
    REQUIRE(res.value());
  }

  SECTION("nullopt without extra args (has_value)")
  {
    const auto res = nullOpt.and_then(transform);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("nullopt with extra args without unwrapped (has_value)")
  {
    const auto res = nullOpt.and_then(transformExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("nullopt with unwrapped in extra args (has_value)")
  {
    const auto res = nullOpt.and_then(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("noexcept when function is noexcept")
  {
    REQUIRE(noexcept(std::declval<Opt>().and_then(NothrowTransformFn{})));
  }

  SECTION("not noexcept when function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().and_then(ThrowingTransformFn{})));
  }
}
