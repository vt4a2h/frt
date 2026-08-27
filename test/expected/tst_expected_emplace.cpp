#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

TEST_CASE("Emplace", "[expected]")
{
  using namespace test::helpers;
  using namespace std::literals;
  using Expected = frt::expected<std::vector<int>, std::string>;

  const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::string expectedError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                    "incididunt ut labore et dolore magna aliqua";

  SECTION("emplace on empty (has_value)")
  {
    Expected expected{std::vector<int>{}};
    expected.emplace(expectedValue);

    REQUIRE(expected.has_value());
  }

  SECTION("emplace on empty (value)")
  {
    Expected expected{std::vector<int>{}};
    expected.emplace(expectedValue);

    REQUIRE(expected.value() == expectedValue);
  }

  SECTION("emplace on empty (return reference)")
  {
    Expected expected{std::vector<int>{}};
    const auto &ref = expected.emplace(expectedValue);

    REQUIRE(&ref == &expected.value());
  }

  SECTION("emplace on value (has_value)")
  {
    Expected expected{std::vector{1, 2, 3}};
    expected.emplace(expectedValue);

    REQUIRE(expected.has_value());
  }

  SECTION("emplace on value (value)")
  {
    Expected expected{std::vector{1, 2, 3}};
    expected.emplace(expectedValue);

    REQUIRE(expected.value() == expectedValue);
  }

  SECTION("emplace on value (return reference)")
  {
    Expected expected{std::vector{1, 2, 3}};
    const auto &ref = expected.emplace(expectedValue);

    REQUIRE(&ref == &expected.value());
  }

  SECTION("emplace on error (has_value)")
  {
    Expected expected{frt::unexpected{expectedError}};
    expected.emplace(expectedValue);

    REQUIRE(expected.has_value());
  }

  SECTION("emplace on error (value)")
  {
    Expected expected{frt::unexpected{expectedError}};
    expected.emplace(expectedValue);

    REQUIRE(expected.value() == expectedValue);
  }

  SECTION("emplace on error (return reference)")
  {
    Expected expected{frt::unexpected{expectedError}};
    const auto &ref = expected.emplace(expectedValue);

    REQUIRE(&ref == &expected.value());
  }

  SECTION("emplace (Args...)")
  {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    ExpectedFoo expected{frt::unexpected{expectedError}};
    expected.emplace(1, 2.0);

    REQUIRE(expected.value() == TestArgsIl{1, 2.0});
  }

  SECTION("emplace (initializer_list, Args...)")
  {
    using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
    ExpectedFoo expected{frt::unexpected{expectedError}};
    expected.emplace({"a"s, "b"s}, 1, 2.0);

    REQUIRE(expected.value() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
  }
}

TEST_CASE("Emplace <void, E>", "[expected]")
{
  using ExpectedVoid = frt::expected<void, std::string>;

  const std::string expectedError = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                    "incididunt ut labore et dolore magna aliqua";

  SECTION("emplace on value (has_value)")
  {
    ExpectedVoid expected;
    expected.emplace();

    REQUIRE(expected.has_value());
  }

  SECTION("emplace on error (has_value)")
  {
    ExpectedVoid expected{frt::unexpected{expectedError}};
    expected.emplace();

    REQUIRE(expected.has_value());
  }
}
