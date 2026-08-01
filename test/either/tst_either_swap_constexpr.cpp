#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/either.hpp>

TEST_CASE("Swap (constexpr)", "[either][constexpr]")
{
  using namespace std::literals;
  using Either = frt::either<std::vector<int>, std::string>;

  SECTION("Both left (member swap)")
  {
    constexpr auto valid = [] {
      const std::vector l1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      const std::vector l2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

      Either e1{frt::in_place_left, l1};
      Either e2{frt::in_place_left, l2};

      e1.swap(e2);

      return e1.left() == l2 && e2.left() == l1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Both left (non-member swap)")
  {
    constexpr auto valid = [] {
      const std::vector l1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      const std::vector l2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

      Either e1{frt::in_place_left, l1};
      Either e2{frt::in_place_left, l2};

      using std::swap;
      swap(e1, e2);

      return e1.left() == l2 && e2.left() == l1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Both right (member swap)")
  {
    constexpr auto valid = [] {
      const std::string r1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                             "incididunt ut labore et dolore magna aliqua";
      const std::string r2 = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque "
                             "laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi "
                             "architecto beatae vitae dicta sunt explicabo.";

      Either e1{frt::in_place_right, r1};
      Either e2{frt::in_place_right, r2};

      e1.swap(e2);

      return e1.right() == r2 && e2.right() == r1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Both right (non-member swap)")
  {
    constexpr auto valid = [] {
      const std::string r1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                             "incididunt ut labore et dolore magna aliqua";
      const std::string r2 = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque "
                             "laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi "
                             "architecto beatae vitae dicta sunt explicabo.";

      Either e1{frt::in_place_right, r1};
      Either e2{frt::in_place_right, r2};

      using std::swap;
      swap(e1, e2);

      return e1.right() == r2 && e2.right() == r1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Left and Right (member swap)")
  {
    constexpr auto valid = [] {
      const std::vector l1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      const std::string r2 = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque "
                             "laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi "
                             "architecto beatae vitae dicta sunt explicabo.";

      Either e1{frt::in_place_left, l1};
      Either e2{frt::in_place_right, r2};

      e1.swap(e2);

      return e1.is_right() && e1.right() == r2 && e2.is_left() && e2.left() == l1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Left and Right (non-member swap)")
  {
    constexpr auto valid = [] {
      const std::vector l1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      const std::string r2 = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque "
                             "laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi "
                             "architecto beatae vitae dicta sunt explicabo.";

      Either e1{frt::in_place_left, l1};
      Either e2{frt::in_place_right, r2};

      using std::swap;
      swap(e1, e2);

      return e1.is_right() && e1.right() == r2 && e2.is_left() && e2.left() == l1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Right and Left (member swap)")
  {
    constexpr auto valid = [] {
      const std::string r1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                             "incididunt ut labore et dolore magna aliqua";
      const std::vector l2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

      Either e1{frt::in_place_right, r1};
      Either e2{frt::in_place_left, l2};

      e1.swap(e2);

      return e1.is_left() && e1.left() == l2 && e2.is_right() && e2.right() == r1;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Right and Left (non-member swap)")
  {
    constexpr auto valid = [] {
      const std::string r1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                             "incididunt ut labore et dolore magna aliqua";
      const std::vector l2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

      Either e1{frt::in_place_right, r1};
      Either e2{frt::in_place_left, l2};

      using std::swap;
      swap(e1, e2);

      return e1.is_left() && e1.left() == l2 && e2.is_right() && e2.right() == r1;
    }();

    STATIC_REQUIRE(valid);
  }
}
