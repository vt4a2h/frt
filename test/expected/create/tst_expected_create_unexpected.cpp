#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create unexpected unexpected copy constructor (has_value)", "[expected]")
{
  const frt::unexpected u{expectedError};
  const Expected e{u};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected unexpected copy constructor (error)", "[expected]")
{
  const frt::unexpected u{expectedError};
  const Expected e{u};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create unexpected unexpected copy constructor (convertible type) (has_value)", "[expected]")
{
  const frt::unexpected u{expectedError.data()};
  const Expected e{u};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected unexpected copy constructor (convertible type) (error)", "[expected]")
{
  const frt::unexpected u{expectedError.data()};
  const Expected e{u};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create unexpected unexpected move constructor (has_value)", "[expected]")
{
  frt::unexpected u{expectedError};
  const Expected e{std::move(u)};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected unexpected move constructor (error)", "[expected]")
{
  frt::unexpected u{expectedError};
  const Expected e{std::move(u)};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create unexpected unexpected move constructor (rvalue) (has_value)", "[expected]")
{
  const Expected e{frt::unexpected{std::string{expectedError}}};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected unexpected move constructor (rvalue) (error)", "[expected]")
{
  const Expected e{frt::unexpected{std::string{expectedError}}};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (has_value)", "[expected]")
{
  const Expected e{frt::in_place_unexpected, expectedError};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (error)", "[expected]")
{
  const Expected e{frt::in_place_unexpected, expectedError};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (custom struct) (has_value)", "[expected]")
{
  using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
  const ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected in_place_unexpected (Args...) (custom struct) (error)", "[expected]")
{
  using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
  const ExpectedErrorFoo e{frt::in_place_unexpected, 1, 2.0};

  REQUIRE(e.error() == TestArgsIl{1, 2.0});
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list, Args...) (custom struct) (has_value)", "[expected]")
{
  using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
  const ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list, Args...) (custom struct) (error)", "[expected]")
{
  using ExpectedErrorFoo = frt::expected<std::string, TestArgsIl>;
  const ExpectedErrorFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

  REQUIRE(e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list) (vector) (has_value)", "[expected]")
{
  using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
  const ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create unexpected in_place_unexpected (initializer_list) (vector) (error)", "[expected]")
{
  using ExpectedErrorVector = frt::expected<std::string, std::vector<int>>;
  const ExpectedErrorVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE(e.error() == expectedValue);
}
