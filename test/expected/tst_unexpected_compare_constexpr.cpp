#include <catch2/catch_all.hpp>

#include <compare>
#include <string>

#include <frt/expected.hpp>

constexpr auto makeS1()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}

constexpr auto makeS2()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}

constexpr auto makeIntVal1() { return 10; }
constexpr auto makeIntVal2() { return 20; }
constexpr auto makeDoubleVal1() { return 10.0; }
constexpr auto makeDoubleVal2() { return 20.0; }

TEST_CASE("Compare unexpected (constexpr)", "[unexpected][constexpr]")
{
  SECTION("Compare same type - equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS1()};
      return u1 == u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - not equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return u1 != u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - less than")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return u1 < u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - less or equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return u1 <= u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - greater than")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return u2 > u1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - greater or equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return u2 >= u1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - three-way less")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return (u1 <=> u2) == std::strong_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - three-way equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS1()};
      return (u1 <=> u2) == std::strong_ordering::equal;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Compare same type - three-way greater")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeS1()};
      const frt::unexpected u2{makeS2()};
      return (u2 <=> u1) == std::strong_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal1()};
      return u1 == u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - not equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal2()};
      return u1 != u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - less than")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal2()};
      return u1 < u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - less or equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal2()};
      return u1 <= u2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - greater than")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal2()};
      return u2 > u1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - greater or equal")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal2()};
      return u2 >= u1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - three-way less")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal2()};
      return (u1 <=> u2) == std::partial_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - three-way equivalent")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal1()};
      const frt::unexpected u2{makeDoubleVal1()};
      return (u1 <=> u2) == std::partial_ordering::equivalent;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Heterogeneous compare - three-way greater")
  {
    constexpr auto valid = [] {
      const frt::unexpected u1{makeIntVal2()};
      const frt::unexpected u2{makeDoubleVal1()};
      return (u1 <=> u2) == std::partial_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }
}
