#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

TEST_CASE("Swap", "[either]")
{
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  const std::vector leftValue1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::vector leftValue2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  const std::string rightValue1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                  "incididunt ut labore et dolore magna aliqua";
  const std::string rightValue2 =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque "
      "laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi "
      "architecto beatae vitae dicta sunt explicabo.";

  SECTION("Both left (member swap)")
  {
    Either e1{frt::in_place_left, leftValue1};
    Either e2{frt::in_place_left, leftValue2};

    e1.swap(e2);

    REQUIRE(e1.left() == leftValue2);
    REQUIRE(e2.left() == leftValue1);
  }

  SECTION("Both left (non-member swap)")
  {
    Either e1{frt::in_place_left, leftValue1};
    Either e2{frt::in_place_left, leftValue2};

    using std::swap;
    swap(e1, e2);

    REQUIRE(e1.left() == leftValue2);
    REQUIRE(e2.left() == leftValue1);
  }

  SECTION("Both right (member swap)")
  {
    Either e1{frt::in_place_right, rightValue1};
    Either e2{frt::in_place_right, rightValue2};

    e1.swap(e2);

    REQUIRE(e1.right() == rightValue2);
    REQUIRE(e2.right() == rightValue1);
  }

  SECTION("Both right (non-member swap)")
  {
    Either e1{frt::in_place_right, rightValue1};
    Either e2{frt::in_place_right, rightValue2};

    using std::swap;
    swap(e1, e2);

    REQUIRE(e1.right() == rightValue2);
    REQUIRE(e2.right() == rightValue1);
  }

  SECTION("Left and Right (member swap)")
  {
    Either e1{frt::in_place_left, leftValue1};
    Either e2{frt::in_place_right, rightValue2};

    e1.swap(e2);

    REQUIRE(e1.is_right());
    REQUIRE(e1.right() == rightValue2);
    REQUIRE(e2.is_left());
    REQUIRE(e2.left() == leftValue1);
  }

  SECTION("Left and Right (non-member swap)")
  {
    Either e1{frt::in_place_left, leftValue1};
    Either e2{frt::in_place_right, rightValue2};

    using std::swap;
    swap(e1, e2);

    REQUIRE(e1.is_right());
    REQUIRE(e1.right() == rightValue2);
    REQUIRE(e2.is_left());
    REQUIRE(e2.left() == leftValue1);
  }

  SECTION("Right and Left (member swap)")
  {
    Either e1{frt::in_place_right, rightValue1};
    Either e2{frt::in_place_left, leftValue2};

    e1.swap(e2);

    REQUIRE(e1.is_left());
    REQUIRE(e1.left() == leftValue2);
    REQUIRE(e2.is_right());
    REQUIRE(e2.right() == rightValue1);
  }

  SECTION("Right and Left (non-member swap)")
  {
    Either e1{frt::in_place_right, rightValue1};
    Either e2{frt::in_place_left, leftValue2};

    using std::swap;
    swap(e1, e2);

    REQUIRE(e1.is_left());
    REQUIRE(e1.left() == leftValue2);
    REQUIRE(e2.is_right());
    REQUIRE(e2.right() == rightValue1);
  }
}
