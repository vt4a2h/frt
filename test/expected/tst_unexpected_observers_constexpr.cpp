#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

constexpr auto makeExpectedString()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}

constexpr auto makeModifiedString()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}

constexpr auto makeExpectedVector() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }

TEST_CASE("Observers constexpr", "[unexpected][constexpr]")
{
  SECTION("error (non-const lvalue)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      return u.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error (non-const lvalue - modification)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      u.error() = makeModifiedString();
      return u.error() == makeModifiedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error (const lvalue)")
  {
    constexpr auto valid = [] {
      const frt::unexpected u{makeExpectedString()};
      return u.error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error (rvalue)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedString()};
      return std::move(u).error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error (const rvalue)")
  {
    constexpr auto valid = [] {
      const frt::unexpected u{makeExpectedString()};
      return static_cast<const frt::unexpected<std::string> &&>(u).error() == makeExpectedString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error for vector (non-const lvalue)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      return u.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error for vector (non-const lvalue - modification)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      u.error().clear();
      return u.error().empty();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error for vector (const lvalue)")
  {
    constexpr auto valid = [] {
      const frt::unexpected u{makeExpectedVector()};
      return u.error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error for vector (rvalue)")
  {
    constexpr auto valid = [] {
      frt::unexpected u{makeExpectedVector()};
      return std::move(u).error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("error for vector (const rvalue)")
  {
    constexpr auto valid = [] {
      const frt::unexpected u{makeExpectedVector()};
      return static_cast<const frt::unexpected<std::string> &&>(u).error() == makeExpectedVector();
    }();
    STATIC_REQUIRE(valid);
  }
}
