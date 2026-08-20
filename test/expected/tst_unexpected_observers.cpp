#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

TEST_CASE("Observers", "[unexpected]")
{
  const std::string expectedStringValue =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua";
  const std::string modifiedStringValue =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";
  const std::vector expectedVectorValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  SECTION("error (non-const lvalue)")
  {
    frt::unexpected u{expectedStringValue};
    REQUIRE(u.error() == expectedStringValue);
  }

  SECTION("error (non-const lvalue - modification)")
  {
    frt::unexpected u{expectedStringValue};
    u.error() = modifiedStringValue;
    REQUIRE(u.error() == modifiedStringValue);
  }

  SECTION("error (const lvalue)")
  {
    const frt::unexpected u{expectedStringValue};
    REQUIRE(u.error() == expectedStringValue);
  }

  SECTION("error (rvalue)")
  {
    frt::unexpected u{expectedStringValue};
    REQUIRE(std::move(u).error() == expectedStringValue);
  }

  SECTION("error (const rvalue)")
  {
    const frt::unexpected u{expectedStringValue};
    REQUIRE(static_cast<const frt::unexpected<std::string> &&>(u).error() == expectedStringValue);
  }

  SECTION("error for vector (non-const lvalue)")
  {
    frt::unexpected u{expectedVectorValue};
    REQUIRE(u.error() == expectedVectorValue);
  }

  SECTION("error for vector (non-const lvalue - modification)")
  {
    frt::unexpected u{expectedVectorValue};
    u.error().clear();
    REQUIRE(u.error().empty());
  }

  SECTION("error for vector (const lvalue)")
  {
    const frt::unexpected u{expectedVectorValue};
    REQUIRE(u.error() == expectedVectorValue);
  }

  SECTION("error for vector (rvalue)")
  {
    frt::unexpected u{expectedVectorValue};
    REQUIRE(std::move(u).error() == expectedVectorValue);
  }

  SECTION("error for vector (const rvalue)")
  {
    const frt::unexpected u{expectedVectorValue};
    REQUIRE(static_cast<const frt::unexpected<std::vector<int>> &&>(u).error() == expectedVectorValue);
  }
}
