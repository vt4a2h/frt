#include <frt/hash/std_hash_expected.hpp>

#include <catch2/catch_all.hpp>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <test/helpers/types.hpp>

TEMPLATE_TEST_CASE_SIG("Hashable concept", "[expected]", ((class T, bool Hashable), T, Hashable),
                       (frt::expected<int, std::string>, true), (frt::expected<std::string, int>, true),
                       (frt::expected<test::helpers::NonHashable, std::string>, false),
                       (frt::expected<std::string, test::helpers::NonHashable>, false),
                       (frt::expected<void, std::string>, true))
{
  STATIC_REQUIRE(frt::detail::common_::IsHashable<T> == Hashable);
}

TEMPLATE_TEST_CASE("Hashable value", "[expected]", int, test::helpers::Hashable)
{
  const TestType testValue{42};
  const frt::expected<TestType, std::string> exp{testValue};

  const auto actualHash = std::hash<frt::expected<TestType, std::string>>{}(exp);
  const auto expectedHash = std::hash<TestType>{}(testValue);

  REQUIRE(actualHash == expectedHash);
}

TEMPLATE_TEST_CASE("Hashable error", "[expected]", int, test::helpers::Hashable)
{
  const TestType testValue{42};
  const frt::expected<std::string, TestType> exp{frt::unexpected{testValue}};

  const auto actualHash = std::hash<frt::expected<std::string, TestType>>{}(exp);
  const auto expectedHash = std::hash<TestType>{}(testValue);

  REQUIRE(actualHash == expectedHash);
}

TEST_CASE("Hashable void value", "[expected]")
{
  const frt::expected<void, std::string> exp{};

  const auto actualHash = std::hash<frt::expected<void, std::string>>{}(exp);
  const auto expectedHash = std::hash<frt::detail::either_::Void>{}(frt::void_val);

  REQUIRE(actualHash == expectedHash);
}

TEST_CASE("Use as a key for unordered_map value", "[expected]")
{
  using namespace std::literals;

  std::unordered_map<frt::expected<test::helpers::Hashable, int>, std::string> map;

  const std::array data = {std::pair{test::helpers::Hashable{1}, "1"s}, std::pair{test::helpers::Hashable{2}, "2"s},
                           std::pair{test::helpers::Hashable{3}, "3"s}};

  for (const auto &p : data) {
    map[frt::expected<test::helpers::Hashable, int>{p.first}] = p.second;
  }

  const auto allInserted = std::ranges::all_of(data, [&map](const auto &d) {
    return map.find(frt::expected<test::helpers::Hashable, int>{d.first}) != std::end(map);
  });

  REQUIRE(allInserted);
}

TEST_CASE("Use as a key for unordered_map error", "[expected]")
{
  using namespace std::literals;

  std::unordered_map<frt::expected<int, test::helpers::Hashable>, std::string> map;

  const std::array data = {std::pair{test::helpers::Hashable{1}, "1"s}, std::pair{test::helpers::Hashable{2}, "2"s},
                           std::pair{test::helpers::Hashable{3}, "3"s}};

  for (const auto &p : data) {
    map[frt::expected<int, test::helpers::Hashable>{frt::unexpected{p.first}}] = p.second;
  }

  const auto allInserted = std::ranges::all_of(data, [&map](const auto &d) {
    return map.find(frt::expected<int, test::helpers::Hashable>{frt::unexpected{d.first}}) != std::end(map);
  });

  REQUIRE(allInserted);
}
