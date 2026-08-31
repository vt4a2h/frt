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

struct NothrowSimpleTransformFn
{
  constexpr std::vector<double> operator()(const std::vector<int> &) const noexcept
  {
    return makeExpectedTransformedValue();
  }
};

struct ThrowingSimpleTransformFn
{
  constexpr std::vector<double> operator()(const std::vector<int> &) const noexcept(false)
  {
    return makeExpectedTransformedValue();
  }
};

} // namespace

TEST_CASE("transform constexpr", "[optional][constexpr]")
{
  auto transform = [](const std::vector<int> &) -> std::vector<double> { return makeExpectedTransformedValue(); };
  auto transformExtra = [](const std::vector<int> &, int) -> std::vector<double> {
    return makeExpectedTransformedValue();
  };
  auto transformUnwrapped = [](int, const std::vector<int> &) -> std::vector<double> {
    return makeExpectedTransformedValue();
  };

  SECTION("value to different value type without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Opt val{makeExpectedValue()};
      const auto res = val.transform(transform);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type without extra args (value)")
  {
    constexpr auto valid = [transform] {
      const Opt val{makeExpectedValue()};
      const auto res = val.transform(transform);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Opt val{makeExpectedValue()};
      const auto res = val.transform(transformExtra, extraArg);
      return res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with extra args without unwrapped (value)")
  {
    constexpr auto valid = [transformExtra] {
      const Opt val{makeExpectedValue()};
      const auto res = val.transform(transformExtra, extraArg);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Opt val{makeExpectedValue()};
      const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value to different value type with unwrapped in extra args (value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Opt val{makeExpectedValue()};
      const auto res = val.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return res.value() == makeExpectedTransformedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with non-const lvalue forwarding")
  {
    constexpr auto valid = [] {
      Opt opt{makeExpectedValue()};
      auto fn = [](std::vector<int> &v) {
        v.clear();
        return true;
      };
      [[maybe_unused]] const auto res = opt.transform(fn);
      return opt.value().empty();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const lvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt{makeExpectedValue()};
      auto fn = [](const std::vector<int> &v) { return v == makeExpectedValue(); };
      const auto res = opt.transform(fn);
      return res.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with rvalue forwarding")
  {
    constexpr auto valid = [] {
      Opt opt{makeExpectedValue()};
      auto fn = [](std::vector<int> &&v) { return std::move(v); };
      const auto res = std::move(opt).transform(fn);
      return res.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("value with const rvalue forwarding")
  {
    constexpr auto valid = [] {
      const Opt opt{makeExpectedValue()};
      auto fn = [](const std::vector<int> &&v) { return v == makeExpectedValue(); };
      const auto res = static_cast<const Opt &&>(opt).transform(fn);
      return res.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt without extra args (has_value)")
  {
    constexpr auto valid = [transform] {
      const Opt nullOpt;
      const auto res = nullOpt.transform(transform);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with extra args without unwrapped (has_value)")
  {
    constexpr auto valid = [transformExtra] {
      const Opt nullOpt;
      const auto res = nullOpt.transform(transformExtra, extraArg);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("nullopt with unwrapped in extra args (has_value)")
  {
    constexpr auto valid = [transformUnwrapped] {
      const Opt nullOpt;
      const auto res = nullOpt.transform(transformUnwrapped, extraArg, frt::unwrapped);
      return !res.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("noexcept when function is noexcept")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().transform(NothrowSimpleTransformFn{}));
    STATIC_REQUIRE(valid);
  }

  SECTION("not noexcept when function throws")
  {
    constexpr auto valid = noexcept(std::declval<Opt>().transform(ThrowingSimpleTransformFn{}));
    STATIC_REQUIRE_FALSE(valid);
  }
}
