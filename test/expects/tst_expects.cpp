#include <catch2/catch_all.hpp>

#include <frt/expects.hpp>

struct AlwaysTrue
{
  constexpr explicit operator bool() const { return true; }
};

TEST_CASE("Bool value", "[expects]") { frt::detail::expects(true); }
TEST_CASE("Expression", "[expects]") { frt::detail::expects(1 < 2); }
TEST_CASE("Bool testable", "[expects]") { frt::detail::expects(AlwaysTrue{}); }
