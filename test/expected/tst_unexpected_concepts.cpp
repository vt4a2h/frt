#include <catch2/catch_all.hpp>

#include <concepts>
#include <string>
#include <vector>

#include <frt/expected.hpp>

#include <test/helpers/types.hpp>

namespace
{

struct NonCopyable
{
  NonCopyable() = default;
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable(NonCopyable &&) = default;
  NonCopyable &operator=(NonCopyable &&) = default;
};

struct NonMovable
{
  NonMovable() = default;
  NonMovable(const NonMovable &) = delete;
  NonMovable &operator=(const NonMovable &) = delete;
  NonMovable(NonMovable &&) = delete;
  NonMovable &operator=(NonMovable &&) = delete;
};

} // namespace

TEMPLATE_TEST_CASE_SIG("IsUnexpected", "[unexpected]", ((class T, bool Valid), T, Valid), (frt::unexpected<int>, true),
                       (const frt::unexpected<int>, true), (frt::unexpected<int> &, true),
                       (const frt::unexpected<int> &, true), (frt::unexpected<int> &&, true),
                       (const frt::unexpected<int> &&, true), (frt::unexpected<std::string>, true),
                       (const frt::unexpected<std::string> &, true), (frt::unexpected<std::vector<int>>, true),
                       (int, false), (double, false), (std::string, false), (std::vector<int>, false), (void, false),
                       (frt::in_place_unexpected_t, false), (frt::either<int, int>, false),
                       (frt::expected<int, double>, false))
{
  STATIC_REQUIRE(frt::IsUnexpected<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("is_unexpected_", "[unexpected]", ((class T, bool Valid), T, Valid),
                       (frt::unexpected<int>, true), (frt::unexpected<std::string>, true),
                       (frt::unexpected<std::vector<int>>, true), (const frt::unexpected<int>, false),
                       (frt::unexpected<int> &, false), (frt::unexpected<int> &&, false), (int, false), (double, false),
                       (std::string, false), (void, false))
{
  STATIC_REQUIRE(frt::detail::expected_::is_unexpected_<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidUnexpectedType", "[unexpected]", ((class T, bool Valid), T, Valid), (int, true),
                       (double, true), (std::string, true), (std::vector<int>, true), (test::helpers::TestArgsIl, true),
                       (void, false), (int &, false), (const int &, false), (int &&, false), (int[5], false),
                       (int[], false), (const int, false), (volatile int, false), (const volatile int, false),
                       (frt::unexpected<int>, false), (frt::unexpected<std::string>, false),
                       (const frt::unexpected<int>, false), (frt::unexpected<int> &, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ValidUnexpectedType<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidUnexpectedError", "[unexpected]", ((class Err, class E, bool Valid), Err, E, Valid),
                       (int, int, true), (const int &, int, true), (int &&, int, true),
                       (const char *, std::string, true), (std::string, std::string, true), (int, double, true),
                       (frt::unexpected<int>, int, false), (const frt::unexpected<int> &, int, false),
                       (frt::unexpected<int> &&, int, false), (frt::unexpected<std::string>, std::string, false),
                       (frt::in_place_expected_t, int, false), (const frt::in_place_expected_t &, int, false),
                       (std::string, int, false), (test::helpers::ComparableTypeFirst, int, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ValidUnexpectedError<Err, E> == Valid);
}

TEST_CASE("unexpected constructors and concepts", "[unexpected]")
{
  SECTION("copy constructible for int") { STATIC_REQUIRE(std::copy_constructible<frt::unexpected<int>>); }
  SECTION("move constructible for int") { STATIC_REQUIRE(std::move_constructible<frt::unexpected<int>>); }
  SECTION("copy assignable for int") { STATIC_REQUIRE(std::is_copy_assignable_v<frt::unexpected<int>>); }
  SECTION("move assignable for int") { STATIC_REQUIRE(std::is_move_assignable_v<frt::unexpected<int>>); }
  SECTION("swappable for int") { STATIC_REQUIRE(std::is_swappable_v<frt::unexpected<int>>); }
  SECTION("equality comparable for int") { STATIC_REQUIRE(std::equality_comparable<frt::unexpected<int>>); }
  SECTION("three way comparable for int") { STATIC_REQUIRE(std::three_way_comparable<frt::unexpected<int>>); }
  SECTION("non-copyable type is not copy constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<frt::unexpected<NonCopyable>>);
  }
  SECTION("non-copyable type is move constructible")
  {
    STATIC_REQUIRE(std::is_move_constructible_v<frt::unexpected<NonCopyable>>);
  }
  SECTION("non-movable type is not move constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_move_constructible_v<frt::unexpected<NonMovable>>);
  }
  SECTION("non-movable type is not swappable")
  {
    STATIC_REQUIRE_FALSE(std::is_swappable_v<frt::unexpected<NonMovable>>);
  }
}
