#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

constexpr auto makeExpectedValue() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedError()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

TEST_CASE("Emplace (constexpr)", "[expected][constexpr]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Expected = frt::expected<std::vector<int>, std::string>;

  SECTION("emplace on empty (has_value)")
  {
    constexpr auto valid = [] {
      Expected expected{std::vector<int>{}};
      expected.emplace(makeExpectedValue());
      return expected.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on empty (value)")
  {
    constexpr auto valid = [] {
      Expected expected{std::vector<int>{}};
      expected.emplace(makeExpectedValue());
      return expected.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on empty (return reference)")
  {
    constexpr auto valid = [] {
      Expected expected{std::vector<int>{}};
      const auto &ref = expected.emplace(makeExpectedValue());
      return &ref == &expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on value (has_value)")
  {
    constexpr auto valid = [] {
      Expected expected{std::vector{1, 2, 3}};
      expected.emplace(makeExpectedValue());
      return expected.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on value (value)")
  {
    constexpr auto valid = [] {
      Expected expected{std::vector{1, 2, 3}};
      expected.emplace(makeExpectedValue());
      return expected.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on value (return reference)")
  {
    constexpr auto valid = [] {
      Expected expected{std::vector{1, 2, 3}};
      const auto &ref = expected.emplace(makeExpectedValue());
      return &ref == &expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on error (has_value)")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      expected.emplace(makeExpectedValue());
      return expected.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on error (value)")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      expected.emplace(makeExpectedValue());
      return expected.value() == makeExpectedValue();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on error (return reference)")
  {
    constexpr auto valid = [] {
      Expected expected{frt::unexpected{makeExpectedError()}};
      const auto &ref = expected.emplace(makeExpectedValue());
      return &ref == &expected.value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace (Args...)")
  {
    constexpr auto valid = [] {
      using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
      ExpectedFoo expected{frt::unexpected{makeExpectedError()}};
      expected.emplace(1, 2.0);
      return expected.value() == TestArgsIl{1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace (initializer_list, Args...)")
  {
    constexpr auto valid = [] {
      using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
      ExpectedFoo expected{frt::unexpected{makeExpectedError()}};
      expected.emplace({"a"s, "b"s}, 1, 2.0);
      return expected.value() == TestArgsIl{{"a"s, "b"s}, 1, 2.0};
    }();

    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Emplace <void, E> (constexpr)", "[expected][constexpr]")
{
  using ExpectedVoid = frt::expected<void, std::string>;

  SECTION("emplace on value (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid expected;
      expected.emplace();
      return expected.has_value();
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("emplace on error (has_value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
      expected.emplace();
      return expected.has_value();
    }();

    STATIC_REQUIRE(valid);
  }
}
