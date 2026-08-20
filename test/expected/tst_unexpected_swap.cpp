#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

TEST_CASE("Swap", "[unexpected]")
{
  const std::string stringVal1 =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua";
  const std::string stringVal2 =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";
  const std::vector vectorVal1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::vector vectorVal2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

  SECTION("String (member swap - first object gets second value)")
  {
    frt::unexpected u1{stringVal1};
    frt::unexpected u2{stringVal2};
    u1.swap(u2);
    REQUIRE(u1.error() == stringVal2);
  }

  SECTION("String (member swap - second object gets first value)")
  {
    frt::unexpected u1{stringVal1};
    frt::unexpected u2{stringVal2};
    u1.swap(u2);
    REQUIRE(u2.error() == stringVal1);
  }

  SECTION("String (non-member swap - first object gets second value)")
  {
    frt::unexpected u1{stringVal1};
    frt::unexpected u2{stringVal2};
    using std::swap;
    swap(u1, u2);
    REQUIRE(u1.error() == stringVal2);
  }

  SECTION("String (non-member swap - second object gets first value)")
  {
    frt::unexpected u1{stringVal1};
    frt::unexpected u2{stringVal2};
    using std::swap;
    swap(u1, u2);
    REQUIRE(u2.error() == stringVal1);
  }

  SECTION("Vector (member swap - first object gets second value)")
  {
    frt::unexpected u1{vectorVal1};
    frt::unexpected u2{vectorVal2};
    u1.swap(u2);
    REQUIRE(u1.error() == vectorVal2);
  }

  SECTION("Vector (member swap - second object gets first value)")
  {
    frt::unexpected u1{vectorVal1};
    frt::unexpected u2{vectorVal2};
    u1.swap(u2);
    REQUIRE(u2.error() == vectorVal1);
  }

  SECTION("Vector (non-member swap - first object gets second value)")
  {
    frt::unexpected u1{vectorVal1};
    frt::unexpected u2{vectorVal2};
    using std::swap;
    swap(u1, u2);
    REQUIRE(u1.error() == vectorVal2);
  }

  SECTION("Vector (non-member swap - second object gets first value)")
  {
    frt::unexpected u1{vectorVal1};
    frt::unexpected u2{vectorVal2};
    using std::swap;
    swap(u1, u2);
    REQUIRE(u2.error() == vectorVal1);
  }
}
