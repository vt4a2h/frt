#include <frt/expected.hpp>
#include <frt/hash/expected-hash.hpp>

#include <catch2/catch_all.hpp>

#include <string>

#include <test/helpers/types.hpp>

TEST_CASE("Hashable value custom type (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const test::helpers::Hashable testValue{42};
    const frt::expected<test::helpers::Hashable, int> exp{testValue};

    const auto actualHash = std::hash<frt::expected<test::helpers::Hashable, int>>{}(exp);
    const auto expectedHash = std::hash<test::helpers::Hashable>{}(testValue);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Hashable error custom type (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const test::helpers::Hashable testValue{42};
    const frt::expected<int, test::helpers::Hashable> exp{frt::unexpected{testValue}};

    const auto actualHash = std::hash<frt::expected<int, test::helpers::Hashable>>{}(exp);
    const auto expectedHash = std::hash<test::helpers::Hashable>{}(testValue);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Hashable void value (constexpr)", "[expected][constexpr]")
{
  constexpr auto valid = [] {
    const frt::expected<void, int> exp{};

    const auto actualHash = std::hash<frt::expected<void, int>>{}(exp);
    const auto expectedHash = std::hash<frt::detail::either_::Void>{}(frt::detail::either_::void_val);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}
