#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

constexpr auto makeStringVal1()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}

constexpr auto makeStringVal2()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}

constexpr auto makeVectorVal1() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeVectorVal2() { return std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}; }

TEST_CASE("Swap (constexpr)", "[unexpected][constexpr]")
{
  SECTION("String (member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeStringVal1()};
      frt::unexpected u2{makeStringVal2()};
      u1.swap(u2);
      return u1.error() == makeStringVal2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("String (member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeStringVal1()};
      frt::unexpected u2{makeStringVal2()};
      u1.swap(u2);
      return u2.error() == makeStringVal1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("String (non-member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeStringVal1()};
      frt::unexpected u2{makeStringVal2()};
      using std::swap;
      swap(u1, u2);
      return u1.error() == makeStringVal2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("String (non-member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeStringVal1()};
      frt::unexpected u2{makeStringVal2()};
      using std::swap;
      swap(u1, u2);
      return u2.error() == makeStringVal1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Vector (member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeVectorVal1()};
      frt::unexpected u2{makeVectorVal2()};
      u1.swap(u2);
      return u1.error() == makeVectorVal2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Vector (member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeVectorVal1()};
      frt::unexpected u2{makeVectorVal2()};
      u1.swap(u2);
      return u2.error() == makeVectorVal1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Vector (non-member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeVectorVal1()};
      frt::unexpected u2{makeVectorVal2()};
      using std::swap;
      swap(u1, u2);
      return u1.error() == makeVectorVal2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Vector (non-member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      frt::unexpected u1{makeVectorVal1()};
      frt::unexpected u2{makeVectorVal2()};
      using std::swap;
      swap(u1, u2);
      return u2.error() == makeVectorVal1();
    }();
    STATIC_REQUIRE(valid);
  }
}
