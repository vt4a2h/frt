#include <catch2/catch_all.hpp>

#include <frt/either.hpp>

TEMPLATE_TEST_CASE_SIG("Is either", "[either]", ((class T, bool Valid), T, Valid), (int, false),
                       (frt::either<int, double>, true), (const frt::either<int, double>, true),
                       (frt::either<int, double> &, true), (std::vector<int>, false))
{
  STATIC_REQUIRE(frt::detail::either_::IsEither<T> == Valid);
}
