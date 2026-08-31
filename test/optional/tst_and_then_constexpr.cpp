#include <catch2/catch_all.hpp>

#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::vector<int>>;

constexpr auto makeExpectedValue() { return std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedTransformedValue() { return std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0}; }
constexpr int extraArg = 42;

struct NothrowTransformFn
{
  constexpr frt::optional<std::vector<double>> operator()(const std::vector<int> &) const noexcept
  {
    return makeExpectedTransformedValue();
  }
};

struct ThrowingTransformFn
{
  constexpr frt::optional<std::vector<double>> operator()(const std::vector<int> &) const noexcept(false)
  {
    return makeExpectedTransformedValue();
  }
};

} // namespace

TEST_CASE("and_then constexpr", "[optional][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> frt::optional<std::vector<double>> {
    return makeExpectedTransformedValue();
  };
  auto transformToEmpty = [](const std::vector<int> &) -> frt::optional<std::vector<double>> { return {}; };
  auto transformExtra = [](const std::vector<int> &, int) -> frt::optional<std::vector<double>> {
    return makeExpectedTransformedValue();
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> frt::optional<std::vector<double>> {
    return makeExpectedTransformedValue();
  };

  SECTION("value to different value type without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transform);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transform);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to empty optional without extra args (has_value)")
  {
    constexpr auto valid = [transformToEmpty] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transformToEmpty);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transformExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transformExtra, extraArg);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Opt val{makeExpectedValue()};
      const auto res = val.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Opt opt{makeExpectedValue()};
      auto fn = [](std::vector<int> &v) -> frt::optional<bool> {
        v.clear();
        return true;
      };
      [[maybe_unused]] const auto res = opt.and_then(fn);
      return opt.value().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt{makeExpectedValue()};
      auto fn = [](const std::vector<int> &v) -> frt::optional<bool> { return v == makeExpectedValue(); };
      const auto res = opt.and_then(fn);
      return res.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Opt opt{makeExpectedValue()};
      auto fn = [](std::vector<int> &&v) -> frt::optional<std::vector<int>> { return std::move(v); };
      const auto res = std::move(opt).and_then(fn);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt{makeExpectedValue()};
      auto fn = [](const std::vector<int> &&v) -> frt::optional<bool> { return v == makeExpectedValue(); };
      const auto res = static_cast<const Opt &&>(opt).and_then(fn);
      return res.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Opt nullOpt;
      const auto res = nullOpt.and_then(transform);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Opt nullOpt;
      const auto res = nullOpt.and_then(transformExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Opt nullOpt;
      const auto res = nullOpt.and_then(transformUnwrapped, extraArg, frt::unwrapped);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when function is noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().and_then(NothrowTransformFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when function throws")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().and_then(ThrowingTransformFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}
