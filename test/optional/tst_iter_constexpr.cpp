#include <frt/optional.hpp>
#include <ranges>

#include <catch2/catch_all.hpp>

#include <numeric>

using Result = frt::optional<int>;

struct StructWithAField
{
  int x{};
};

TEST_CASE("Optional iterator constexpr", "[optional][constexpr]")
{
  SECTION("Begin == end for optional without value")
  {
    constexpr auto result = [] {
      Result opt;
      return opt.begin() == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Begin != end for optional with value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin() == opt.end();
    }();
    STATIC_REQUIRE_FALSE(result);
  }

  SECTION("Can get a value")
  {
    constexpr int expectedValue = 42;

    constexpr auto actualValue = [expectedValue] {
      Result opt = expectedValue;
      return *opt.begin();
    }();

    STATIC_REQUIRE(actualValue == expectedValue);
  }

  SECTION("Can iterate over optional")
  {
    SECTION("Optional with value")
    {
      constexpr int expectedSum = 42;
      constexpr auto actualSum = [] {
        Result opt{42};
        return std::accumulate(std::begin(opt), std::end(opt), 0);
      }();

      STATIC_REQUIRE(actualSum == expectedSum);
    }

    SECTION("Optional without value")
    {
      constexpr int expectedSum = 0;
      constexpr auto actualSum = [] {
        Result opt{};
        return std::accumulate(std::begin(opt), std::end(opt), 0);
      }();

      STATIC_REQUIRE(actualSum == expectedSum);
    }
  }

  SECTION("Can flat optional")
  {
    constexpr auto result = [] {
      const std::array vec{Result{1}, Result{}, Result{}, Result{2}, Result{3}};
      const std::array expectedRange = {1, 2, 3};

      return std::ranges::equal(vec | std::ranges::views::join, expectedRange);
    }();

    STATIC_REQUIRE(result);
  }

  SECTION("Satisfies std::contiguous_iterator concept") { STATIC_REQUIRE(std::contiguous_iterator<Result::iterator>); }

  SECTION("Satisfies std::contiguous_iterator concept for const_iterator")
  {
    STATIC_REQUIRE(std::contiguous_iterator<Result::const_iterator>);
  }

  SECTION("Iterator trait: value_type")
  {
    STATIC_REQUIRE(std::is_same_v<std::iterator_traits<Result::iterator>::value_type, int>);
  }

  SECTION("Iterator trait: difference_type")
  {
    STATIC_REQUIRE(std::is_same_v<std::iterator_traits<Result::iterator>::difference_type, std::ptrdiff_t>);
  }

  SECTION("Iterator trait: iterator_concept")
  {
    STATIC_REQUIRE(std::is_same_v<Result::iterator::iterator_concept, std::contiguous_iterator_tag>);
  }

  SECTION("Prefix increment")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.begin();
      return &(++it) == &it;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Prefix increment value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.begin();
      return ++it == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Postfix increment")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.begin();
      return it++ == opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Postfix increment value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.begin();
      it++;
      return it == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Prefix decrement")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.end();
      return &(--it) == &it;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Prefix decrement value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.end();
      return --it == opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Postfix decrement")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.end();
      return it-- == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Postfix decrement value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.end();
      it--;
      return it == opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator+=")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.begin();
      return &(it += 1) == &it;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator+= value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.begin();
      it += 1;
      return it == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator-=")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.end();
      return &(it -= 1) == &it;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator-= value")
  {
    constexpr auto result = [] {
      Result opt = 42;
      auto it = opt.end();
      it -= 1;
      return it == opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator+")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin() + 1 == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator+ (symmetric)")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return 1 + opt.begin() == opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator- (n)")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.end() - 1 == opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator- (iterators)")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.end() - opt.begin() == 1;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator[]")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin()[0] == 42;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("operator->")
  {
    constexpr auto result = [] {
      frt::optional optS = StructWithAField{.x = 42};
      return optS.begin()->x == 42;
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Comparison operator==")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin() == opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Comparison operator!=")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin() != opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Comparison operator<")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin() < opt.end();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Comparison operator<=")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.begin() <= opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Comparison operator>")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.end() > opt.begin();
    }();
    STATIC_REQUIRE(result);
  }

  SECTION("Comparison operator>=")
  {
    constexpr auto result = [] {
      Result opt = 42;
      return opt.end() >= opt.end();
    }();
    STATIC_REQUIRE(result);
  }
}
