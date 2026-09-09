#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create copy and move copy constructor (value) (has_value)", "[expected]")
{
  const Expected e{expectedValue};
  const Expected other{e};

  REQUIRE(other.has_value());
}

TEST_CASE("Create copy and move copy constructor (value) (value)", "[expected]")
{
  const Expected e{expectedValue};
  const Expected other{e};

  REQUIRE(other.value() == expectedValue);
}

TEST_CASE("Create copy and move copy constructor (error) (has_value)", "[expected]")
{
  const Expected e{frt::unexpected{expectedError}};
  const Expected other{e};

  REQUIRE_FALSE(other.has_value());
}

TEST_CASE("Create copy and move copy constructor (error) (error)", "[expected]")
{
  const Expected e{frt::unexpected{expectedError}};
  const Expected other{e};

  REQUIRE(other.error() == expectedError);
}

TEST_CASE("Create copy and move move constructor (value) (has_value)", "[expected]")
{
  Expected e{expectedValue};
  const Expected other{std::move(e)};

  REQUIRE(other.has_value());
}

TEST_CASE("Create copy and move move constructor (value) (value)", "[expected]")
{
  Expected e{expectedValue};
  const Expected other{std::move(e)};

  REQUIRE(other.value() == expectedValue);
}

TEST_CASE("Create copy and move move constructor (error) (has_value)", "[expected]")
{
  Expected e{frt::unexpected{expectedError}};
  const Expected other{std::move(e)};

  REQUIRE_FALSE(other.has_value());
}

TEST_CASE("Create copy and move move constructor (error) (error)", "[expected]")
{
  Expected e{frt::unexpected{expectedError}};
  const Expected other{std::move(e)};

  REQUIRE(other.error() == expectedError);
}

TEST_CASE("Create copy and move converting copy constructor (value) (has_value)", "[expected]")
{
  const int intVal = 42;
  const frt::expected<int, std::string> src{intVal};
  const frt::expected<double, std::string> dest{src};

  REQUIRE(dest.has_value());
}

TEST_CASE("Create copy and move converting copy constructor (value) (value)", "[expected]")
{
  const int intVal = 42;
  const double expectedDoubleVal = 42.0;
  const frt::expected<int, std::string> src{intVal};
  const frt::expected<double, std::string> dest{src};

  REQUIRE(dest.value() == expectedDoubleVal);
}

TEST_CASE("Create copy and move converting copy constructor (error) (has_value)", "[expected]")
{
  const frt::expected<int, std::string> src{frt::unexpected{expectedError}};
  const frt::expected<double, std::string> dest{src};

  REQUIRE_FALSE(dest.has_value());
}

TEST_CASE("Create copy and move converting copy constructor (error) (error)", "[expected]")
{
  const frt::expected<int, std::string> src{frt::unexpected{expectedError}};
  const frt::expected<double, std::string> dest{src};

  REQUIRE(dest.error() == expectedError);
}

TEST_CASE("Create copy and move converting move constructor (value) (has_value)", "[expected]")
{
  const int intVal = 42;
  frt::expected<int, std::string> src{intVal};
  const frt::expected<double, std::string> dest{std::move(src)};

  REQUIRE(dest.has_value());
}

TEST_CASE("Create copy and move converting move constructor (value) (value)", "[expected]")
{
  const int intVal = 42;
  const double expectedDoubleVal = 42.0;
  frt::expected<int, std::string> src{intVal};
  const frt::expected<double, std::string> dest{std::move(src)};

  REQUIRE(dest.value() == expectedDoubleVal);
}

TEST_CASE("Create copy and move converting move constructor (error) (has_value)", "[expected]")
{
  frt::expected<int, std::string> src{frt::unexpected{expectedError}};
  const frt::expected<double, std::string> dest{std::move(src)};

  REQUIRE_FALSE(dest.has_value());
}

TEST_CASE("Create copy and move converting move constructor (error) (error)", "[expected]")
{
  frt::expected<int, std::string> src{frt::unexpected{expectedError}};
  const frt::expected<double, std::string> dest{std::move(src)};

  REQUIRE(dest.error() == expectedError);
}

TEST_CASE("Create copy and move copy assignment (value to value) (value)", "[expected]")
{
  const Expected e1{expectedValue};
  Expected e2;
  e2 = e1;

  REQUIRE(e2.value() == expectedValue);
}

TEST_CASE("Create copy and move copy assignment (value to error) (has_value)", "[expected]")
{
  const Expected e1{expectedValue};
  Expected e2{frt::unexpected{""s}};
  e2 = e1;

  REQUIRE(e2.has_value());
}

TEST_CASE("Create copy and move copy assignment (value to error) (value)", "[expected]")
{
  const Expected e1{expectedValue};
  Expected e2{frt::unexpected{""s}};
  e2 = e1;

  REQUIRE(e2.value() == expectedValue);
}

TEST_CASE("Create copy and move copy assignment (error to value) (has_value)", "[expected]")
{
  const Expected e1{frt::unexpected{expectedError}};
  Expected e2{expectedValue};
  e2 = e1;

  REQUIRE_FALSE(e2.has_value());
}

TEST_CASE("Create copy and move copy assignment (error to value) (error)", "[expected]")
{
  const Expected e1{frt::unexpected{expectedError}};
  Expected e2{expectedValue};
  e2 = e1;

  REQUIRE(e2.error() == expectedError);
}

TEST_CASE("Create copy and move copy assignment (error to error) (error)", "[expected]")
{
  const Expected e1{frt::unexpected{expectedError}};
  Expected e2{frt::unexpected{""s}};
  e2 = e1;

  REQUIRE(e2.error() == expectedError);
}

TEST_CASE("Create copy and move move assignment (value to value) (value)", "[expected]")
{
  Expected e1{expectedValue};
  Expected e2;
  e2 = std::move(e1);

  REQUIRE(e2.value() == expectedValue);
}

TEST_CASE("Create copy and move move assignment (value to error) (has_value)", "[expected]")
{
  Expected e1{expectedValue};
  Expected e2{frt::unexpected{""s}};
  e2 = std::move(e1);

  REQUIRE(e2.has_value());
}

TEST_CASE("Create copy and move move assignment (value to error) (value)", "[expected]")
{
  Expected e1{expectedValue};
  Expected e2{frt::unexpected{""s}};
  e2 = std::move(e1);

  REQUIRE(e2.value() == expectedValue);
}

TEST_CASE("Create copy and move move assignment (error to value) (has_value)", "[expected]")
{
  Expected e1{frt::unexpected{expectedError}};
  Expected e2{expectedValue};
  e2 = std::move(e1);

  REQUIRE_FALSE(e2.has_value());
}

TEST_CASE("Create copy and move move assignment (error to value) (error)", "[expected]")
{
  Expected e1{frt::unexpected{expectedError}};
  Expected e2{expectedValue};
  e2 = std::move(e1);

  REQUIRE(e2.error() == expectedError);
}

TEST_CASE("Create copy and move move assignment (error to error) (error)", "[expected]")
{
  Expected e1{frt::unexpected{expectedError}};
  Expected e2{frt::unexpected{""s}};
  e2 = std::move(e1);

  REQUIRE(e2.error() == expectedError);
}
