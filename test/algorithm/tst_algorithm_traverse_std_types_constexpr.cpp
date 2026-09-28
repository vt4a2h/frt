#include <catch2/catch_all.hpp>

#include <frt/algorithm/traverse.hpp>

#include <array>
#include <expected>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace frt
{

template <class T, class E>
struct meta_either<std::expected<T, E>>
{
  using Self = std::expected<T, E>;

  using right_t = Self::value_type;
  using left_t = Self::error_type;

  template <class U>
  using rebind_right = Self::template rebind<U>;

  static constexpr auto left = []<std::same_as<Self> S>(S &&self) -> decltype(auto) {
    return std::forward<S>(self).error();
  };
  static constexpr auto right = []<std::same_as<Self> S>(S &&self) -> decltype(auto) {
    if constexpr (std::is_void_v<std::remove_cvref_t<right_t>>) {
      return void_val;
    } else {
      return std::forward<S>(self).value();
    }
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&val) { return Self{std::unexpect, std::forward<U>(val)}; };
  static constexpr auto make_right = []<class U>(U &&val) {
    if constexpr (std::same_as<void_t, std::remove_cvref_t<U>>) {
      (void)val;
      return Self{std::in_place};
    } else {
      return Self{std::in_place, std::forward<U>(val)};
    }
  };
};

template <class L>
struct meta_either<std::optional<L>>
{
  using Self = std::optional<L>;

  using right_t = Self::value_type;
  using left_t = void_t;

  template <class U>
  using rebind_right = std::optional<U>;

  template <class>
  using rebind_left = std::optional<L>;

  static constexpr auto left = []<std::same_as<Self> T>(T &&) { return void_val; };
  static constexpr auto right = []<std::same_as<Self> T>(T &&self) -> decltype(auto) {
    return std::forward<T>(self).value();
  };

  static constexpr auto is_left = [](const Self &self) { return !self.has_value(); };
  static constexpr auto is_right = [](const Self &self) { return self.has_value(); };

  static constexpr auto make_left = []<class U>(U &&) { return Self{}; };
  static constexpr auto make_right = []<class U>(U &&val) { return Self{std::in_place, std::forward<U>(val)}; };
};

template <class T, class E>
inline constexpr bool is_result_type<std::expected<T, E>> = true;

template <class T>
inline constexpr bool is_result_type<std::optional<T>> = true;

} // namespace frt

TEST_CASE("frt::traverse constexpr -- std::expected", "[algorithm][traverse][constexpr]")
{
  using Result = std::expected<int, double>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place, 42},
          Result{std::in_place, 41},
          Result{std::in_place, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      using ExpectedResult = std::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{std::in_place, std::vector{42, 41, 40}};
      return actual.has_value() && actual.value() == expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place, 42},
          Result{std::in_place, 41},
          Result{std::in_place, 40},
      };

      const auto actual = input | frt::traverse(std::identity{});
      using ExpectedResult = std::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{std::in_place, std::vector{42, 41, 40}};
      return actual.has_value() && actual.value() == expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure short-circuits and preserves error")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place, 42},
          Result{std::unexpect, -1},
          Result{std::in_place, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      return !actual.has_value() && actual.error() == -1;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse constexpr -- std::expected<void, ...>", "[algorithm][traverse][constexpr]")
{
  using Result = std::expected<void, int>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place},
          Result{std::in_place},
          Result{std::in_place},
      };

      const auto actual = frt::traverse(input, [](const Result &) -> std::expected<int, double> { return 42; });
      using ExpectedResult = std::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{std::in_place, std::vector{42, 42, 42}};
      return actual.has_value() && actual.value() == expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place},
          Result{std::in_place},
          Result{std::in_place},
      };

      const auto actual = input | frt::traverse([](const Result &) -> std::expected<int, double> { return 42; });
      using ExpectedResult = std::expected<std::vector<int>, int>;
      const auto expected = ExpectedResult{std::in_place, std::vector{42, 42, 42}};
      return actual.has_value() && actual.value() == expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("failure from mapped function")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place},
          Result{std::in_place},
      };

      int callCount = 0;
      auto fn = [&callCount](const Result &) -> std::expected<int, double> {
        ++callCount;
        if (callCount == 1) {
          return std::unexpected{-42};
        }
        return 100;
      };

      const auto actual = frt::traverse(input, fn);
      return !actual.has_value() && actual.error() == -42 && callCount == 1;
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("frt::traverse constexpr -- optional", "[algorithm][traverse][constexpr]")
{
  using Result = std::optional<int>;

  SECTION("success")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place, 42},
          Result{std::in_place, 41},
          Result{std::in_place, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      using ExpectedResult = std::optional<std::vector<int>>;
      const auto expected = ExpectedResult{std::in_place, std::vector{42, 41, 40}};
      return actual.has_value() && actual.value() == expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("success with pipe operator")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place, 42},
          Result{std::in_place, 41},
          Result{std::in_place, 40},
      };

      const auto actual = input | frt::traverse(std::identity{});
      using ExpectedResult = std::optional<std::vector<int>>;
      const auto expected = ExpectedResult{std::in_place, std::vector{42, 41, 40}};
      return actual.has_value() && actual.value() == expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("empty optional short-circuits")
  {
    constexpr auto valid = [] {
      const std::array input{
          Result{std::in_place, 42},
          Result{},
          Result{std::in_place, 40},
      };

      const auto actual = frt::traverse(input, std::identity{});
      return !actual.has_value();
    }();

    STATIC_REQUIRE(valid);
  }
}
