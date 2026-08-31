#include <frt/optional.hpp>

#include <catch2/catch_all.hpp>

#include <numeric>
#include <ranges>

using Result = frt::optional<int>;

struct StructWithAField
{
  int x{};
};

TEST_CASE("Optional iterator")
{
  Result opt;

  SECTION("Begin == end for optional without value") { REQUIRE(opt.begin() == opt.end()); }

  SECTION("Begin != end for optional with value")
  {
    opt = 42;
    REQUIRE_FALSE(opt.begin() == opt.end());
  }

  SECTION("Can get a value")
  {
    constexpr int expectedValue = 42;

    opt = expectedValue;

    const int actualValue = *opt.begin();

    REQUIRE(actualValue == expectedValue);
  }

  SECTION("Can iterate over optional")
  {
    const auto &[newOpt, expectedSum] = GENERATE(table<Result, int>({
        {Result{42}, 42},
        {Result{}, 0},
    }));

    opt = newOpt;

    const int actualSum = std::accumulate(std::begin(opt), std::end(opt), 0);

    REQUIRE(actualSum == expectedSum);
  }

  SECTION("Can flat optional")
  {
    const std::array vec{Result{1}, Result{}, Result{}, Result{2}, Result{3}};

    const std::array expectedRange = {1, 2, 3};

    REQUIRE(std::ranges::equal(vec | std::ranges::views::join, expectedRange));
  }

  SECTION("Prefix increment")
  {
    opt = 42;
    auto it = opt.begin();
    REQUIRE(&(++it) == &it);
  }

  SECTION("Prefix increment value")
  {
    opt = 42;
    auto it = opt.begin();
    REQUIRE(++it == opt.end());
  }

  SECTION("Postfix increment")
  {
    opt = 42;
    auto it = opt.begin();
    REQUIRE(it++ == opt.begin());
  }

  SECTION("Postfix increment value")
  {
    opt = 42;
    auto it = opt.begin();
    it++;
    REQUIRE(it == opt.end());
  }

  SECTION("Prefix decrement")
  {
    opt = 42;
    auto it = opt.end();
    REQUIRE(&(--it) == &it);
  }

  SECTION("Prefix decrement value")
  {
    opt = 42;
    auto it = opt.end();
    REQUIRE(--it == opt.begin());
  }

  SECTION("Postfix decrement")
  {
    opt = 42;
    auto it = opt.end();
    REQUIRE(it-- == opt.end());
  }

  SECTION("Postfix decrement value")
  {
    opt = 42;
    auto it = opt.end();
    it--;
    REQUIRE(it == opt.begin());
  }

  SECTION("operator+=")
  {
    opt = 42;
    auto it = opt.begin();
    REQUIRE(&(it += 1) == &it);
  }

  SECTION("operator+= value")
  {
    opt = 42;
    auto it = opt.begin();
    it += 1;
    REQUIRE(it == opt.end());
  }

  SECTION("operator-=")
  {
    opt = 42;
    auto it = opt.end();
    REQUIRE(&(it -= 1) == &it);
  }

  SECTION("operator-= value")
  {
    opt = 42;
    auto it = opt.end();
    it -= 1;
    REQUIRE(it == opt.begin());
  }

  SECTION("operator+")
  {
    opt = 42;
    REQUIRE(opt.begin() + 1 == opt.end());
  }

  SECTION("operator+ (symmetric)")
  {
    opt = 42;
    REQUIRE(1 + opt.begin() == opt.end());
  }

  SECTION("operator- (n)")
  {
    opt = 42;
    REQUIRE(opt.end() - 1 == opt.begin());
  }

  SECTION("operator- (iterators)")
  {
    opt = 42;
    REQUIRE(opt.end() - opt.begin() == 1);
  }

  SECTION("operator[]")
  {
    opt = 42;
    REQUIRE(opt.begin()[0] == 42);
  }

  SECTION("operator->")
  {
    frt::optional optS = StructWithAField{.x = 42};
    REQUIRE(optS.begin()->x == 42);
  }

  SECTION("Comparison operator==")
  {
    opt = 42;
    REQUIRE(opt.begin() == opt.begin());
  }

  SECTION("Comparison operator!=")
  {
    opt = 42;
    REQUIRE(opt.begin() != opt.end());
  }

  SECTION("Comparison operator<")
  {
    opt = 42;
    REQUIRE(opt.begin() < opt.end());
  }

  SECTION("Comparison operator<=")
  {
    opt = 42;
    REQUIRE(opt.begin() <= opt.begin());
  }

  SECTION("Comparison operator>")
  {
    opt = 42;
    REQUIRE(opt.end() > opt.begin());
  }

  SECTION("Comparison operator>=")
  {
    opt = 42;
    REQUIRE(opt.end() >= opt.end());
  }
}
