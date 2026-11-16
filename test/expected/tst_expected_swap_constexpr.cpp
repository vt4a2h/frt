#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

namespace
{
using Expected = frt::expected<std::array<int, 10>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

constexpr std::array value1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr std::array value2{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
constexpr std::string_view error1{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                                  "incididunt ut labore et dolore magna aliqua"};

constexpr std::string_view error2{
    "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
    "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
} // namespace

TEST_CASE("Swap (both value - member swap - first object gets second value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    return e1.value() == value2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - member swap - second object gets first value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    return e2.value() == value1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - member swap - first object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - member swap - second object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    e1.swap(e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - non-member swap - first object gets second value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e1.value() == value2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - non-member swap - second object gets first value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e2.value() == value1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - non-member swap - first object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both value - non-member swap - second object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - member swap - first object gets second error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - member swap - second object gets first error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - member swap - first object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - member swap - second object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - non-member swap - first object gets second error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - non-member swap - second object gets first error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - non-member swap - first object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (both error - non-member swap - second object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - member swap - first object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - member swap - first object gets error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - member swap - second object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - member swap - second object gets value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e2.value() == value1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - non-member swap - first object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - non-member swap - first object gets error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - non-member swap - second object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (value and error - non-member swap - second object gets value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{value1};
    Expected e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e2.value() == value1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - member swap - first object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - member swap - first object gets value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    return e1.value() == value2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - member swap - second object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - member swap - second object gets error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    e1.swap(e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - non-member swap - first object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - non-member swap - first object gets value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e1.value() == value2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - non-member swap - second object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap (error and value - non-member swap - second object gets error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    Expected e1{frt::unexpected{error1}};
    Expected e2{value2};
    using std::swap;
    swap(e1, e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both value - member swap - first object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2;
    e1.swap(e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both value - member swap - second object has value) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2;
    e1.swap(e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both value - non-member swap - first object has value) (constexpr)", "[expected][constexpr]")
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

TEST_CASE("Swap <void, E> (both value - non-member swap - second object has value) (constexpr)",
          "[expected][constexpr]")
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

TEST_CASE("Swap <void, E> (both error - member swap - first object gets second error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - member swap - second object gets first error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - member swap - first object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - member swap - second object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - non-member swap - first object gets second error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - non-member swap - second object gets first error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - non-member swap - first object has error) (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (both error - non-member swap - second object has error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (value and error - member swap - first object has error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (value and error - member swap - first object gets error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (value and error - member swap - second object has value) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    e1.swap(e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (value and error - non-member swap - first object has error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return !e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (value and error - non-member swap - first object gets error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e1.error() == error2;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (value and error - non-member swap - second object has value) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1;
    ExpectedVoid e2{frt::unexpected{error2}};
    using std::swap;
    swap(e1, e2);
    return e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (error and value - member swap - first object has value) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    e1.swap(e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (error and value - member swap - second object has error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    e1.swap(e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (error and value - member swap - second object gets error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    e1.swap(e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (error and value - non-member swap - first object has value) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    return e1.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (error and value - non-member swap - second object has error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    return !e2.has_value();
  }();
  STATIC_REQUIRE(valid);
}

TEST_CASE("Swap <void, E> (error and value - non-member swap - second object gets error) (constexpr)",
          "[expected][constexpr]")
{
  constexpr auto valid = [] {
    ExpectedVoid e1{frt::unexpected{error1}};
    ExpectedVoid e2;
    using std::swap;
    swap(e1, e2);
    return e2.error() == error1;
  }();
  STATIC_REQUIRE(valid);
}
