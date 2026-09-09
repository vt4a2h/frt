#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create from either converting copy constructor from either (value) (has_value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  const Expected e{src};

  REQUIRE(e.has_value());
}

TEST_CASE("Create from either converting copy constructor from either (value) (value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  const Expected e{src};

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create from either converting copy constructor from either (error) (has_value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  const Expected e{src};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create from either converting copy constructor from either (error) (error)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  const Expected e{src};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create from either converting move constructor from either (value) (has_value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  const Expected e{std::move(src)};

  REQUIRE(e.has_value());
}

TEST_CASE("Create from either converting move constructor from either (value) (value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  const Expected e{std::move(src)};

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create from either converting move constructor from either (error) (has_value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  const Expected e{std::move(src)};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create from either converting move constructor from either (error) (error)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  const Expected e{std::move(src)};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create from either copy assignment from either (value to value) (value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  Expected e{frt::in_place_expected, std::vector<int>{}};
  e = src;

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create from either copy assignment from either (value to error) (has_value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  Expected e{frt::unexpected{""s}};
  e = src;

  REQUIRE(e.has_value());
}

TEST_CASE("Create from either copy assignment from either (value to error) (value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  Expected e{frt::unexpected{""s}};
  e = src;

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create from either copy assignment from either (error to value) (has_value)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  Expected e{frt::in_place_expected, std::vector<int>{}};
  e = src;

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create from either copy assignment from either (error to value) (error)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  Expected e{frt::in_place_expected, std::vector<int>{}};
  e = src;

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create from either copy assignment from either (error to error) (error)", "[expected]")
{
  const frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  Expected e{frt::unexpected{""s}};
  e = src;

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create from either move assignment from either (value to value) (value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  Expected e{frt::in_place_expected, std::vector<int>{}};
  e = std::move(src);

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create from either move assignment from either (value to error) (has_value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  Expected e{frt::unexpected{""s}};
  e = std::move(src);

  REQUIRE(e.has_value());
}

TEST_CASE("Create from either move assignment from either (value to error) (value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_left, expectedValue};
  Expected e{frt::unexpected{""s}};
  e = std::move(src);

  REQUIRE(e.value() == expectedValue);
}

TEST_CASE("Create from either move assignment from either (error to value) (has_value)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  Expected e{frt::in_place_expected, std::vector<int>{}};
  e = std::move(src);

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create from either move assignment from either (error to value) (error)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  Expected e{frt::in_place_expected, std::vector<int>{}};
  e = std::move(src);

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create from either move assignment from either (error to error) (error)", "[expected]")
{
  frt::either<std::vector<int>, std::string> src{frt::in_place_right, expectedError};
  Expected e{frt::unexpected{""s}};
  e = std::move(src);

  REQUIRE(e.error() == expectedError);
}
