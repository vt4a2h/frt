#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>

#include <forward_list>
#include <list>

#include <map>
#include <unordered_map>

#include <set>

namespace frt
{

template <class E>
struct meta_container<std::forward_list<E>>
{
  static constexpr auto add_element = []<class ElemT>(std::forward_list<E> &c, ElemT &&e) {
    c.push_front(std::forward<ElemT>(e));
  };
};

} // namespace frt

TEST_CASE("frt::traverse -- either - std::list", "[algorithm]")
{
  using Result = frt::either<std::string, int>;

  std::vector input{
      Result{frt::in_place_right, 42},
      Result{frt::in_place_right, 41},
      Result{frt::in_place_right, 40},
  };

  const auto actualResult = input | frt::traverse_into<std::list>(std::identity{});

  using ExpectedResultType = frt::either<std::string, std::list<int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_right, std::list{42, 41, 40}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- either - std::forward_list", "[algorithm]")
{
  using Result = frt::either<std::string, int>;

  std::vector input{
      Result{frt::in_place_right, 42},
      Result{frt::in_place_right, 41},
      Result{frt::in_place_right, 40},
  };

  std::forward_list<int> fl;

  const auto actualResult = frt::traverse_into<std::forward_list>(input, std::identity{});

  using ExpectedResultType = frt::either<std::string, std::forward_list<int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_right, std::forward_list{40, 41, 42}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- either - std::map", "[algorithm]")
{
  using P = std::pair<int, int>;
  using Result = frt::either<std::string, std::pair<int, int>>;

  std::vector input{
      Result{frt::in_place_right, P{42, 42}},
      Result{frt::in_place_right, P{41, 41}},
      Result{frt::in_place_right, P{40, 40}},
  };

  const auto actualResult = frt::traverse_into<std::map>(input, std::identity{});

  using ExpectedResultType = frt::either<std::string, std::map<int, int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_right, std::map{P{42, 42}, P{41, 41}, P{40, 40}}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- either - std::unordered_map", "[algorithm]")
{
  using P = std::pair<int, int>;
  using Result = frt::either<std::string, std::pair<int, int>>;

  std::vector input{Result{frt::in_place_right, P{42, 42}}};

  const auto actualResult = frt::traverse_into<std::unordered_map>(input, std::identity{});

  using ExpectedResultType = frt::either<std::string, std::unordered_map<int, int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_right, std::unordered_map{P{42, 42}}};

  REQUIRE(actualResult == expectedResult);
}

TEST_CASE("frt::traverse -- either - std::set", "[algorithm]")
{
  using Result = frt::either<std::string, int>;

  std::vector input{
      Result{frt::in_place_right, 42},
      Result{frt::in_place_right, 41},
      Result{frt::in_place_right, 40},
  };

  const auto actualResult = input | frt::traverse_into<std::set>(std::identity{});

  using ExpectedResultType = frt::either<std::string, std::set<int>>;
  const auto expectedResult = ExpectedResultType{frt::in_place_right, std::set{42, 41, 40}};

  REQUIRE(actualResult == expectedResult);
}
