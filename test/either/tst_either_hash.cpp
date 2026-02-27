#include <frt/either.hpp>

#include <catch2/catch_all.hpp>

#include <string>

namespace test
{
struct NonHashable
{
  int v{};

  friend constexpr bool operator==(const NonHashable &l, const NonHashable &r) noexcept { return l.v == r.v; }
};
} // namespace test

TEMPLATE_TEST_CASE_SIG("Hashable concept", "[either]", ((class T, bool Hashable), T, Hashable), (frt::void_t, true),
                       (frt::either<int, std::string>, true), (frt::either<std::string, int>, true),
                       (frt::either<test::NonHashable, std::string>, false),
                       (frt::either<std::string, test::NonHashable>, false),
                       (frt::either<test::NonHashable, test::NonHashable>, false),
                       (frt::either<frt::void_t, std::string>, true), (frt::either<std::string, frt::void_t>, true),
                       (frt::either<frt::void_t, frt::void_t>, true))
{
  STATIC_REQUIRE(frt::detail::either_::IsHashable<T> == Hashable);
}

// TODO: add tests for actual hashing
