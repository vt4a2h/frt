#include "common.hpp"

using namespace test::expected;

TEST_CASE("Create expected<void, E> default constructor (has_value)", "[expected]")
{
  const ExpectedVoid e;

  REQUIRE(e.has_value());
}

TEST_CASE("Create expected<void, E> default constructor (bool operator)", "[expected]")
{
  const ExpectedVoid e;

  REQUIRE(static_cast<bool>(e));
}

TEST_CASE("Create expected<void, E> in_place_expected (has_value)", "[expected]")
{
  const ExpectedVoid e{frt::in_place_expected};

  REQUIRE(e.has_value());
}

TEST_CASE("Create expected<void, E> in_place_expected (from frt::void)", "[expected]")
{
  const ExpectedVoid e{frt::in_place_expected, frt::void_val};

  REQUIRE(e.value() == frt::void_val);
}

TEST_CASE("Create expected<void, E> in_place_expected (no extra arg)", "[expected]")
{
  const ExpectedVoid e{frt::in_place_expected};

  REQUIRE(e.value() == frt::void_val);
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (has_value)", "[expected]")
{
  const frt::unexpected u{expectedError};
  const ExpectedVoid e{u};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (error)", "[expected]")
{
  const frt::unexpected u{expectedError};
  const ExpectedVoid e{u};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (convertible type) (has_value)", "[expected]")
{
  const frt::unexpected u{expectedError.data()};
  const ExpectedVoid e{u};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> unexpected copy constructor (convertible type) (error)", "[expected]")
{
  const frt::unexpected u{expectedError.data()};
  const ExpectedVoid e{u};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create expected<void, E> unexpected move constructor (has_value)", "[expected]")
{
  frt::unexpected u{expectedError};
  const ExpectedVoid e{std::move(u)};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> unexpected move constructor (error)", "[expected]")
{
  frt::unexpected u{expectedError};
  const ExpectedVoid e{std::move(u)};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create expected<void, E> unexpected move constructor (rvalue) (has_value)", "[expected]")
{
  const ExpectedVoid e{frt::unexpected{std::string{expectedError}}};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> unexpected move constructor (rvalue) (error)", "[expected]")
{
  const ExpectedVoid e{frt::unexpected{std::string{expectedError}}};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (has_value)", "[expected]")
{
  const ExpectedVoid e{frt::in_place_unexpected, expectedError};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (error)", "[expected]")
{
  const ExpectedVoid e{frt::in_place_unexpected, expectedError};

  REQUIRE(e.error() == expectedError);
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (custom struct) (has_value)", "[expected]")
{
  using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
  const ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> in_place_unexpected (Args...) (custom struct) (error)", "[expected]")
{
  using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
  const ExpectedVoidFoo e{frt::in_place_unexpected, 1, 2.0};

  REQUIRE(e.error() == TestArgsIl{1, 2.0});
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list, Args...) (custom struct) (has_value)",
          "[expected]")
{
  using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
  const ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list, Args...) (custom struct) (error)",
          "[expected]")
{
  using ExpectedVoidFoo = frt::expected<void, TestArgsIl>;
  const ExpectedVoidFoo e{frt::in_place_unexpected, {"a"s, "b"s}, 1, 2.0};

  REQUIRE(e.error() == TestArgsIl{{"a"s, "b"s}, 1, 2.0});
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list) (vector) (has_value)", "[expected]")
{
  using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
  const ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE_FALSE(e.has_value());
}

TEST_CASE("Create expected<void, E> in_place_unexpected (initializer_list) (vector) (error)", "[expected]")
{
  using ExpectedVoidVector = frt::expected<void, std::vector<int>>;
  const ExpectedVoidVector e{frt::in_place_unexpected, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

  REQUIRE(e.error() == expectedVectorValue);
}

TEST_CASE("Create expected<void, E> copy constructor (value) (has_value)", "[expected]")
{
  const ExpectedVoid e;
  const ExpectedVoid other{e};

  REQUIRE(other.has_value());
}

TEST_CASE("Create expected<void, E> copy constructor (error) (has_value)", "[expected]")
{
  const ExpectedVoid e{frt::unexpected{expectedError}};
  const ExpectedVoid other{e};

  REQUIRE_FALSE(other.has_value());
}

TEST_CASE("Create expected<void, E> copy constructor (error) (error)", "[expected]")
{
  const ExpectedVoid e{frt::unexpected{expectedError}};
  const ExpectedVoid other{e};

  REQUIRE(other.error() == expectedError);
}

TEST_CASE("Create expected<void, E> move constructor (value) (has_value)", "[expected]")
{
  ExpectedVoid e;
  const ExpectedVoid other{std::move(e)};

  REQUIRE(other.has_value());
}

TEST_CASE("Create expected<void, E> move constructor (error) (has_value)", "[expected]")
{
  ExpectedVoid e{frt::unexpected{expectedError}};
  const ExpectedVoid other{std::move(e)};

  REQUIRE_FALSE(other.has_value());
}

TEST_CASE("Create expected<void, E> move constructor (error) (error)", "[expected]")
{
  ExpectedVoid e{frt::unexpected{expectedError}};
  const ExpectedVoid other{std::move(e)};

  REQUIRE(other.error() == expectedError);
}

TEST_CASE("Create expected<void, E> converting copy constructor (value) (has_value)", "[expected]")
{
  const frt::expected<void, int> src;
  const frt::expected<void, double> dest{src};

  REQUIRE(dest.has_value());
}

TEST_CASE("Create expected<void, E> converting copy constructor (error) (has_value)", "[expected]")
{
  const int intVal = 42;
  const frt::expected<void, int> src{frt::unexpected{intVal}};
  const frt::expected<void, double> dest{src};

  REQUIRE_FALSE(dest.has_value());
}

TEST_CASE("Create expected<void, E> converting copy constructor (error) (error)", "[expected]")
{
  const int intVal = 42;
  const double expectedDoubleVal = 42.0;
  const frt::expected<void, int> src{frt::unexpected{intVal}};
  const frt::expected<void, double> dest{src};

  REQUIRE(dest.error() == expectedDoubleVal);
}

TEST_CASE("Create expected<void, E> converting move constructor (value) (has_value)", "[expected]")
{
  frt::expected<void, int> src;
  const frt::expected<void, double> dest{std::move(src)};

  REQUIRE(dest.has_value());
}

TEST_CASE("Create expected<void, E> converting move constructor (error) (has_value)", "[expected]")
{
  const int intVal = 42;
  frt::expected<void, int> src{frt::unexpected{intVal}};
  const frt::expected<void, double> dest{std::move(src)};

  REQUIRE_FALSE(dest.has_value());
}

TEST_CASE("Create expected<void, E> converting move constructor (error) (error)", "[expected]")
{
  const int intVal = 42;
  const double expectedDoubleVal = 42.0;
  frt::expected<void, int> src{frt::unexpected{intVal}};
  const frt::expected<void, double> dest{std::move(src)};

  REQUIRE(dest.error() == expectedDoubleVal);
}

TEST_CASE("Create expected<void, E> copy assignment (value to error) (has_value)", "[expected]")
{
  const ExpectedVoid e1;
  ExpectedVoid e2{frt::unexpected{""s}};
  e2 = e1;

  REQUIRE(e2.has_value());
}

TEST_CASE("Create expected<void, E> copy assignment (value to value) (has_value)", "[expected]")
{
  const ExpectedVoid e1;
  ExpectedVoid e2;
  e2 = e1;

  REQUIRE(e2.has_value());
}

TEST_CASE("Create expected<void, E> copy assignment (error to value) (has_value)", "[expected]")
{
  const ExpectedVoid e1{frt::unexpected{expectedError}};
  ExpectedVoid e2;
  e2 = e1;

  REQUIRE_FALSE(e2.has_value());
}

TEST_CASE("Create expected<void, E> copy assignment (error to value) (error)", "[expected]")
{
  const ExpectedVoid e1{frt::unexpected{expectedError}};
  ExpectedVoid e2;
  e2 = e1;

  REQUIRE(e2.error() == expectedError);
}

TEST_CASE("Create expected<void, E> copy assignment (error to error) (error)", "[expected]")
{
  const ExpectedVoid e1{frt::unexpected{expectedError}};
  ExpectedVoid e2{frt::unexpected{""s}};
  e2 = e1;

  REQUIRE(e2.error() == expectedError);
}

TEST_CASE("Create expected<void, E> move assignment (value to error) (has_value)", "[expected]")
{
  ExpectedVoid e1;
  ExpectedVoid e2{frt::unexpected{""s}};
  e2 = std::move(e1);

  REQUIRE(e2.has_value());
}

TEST_CASE("Create expected<void, E> move assignment (value to value) (has_value)", "[expected]")
{
  ExpectedVoid e1;
  ExpectedVoid e2;
  e2 = std::move(e1);

  REQUIRE(e2.has_value());
}

TEST_CASE("Create expected<void, E> move assignment (error to value) (has_value)", "[expected]")
{
  ExpectedVoid e1{frt::unexpected{expectedError}};
  ExpectedVoid e2;
  e2 = std::move(e1);

  REQUIRE_FALSE(e2.has_value());
}

TEST_CASE("Create expected<void, E> move assignment (error to value) (error)", "[expected]")
{
  ExpectedVoid e1{frt::unexpected{expectedError}};
  ExpectedVoid e2;
  e2 = std::move(e1);

  REQUIRE(e2.error() == expectedError);
}

TEST_CASE("Create expected<void, E> move assignment (error to error) (error)", "[expected]")
{
  ExpectedVoid e1{frt::unexpected{expectedError}};
  ExpectedVoid e2{frt::unexpected{""s}};
  e2 = std::move(e1);

  REQUIRE(e2.error() == expectedError);
}
