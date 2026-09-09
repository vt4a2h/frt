#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create default and value default constructor (has_value)", "[expected]")
{
  const Expected e;

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value default constructor (value)", "[expected]")
{
  const Expected e;

  REQUIRE(e.value() == std::vector<int>{});
}

TEST_CASE("Create default and value default constructor (bool operator)", "[expected]")
{
  const Expected e;

  REQUIRE(static_cast<bool>(e));
}

TEST_CASE("Create default and value value constructor (lvalue) (has_value)", "[expected]")
{
  const Expected e{expectedValue};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value value constructor (lvalue) (value)", "[expected]")
{
  const Expected e{expectedValue};

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create default and value value constructor (rvalue) (has_value)", "[expected]")
{
  const Expected e{std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value value constructor (rvalue) (value)", "[expected]")
{
  const Expected e{std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create default and value value constructor (convertible type) (has_value)", "[expected]")
{
  const int intVal = 42;
  const frt::expected<double, std::string> e{intVal};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value value constructor (convertible type) (value)", "[expected]")
{
  const int intVal = 42;
  const double expectedDoubleVal = 42.0;
  const frt::expected<double, std::string> e{intVal};

  REQUIRE(e.value() == expectedDoubleVal);
}

TEST_CASE("Create default and value in_place_expected (Args...) (has_value)", "[expected]")
{
  const Expected e{frt::in_place_expected, expectedValue};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value in_place_expected (Args...) (value)", "[expected]")
{
  const Expected e{frt::in_place_expected, expectedValue};

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create default and value in_place_expected (Args...) (custom struct) (has_value)", "[expected]")
{
  using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
  const ExpectedFoo e{frt::in_place_expected, 1, 2.0};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value in_place_expected (Args...) (custom struct) (value)", "[expected]")
{
  using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
  const ExpectedFoo e{frt::in_place_expected, 1, 2.0};

  REQUIRE(e.value() == TestArgsIl{1, 2.0});
}

TEST_CASE("Create default and value in_place_expected (initializer_list, Args...) (custom struct) (has_value)",
          "[expected]")
{
  using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
  const ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value in_place_expected (initializer_list, Args...) (custom struct) (value)",
          "[expected]")
{
  using ExpectedFoo = frt::expected<TestArgsIl, std::string>;
  const ExpectedFoo e{frt::in_place_expected, {"a"s, "b"s}, 1, 2.0};

  REQUIRE(e.value() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
}

TEST_CASE("Create default and value in_place_expected (initializer_list) (vector) (has_value)", "[expected]")
{
  const Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE(e.has_value());
}

TEST_CASE("Create default and value in_place_expected (initializer_list) (vector) (value)", "[expected]")
{
  const Expected e{frt::in_place_expected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE(e.value() == expectedValue);
}
