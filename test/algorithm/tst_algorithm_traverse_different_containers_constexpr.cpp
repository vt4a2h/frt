#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>
#include <frt/either.hpp>

#include <array>
#include <functional>
#include <list>
#include <utility>
#include <vector>

namespace
{
template <class E>
struct MyVec : std::vector<E>
{
  using std::vector<E>::vector;
};
} // namespace

template <class E>
struct frt::meta_container<MyVec<E>>
{
  static constexpr auto add_element = [](MyVec<E> &c, E &&e) { c.push_back(std::move(e)); };
};

TEST_CASE("frt::traverse_into constexpr -- std::vector", "[algorithm][traverse][constexpr]")
{
  using Result = frt::either<int, int>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = frt::traverse_into<std::vector>(input, std::identity{});
      using ExpectedResult = frt::either<int, std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = input | frt::traverse_into<std::vector>(std::identity{});
      using ExpectedResult = frt::either<int, std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, std::vector{42, 41, 40}};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure short-circuits")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_left, -1},
          Result{frt::in_place_right, 40},
      };

      const auto actual = frt::traverse_into<std::vector>(input, std::identity{});
      return actual.is_left() && actual.left() == -1;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse_into constexpr -- custom sequential container", "[algorithm][traverse][constexpr]")
{
  using Result = frt::either<int, int>;

  SECTION("success with push_back")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = frt::traverse_into<std::vector>(input, std::identity{});

      std::vector expectedVec{42, 41, 40};

      using ExpectedResult = frt::either<int, std::vector<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, expectedVec};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with custom meta_container")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = input | frt::traverse_into<MyVec>(std::identity{});

      MyVec<int> expectedVec{42, 41, 40};

      using ExpectedResult = frt::either<int, MyVec<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, expectedVec};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure short-circuits")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_left, -99},
          Result{frt::in_place_right, 40},
      };

      const auto actual = frt::traverse_into<std::vector>(input, std::identity{});
      return actual.is_left() && actual.left() == -99;
    }();

    STATIC_REQUIRE(valid);
  }
}
