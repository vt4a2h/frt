#include "catch.hpp"

#include <frt/expects.hpp>

struct AlwaysTrue
{
  constexpr explicit operator bool() const { return true; }
};

TEST_CASE("Bool value", "[expects]") { frt::expects(true); }
TEST_CASE("Expression", "[expects]") { frt::expects(1 < 2); }
TEST_CASE("Bool testable", "[expects]") { frt::expects(AlwaysTrue{}); }
