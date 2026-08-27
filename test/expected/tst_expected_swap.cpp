#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

TEST_CASE("Swap", "[expected]")
{
  using Expected = frt::expected<std::vector<int>, std::string>;

  const std::vector value1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::vector value2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  const std::string error1 =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua";
  const std::string error2 =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";

  SECTION("Both value (member swap - first object gets second value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e1.value() == value2);
  }

  SECTION("Both value (member swap - second object gets first value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e2.value() == value1);
  }

  SECTION("Both value (member swap - first object has value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Both value (member swap - second object has value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Both value (non-member swap - first object gets second value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.value() == value2);
  }

  SECTION("Both value (non-member swap - second object gets first value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.value() == value1);
  }

  SECTION("Both value (non-member swap - first object has value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Both value (non-member swap - second object has value)")
  {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Both error (member swap - first object gets second error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Both error (member swap - second object gets first error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e2.error() == error1);
  }

  SECTION("Both error (member swap - first object has error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Both error (member swap - second object has error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Both error (non-member swap - first object gets second error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Both error (non-member swap - second object gets first error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.error() == error1);
  }

  SECTION("Both error (non-member swap - first object has error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Both error (non-member swap - second object has error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Value and error (member swap - first object has error)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Value and error (member swap - first object gets error)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Value and error (member swap - second object has value)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Value and error (member swap - second object gets value)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e2.value() == value1);
  }

  SECTION("Value and error (non-member swap - first object has error)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Value and error (non-member swap - first object gets error)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Value and error (non-member swap - second object has value)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Value and error (non-member swap - second object gets value)")
  {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.value() == value1);
  }

  SECTION("Error and value (member swap - first object has value)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Error and value (member swap - first object gets value)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e1.value() == value2);
  }

  SECTION("Error and value (member swap - second object has error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Error and value (member swap - second object gets error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    REQUIRE(e2.error() == error1);
  }

  SECTION("Error and value (non-member swap - first object has value)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Error and value (non-member swap - first object gets value)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.value() == value2);
  }

  SECTION("Error and value (non-member swap - second object has error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Error and value (non-member swap - second object gets error)")
  {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.error() == error1);
  }
}

TEST_CASE("Swap <void, E>", "[expected]")
{
  using ExpectedVoid = frt::expected<void, std::string>;

  const std::string error1 =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua";
  const std::string error2 =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";

  SECTION("Both value (member swap - first object has value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2;
    e1.swap(e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Both value (member swap - second object has value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2;
    e1.swap(e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Both value (non-member swap - first object has value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Both value (non-member swap - second object has value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Both error (member swap - first object gets second error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Both error (member swap - second object gets first error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e2.error() == error1);
  }

  SECTION("Both error (member swap - first object has error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Both error (member swap - second object has error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Both error (non-member swap - first object gets second error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Both error (non-member swap - second object gets first error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.error() == error1);
  }

  SECTION("Both error (non-member swap - first object has error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Both error (non-member swap - second object has error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Value and error (member swap - first object has error)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Value and error (member swap - first object gets error)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Value and error (member swap - second object has value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Value and error (non-member swap - first object has error)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e1.has_value());
  }

  SECTION("Value and error (non-member swap - first object gets error)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.error() == error2);
  }

  SECTION("Value and error (non-member swap - second object has value)")
  {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.has_value());
  }

  SECTION("Error and value (member swap - first object has value)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    e1.swap(e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Error and value (member swap - second object has error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    e1.swap(e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Error and value (member swap - second object gets error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    e1.swap(e2);
    REQUIRE(e2.error() == error1);
  }

  SECTION("Error and value (non-member swap - first object has value)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    REQUIRE(e1.has_value());
  }

  SECTION("Error and value (non-member swap - second object has error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    REQUIRE_FALSE(e2.has_value());
  }

  SECTION("Error and value (non-member swap - second object gets error)")
  {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    REQUIRE(e2.error() == error1);
  }
}
