#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>
#include <frt/either.hpp>
#include <frt/optional.hpp>

#include <array>
#include <functional>
#include <utility>
#include <vector>

namespace
{

template <class T>
struct ConstexprFixedVector
{
  T data_[16]{};
  std::size_t size_{0};

  constexpr void push_back(const T &val) { data_[size_++] = val; }
  constexpr void push_back(T &&val) { data_[size_++] = std::move(val); }

  constexpr const T *begin() const noexcept { return data_; }
  constexpr const T *end() const noexcept { return data_ + size_; }
  constexpr std::size_t size() const noexcept { return size_; }

  constexpr bool operator==(const ConstexprFixedVector &other) const noexcept
  {
    if (size_ != other.size_) {
      return false;
    }
    for (std::size_t i = 0; i < size_; ++i) {
      if (data_[i] != other.data_[i]) {
        return false;
      }
    }
    return true;
  }
};

template <class T>
struct ConstexprCustomList
{
  T data_[16]{};
  std::size_t size_{0};

  constexpr void push_front(const T &val)
  {
    for (std::size_t i = size_; i > 0; --i) {
      data_[i] = data_[i - 1];
    }
    data_[0] = val;
    ++size_;
  }

  constexpr const T *begin() const noexcept { return data_; }
  constexpr const T *end() const noexcept { return data_ + size_; }
  constexpr std::size_t size() const noexcept { return size_; }

  constexpr bool operator==(const ConstexprCustomList &other) const noexcept
  {
    if (size_ != other.size_) {
      return false;
    }
    for (std::size_t i = 0; i < size_; ++i) {
      if (data_[i] != other.data_[i]) {
        return false;
      }
    }
    return true;
  }
};

template <class K, class V>
struct ConstexprFlatMap
{
  using value_type = std::pair<K, V>;
  value_type data_[16]{};
  std::size_t size_{0};

  constexpr void emplace(const std::pair<K, V> &p) { data_[size_++] = p; }
  constexpr void emplace(std::pair<K, V> &&p) { data_[size_++] = std::move(p); }

  constexpr const value_type *begin() const noexcept { return data_; }
  constexpr const value_type *end() const noexcept { return data_ + size_; }
  constexpr std::size_t size() const noexcept { return size_; }

  constexpr bool operator==(const ConstexprFlatMap &other) const noexcept
  {
    if (size_ != other.size_) {
      return false;
    }
    for (std::size_t i = 0; i < size_; ++i) {
      if (data_[i] != other.data_[i]) {
        return false;
      }
    }
    return true;
  }
};

} // namespace

namespace frt
{

template <class E>
struct meta_container<ConstexprCustomList<E>>
{
  static constexpr auto add_element = []<class ElemT>(ConstexprCustomList<E> &c, ElemT &&e) {
    c.push_front(std::forward<ElemT>(e));
  };
};

} // namespace frt

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

      const auto actual = frt::traverse_into<ConstexprFixedVector>(input, std::identity{});

      ConstexprFixedVector<int> expectedVec;
      expectedVec.push_back(42);
      expectedVec.push_back(41);
      expectedVec.push_back(40);

      using ExpectedResult = frt::either<int, ConstexprFixedVector<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, expectedVec};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with custom meta_container push_front")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_right, 42},
          Result{frt::in_place_right, 41},
          Result{frt::in_place_right, 40},
      };

      const auto actual = input | frt::traverse_into<ConstexprCustomList>(std::identity{});

      ConstexprCustomList<int> expectedList;
      expectedList.push_front(42);
      expectedList.push_front(41);
      expectedList.push_front(40);

      using ExpectedResult = frt::either<int, ConstexprCustomList<int>>;
      const auto expected = ExpectedResult{frt::in_place_right, expectedList};
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

      const auto actual = frt::traverse_into<ConstexprFixedVector>(input, std::identity{});
      return actual.is_left() && actual.left() == -99;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse_into constexpr -- associative container", "[algorithm][traverse][constexpr]")
{
  using P = std::pair<int, int>;
  using Result = frt::optional<P>;

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_opt, P{1, 10}},
          Result{frt::in_place_opt, P{2, 20}},
          Result{frt::in_place_opt, P{3, 30}},
      };

      const auto actual = input | frt::traverse_into<ConstexprFlatMap>(std::identity{});

      ConstexprFlatMap<int, int> expectedMap;
      expectedMap.emplace(P{1, 10});
      expectedMap.emplace(P{2, 20});
      expectedMap.emplace(P{3, 30});

      using ExpectedResult = frt::optional<ConstexprFlatMap<int, int>>;
      const auto expected = ExpectedResult{frt::in_place_opt, expectedMap};
      return actual == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("empty optional short-circuits")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{frt::in_place_opt, P{1, 10}},
          Result{},
          Result{frt::in_place_opt, P{3, 30}},
      };

      const auto actual = frt::traverse_into<ConstexprFlatMap>(input, std::identity{});
      return !actual.has_value();
    }();

    STATIC_REQUIRE(valid);
  }
}
