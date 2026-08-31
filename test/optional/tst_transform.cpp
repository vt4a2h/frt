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

struct NothrowSimpleTransformFn
{
  std::vector<double> operator()(const std::vector<int> &) const noexcept { return expectedTransformedValue; }
};

struct ThrowingSimpleTransformFn
{
  std::vector<double> operator()(const std::vector<int> &) const noexcept(false) { return expectedTransformedValue; }
};

} // namespace

TEST_CASE("transform", "[optional]")
{
  auto transform = [](const std::vector<int> &) -> std::vector<double> { return expectedTransformedValue; };
  auto transformExtra = [](const std::vector<int> &, int) -> std::vector<double> { return expectedTransformedValue; };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::vector<double> {
    return expectedTransformedValue;
  };

  const Opt val{expectedValue};
  const Opt nullOpt;

  SECTION("value to different value type without extra args (has_value)")
  {
    const auto res = val.transform(transform);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type without extra args (value)")
  {
    const auto res = val.transform(transform);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    const auto res = val.transform(transformExtra, extraArg);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    const auto res = val.transform(transformExtra, extraArg);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.has_value());
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE(res.value() == expectedTransformedValue);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    Opt opt{expectedValue};
    auto fn = [](std::vector<int> &v) {
      v.clear();
      return true;
    };
    [[maybe_unused]] const auto res = opt.transform(fn);
    REQUIRE(opt.value().empty());
  }

  SECTION("value with const lvalue forwarding")
  {
    const Opt opt{expectedValue};
    auto fn = [](const std::vector<int> &v) { return v == expectedValue; };
    const auto res = opt.transform(fn);
    REQUIRE(res.value());
  }

  SECTION("value with rvalue forwarding")
  {
    Opt opt{expectedValue};
    auto fn = [](std::vector<int> &&v) { return std::move(v); };
    const auto res = std::move(opt).transform(fn);
    REQUIRE(res.value() == expectedValue);
  }

  SECTION("value with const rvalue forwarding")
  {
    const Opt opt{expectedValue};
    auto fn = [](const std::vector<int> &&v) { return v == expectedValue; };
    const auto res = static_cast<const Opt &&>(opt).transform(fn);
    REQUIRE(res.value());
  }

  SECTION("nullopt without extra args (has_value)")
  {
    const auto res = nullOpt.transform(transform);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("nullopt with extra args without unwrapped (has_value)")
  {
    const auto res = nullOpt.transform(transformExtra, extraArg);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("nullopt with unwrapped in extra args (has_value)")
  {
    const auto res = nullOpt.transform(transformUnwrapped, extraArg, frt::unwrapped);
    REQUIRE_FALSE(res.has_value());
  }

  SECTION("noexcept when function is noexcept")
  {
    REQUIRE(noexcept(std::declval<Opt>().transform(NothrowSimpleTransformFn{})));
  }

  SECTION("not noexcept when function throws")
  {
    REQUIRE_FALSE(noexcept(std::declval<Opt>().transform(ThrowingSimpleTransformFn{})));
  }
}
