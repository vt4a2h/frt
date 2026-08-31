#include <frt/hash/optional-hash.hpp>

#include <catch2/catch_all.hpp>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <test/helpers/types.hpp>

TEMPLATE_TEST_CASE_SIG("Hashable concept", "[optional]", ((class T, bool Hashable), T, Hashable),
                       (frt::optional<int>, true), (frt::optional<std::string>, true),
                       (frt::optional<test::helpers::NonHashable>, false))
{
  STATIC_REQUIRE(frt::detail::common_::IsHashable<T> == Hashable);
}

TEMPLATE_TEST_CASE("Hashable value", "[optional]", int, test::helpers::Hashable)
{
  const TestType testValue{42};
  const frt::optional<TestType> opt{testValue};

  const auto actualHash = std::hash<frt::optional<TestType>>{}(opt);
  const auto expectedHash = std::hash<TestType>{}(testValue);

  REQUIRE(actualHash == expectedHash);
}

TEST_CASE("Hashable nullopt", "[optional]")
{
  const frt::optional<int> opt{};

  const auto actualHash = std::hash<frt::optional<int>>{}(opt);
  const auto expectedHash = std::hash<frt::detail::either_::Void>{}(frt::detail::either_::void_val);

  REQUIRE(actualHash == expectedHash);
}

TEST_CASE("Use as a key for unordered_map value", "[optional]")
{
  using namespace std::literals;

  std::unordered_map<frt::optional<test::helpers::Hashable>, std::string> map;

  const std::array data = {std::pair{test::helpers::Hashable{1}, "1"s}, std::pair{test::helpers::Hashable{2}, "2"s},
                           std::pair{test::helpers::Hashable{3}, "3"s}};

  for (const auto &p : data) {
    map[frt::optional<test::helpers::Hashable>{p.first}] = p.second;
  }

  const auto allInserted = std::ranges::all_of(data, [&map](const auto &d) {
    return map.find(frt::optional<test::helpers::Hashable>{d.first}) != std::end(map);
  });

  REQUIRE(allInserted);
}

TEST_CASE("Use as a key for unordered_map nullopt", "[optional]")
{
  using namespace std::literals;

  std::unordered_map<frt::optional<test::helpers::Hashable>, std::string> map;

  map[frt::optional<test::helpers::Hashable>{}] = "nullopt"s;

  REQUIRE(map.find(frt::optional<test::helpers::Hashable>{}) != std::end(map));
}
