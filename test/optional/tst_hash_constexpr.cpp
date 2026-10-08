#include <frt/hash/std_hash_optional.hpp>
#include <frt/optional.hpp>

#include <catch2/catch_all.hpp>

#include <string>

#include <test/helpers/types.hpp>

TEST_CASE("Hashable value custom type (constexpr)", "[optional][constexpr]")
{
  constexpr auto valid = [] {
    const test::helpers::Hashable testValue{42};
    const frt::optional<test::helpers::Hashable> opt{testValue};

    const auto actualHash = std::hash<frt::optional<test::helpers::Hashable>>{}(opt);
    const auto expectedHash = std::hash<test::helpers::Hashable>{}(testValue);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}

TEST_CASE("Hashable nullopt (constexpr)", "[optional][constexpr]")
{
  constexpr auto valid = [] {
    const frt::optional<test::helpers::Hashable> opt{};

    const auto actualHash = std::hash<frt::optional<test::helpers::Hashable>>{}(opt);
    const auto expectedHash = std::hash<frt::detail::either_::Void>{}(frt::void_val);

    return actualHash == expectedHash;
  }();

  STATIC_REQUIRE(valid);
}
