#include <frt/either.hpp>
#include <frt/hash/either-hash.hpp>

#include <catch2/catch_all.hpp>

#include <string>

#include <test/helpers/types.hpp>

TEST_CASE("Hashable left custom type (constexpr)", "[either][constexpr]")
{
  constexpr auto valid = [] {
    const test::helpers::Hashable testValue{42};
    const frt::either<test::helpers::Hashable, int> either{frt::in_place_left, testValue};

    const auto actualHash = std::hash<frt::either<test::helpers::Hashable, int>>{}(either);
    const auto expectedHash = std::hash<test::helpers::Hashable>{}(testValue);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Hashable right custom type (constexpr)", "[either][constexpr]")
{
  constexpr auto valid = [] {
    const test::helpers::Hashable testValue{42};
    const frt::either<int, test::helpers::Hashable> either{frt::in_place_right, testValue};

    const auto actualHash = std::hash<frt::either<int, test::helpers::Hashable>>{}(either);
    const auto expectedHash = std::hash<test::helpers::Hashable>{}(testValue);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}

// TEST_CASE("Hashable void_t (constexpr)", "[either][constexpr]")
// {
//   constexpr auto valid = [] {
//     const frt::either<void, int> either{frt::in_place_left, };
//
//     const auto actualHash = std::hash<frt::either<frt::void_t, int>>{}(either);
//     const auto expectedHash = std::hash<frt::void_t>{}(frt::void_val);
//
//     return actualHash == expectedHash;
//   }();
//
//   STATIC_REQUIRE(valid);
// }
