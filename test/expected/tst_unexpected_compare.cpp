#include <catch2/catch_all.hpp>

#include <string>

#include <frt/expected.hpp>

TEST_CASE("Compare unexpected", "[unexpected]")
{
  const std::string s1 =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua";
  const std::string s2 =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";

  const int intVal1 = 10;
  const int intVal2 = 20;
  const double doubleVal1 = 10.0;
  const double doubleVal2 = 20.0;

  SECTION("Compare same type - equal")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s1};
    REQUIRE(u1 == u2);
  }

  SECTION("Compare same type - not equal")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE(u1 != u2);
  }

  SECTION("Compare same type - less than")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE(u1 < u2);
  }

  SECTION("Compare same type - less or equal")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE(u1 <= u2);
  }

  SECTION("Compare same type - greater than")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE(u2 > u1);
  }

  SECTION("Compare same type - greater or equal")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE(u2 >= u1);
  }

  SECTION("Compare same type - three-way less")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE((u1 <=> u2) == std::strong_ordering::less);
  }

  SECTION("Compare same type - three-way equal")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s1};
    REQUIRE((u1 <=> u2) == std::strong_ordering::equal);
  }

  SECTION("Compare same type - three-way greater")
  {
    const frt::unexpected u1{s1};
    const frt::unexpected u2{s2};
    REQUIRE((u2 <=> u1) == std::strong_ordering::greater);
  }

  SECTION("Heterogeneous compare - equal")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal1};
    REQUIRE(u1 == u2);
  }

  SECTION("Heterogeneous compare - not equal")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal2};
    REQUIRE(u1 != u2);
  }

  SECTION("Heterogeneous compare - less than")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal2};
    REQUIRE(u1 < u2);
  }

  SECTION("Heterogeneous compare - less or equal")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal2};
    REQUIRE(u1 <= u2);
  }

  SECTION("Heterogeneous compare - greater than")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal2};
    REQUIRE(u2 > u1);
  }

  SECTION("Heterogeneous compare - greater or equal")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal2};
    REQUIRE(u2 >= u1);
  }

  SECTION("Heterogeneous compare - three-way less")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal2};
    REQUIRE((u1 <=> u2) == std::partial_ordering::less);
  }

  SECTION("Heterogeneous compare - three-way equivalent")
  {
    const frt::unexpected u1{intVal1};
    const frt::unexpected u2{doubleVal1};
    REQUIRE((u1 <=> u2) == std::partial_ordering::equivalent);
  }

  SECTION("Heterogeneous compare - three-way greater")
  {
    const frt::unexpected u1{intVal2};
    const frt::unexpected u2{doubleVal1};
    REQUIRE((u1 <=> u2) == std::partial_ordering::greater);
  }
}
