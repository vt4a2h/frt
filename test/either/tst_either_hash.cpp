#include <frt/hash/either-hash.hpp>

#include <catch2/catch_all.hpp>

#include <string>
#include <unordered_map>

#include <test/helpers/types.hpp>

TEMPLATE_TEST_CASE_SIG("Hashable concept", "[either]", ((class T, bool Hashable), T, Hashable),
                       (frt::detail::either_::Void, true), (frt::either<int, std::string>, true),
                       (frt::either<std::string, int>, true),
                       (frt::either<test::helpers::NonHashable, std::string>, false),
                       (frt::either<std::string, test::helpers::NonHashable>, false),
                       (frt::either<test::helpers::NonHashable, test::helpers::NonHashable>, false),
                       (frt::either<void, std::string>, true), (frt::either<std::string, void>, true),
                       (frt::either<void, void>, true))
{
  STATIC_REQUIRE(frt::detail::either_::IsHashable<T> == Hashable);
}

TEMPLATE_TEST_CASE("Hashable left", "[either]", int, test::helpers::Hashable)
{
  const TestType testValue{42};
  const frt::either<TestType, std::string> either{frt::in_place_left, testValue};

  const auto actualHash = std::hash<frt::either<TestType, std::string>>{}(either);
  const auto expectedHash = std::hash<TestType>{}(testValue);

  REQUIRE(actualHash == expectedHash);
}

TEMPLATE_TEST_CASE("Hashable right", "[either]", int, test::helpers::Hashable)
{
  const TestType testValue{42};
  const frt::either<std::string, TestType> either{frt::in_place_right, testValue};

  const auto actualHash = std::hash<frt::either<std::string, TestType>>{}(either);
  const auto expectedHash = std::hash<TestType>{}(testValue);

  REQUIRE(actualHash == expectedHash);
}

TEST_CASE("Use as a key for unordered_map left", "[either]")
{
  using namespace std::literals;

  std::unordered_map<frt::either<test::helpers::Hashable, int>, std::string> map;

  const std::array data = {std::pair{test::helpers::Hashable{1}, "1"s}, std::pair{test::helpers::Hashable{2}, "2"s},
                           std::pair{test::helpers::Hashable{3}, "3"s}};

  for (const auto &p : data) {
    map[frt::either<test::helpers::Hashable, int>{frt::in_place_left, p.first}] = p.second;
  }

  const auto allInserted = std::ranges::all_of(data, [&map](const auto &d) {
    return map.find(frt::either<test::helpers::Hashable, int>{frt::in_place_left, d.first}) != std::end(map);
  });

  REQUIRE(allInserted);
}

TEST_CASE("Use as a key for unordered_map right", "[either]")
{
  using namespace std::literals;

  std::unordered_map<frt::either<int, test::helpers::Hashable>, std::string> map;

  const std::array data = {std::pair{test::helpers::Hashable{1}, "1"s}, std::pair{test::helpers::Hashable{2}, "2"s},
                           std::pair{test::helpers::Hashable{3}, "3"s}};

  for (const auto &p : data) {
    map[frt::either<int, test::helpers::Hashable>{frt::in_place_right, p.first}] = p.second;
  }

  const auto allInserted = std::ranges::all_of(data, [&map](const auto &d) {
    return map.find(frt::either<int, test::helpers::Hashable>{frt::in_place_right, d.first}) != std::end(map);
  });

  REQUIRE(allInserted);
}
