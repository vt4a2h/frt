#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

constexpr auto makeValue1() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeValue2() { return std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}; }
constexpr auto makeError1()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}
constexpr auto makeError2()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}

TEST_CASE("Swap (constexpr)", "[expected][constexpr]")
{
  using Expected = frt::expected<std::vector<int>, std::string>;

  SECTION("Both value (member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (member swap - first object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (member swap - second object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (non-member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (non-member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (non-member swap - first object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (non-member swap - second object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - first object gets second error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - second object gets first error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - first object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - second object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - first object gets second error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - second object gets first error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - first object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - second object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - first object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - first object gets error)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - second object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - second object gets value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - first object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - first object gets error)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - second object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - second object gets value)")
  {
    constexpr auto valid = [] {
      Expected e1{makeValue1()};
      Expected e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - first object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - first object gets value)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - second object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - second object gets error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      e1.swap(e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - first object has value)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - first object gets value)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - second object has error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - second object gets error)")
  {
    constexpr auto valid = [] {
      Expected e1{frt::unexpected{makeError1()}};
      Expected e2{makeValue2()};
      using std::swap;
      swap(e1, e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Swap <void, E> (constexpr)", "[expected][constexpr]")
{
  using ExpectedVoid = frt::expected<void, std::string>;

  SECTION("Both value (member swap - first object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2;
      e1.swap(e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (member swap - second object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2;
      e1.swap(e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (non-member swap - first object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2;
      using std::swap;
      swap(e1, e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both value (non-member swap - second object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2;
      using std::swap;
      swap(e1, e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - first object gets second error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - second object gets first error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - first object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (member swap - second object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - first object gets second error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - second object gets first error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - first object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Both error (non-member swap - second object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - first object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - first object gets error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (member swap - second object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      e1.swap(e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - first object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return !e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - first object gets error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e1.error() == makeError2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Value and error (non-member swap - second object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1;
      ExpectedVoid e2{frt::unexpected{makeError2()}};
      using std::swap;
      swap(e1, e2);
      return e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - first object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2;
      e1.swap(e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - second object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2;
      e1.swap(e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (member swap - second object gets error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2;
      e1.swap(e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - first object has value)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2;
      using std::swap;
      swap(e1, e2);
      return e1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - second object has error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2;
      using std::swap;
      swap(e1, e2);
      return !e2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Error and value (non-member swap - second object gets error)")
  {
    constexpr auto valid = [] {
      ExpectedVoid e1{frt::unexpected{makeError1()}};
      ExpectedVoid e2;
      using std::swap;
      swap(e1, e2);
      return e2.error() == makeError1();
    }();
    STATIC_REQUIRE(valid);
  }
}
