#include <catch2/catch_all.hpp>

#include <string>
#include <string_view>
#include <utility>

#include <frt/optional.hpp>

#include <test/helpers/types.hpp>

TEST_CASE("Create", "[optional]")
{
  static constexpr std::string_view lorem_ipsum =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua";

  SECTION("By default")
  {
    const frt::optional<int> opt;
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("With nullopt_t")
  {
    const frt::optional<int> opt{frt::nullopt};
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("Assign nullopt_t")
  {
    frt::optional<int> opt{42};
    opt = frt::nullopt;
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("Assign nullopt_t to empty optional")
  {
    frt::optional<int> opt;
    opt = frt::nullopt;
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("With value (by copy)")
  {
    const int expectedResult = 42;

    const frt::optional opt{expectedResult};

    const auto &actualResult = opt.value();

    REQUIRE(actualResult == expectedResult);
  }

  SECTION("With value assigment (by copy)")
  {
    const int expectedResult = 42;

    frt::optional<int> opt;

    opt = expectedResult;

    const auto &actualResult = opt.value();

    REQUIRE(actualResult == expectedResult);
  }

  SECTION("Implicit arg conversion")
  {
    const double expectedValue = 42;

    const auto testFunc = [](frt::optional<double> opt) { return opt.value(); };

    REQUIRE(testFunc(expectedValue) == expectedValue);
  }

  SECTION("With value (by move)")
  {
    const std::string expectedResult{lorem_ipsum};

    auto tmp = expectedResult;
    const frt::optional opt{std::move(tmp)};

    const auto &actualResult = opt.value();

    REQUIRE(actualResult == expectedResult);
  }

  SECTION("With value assigment (by move)")
  {
    const std::string expectedResult{lorem_ipsum};

    frt::optional<std::string> opt;

    auto tmp = expectedResult;
    opt = std::move(tmp);

    const auto &actualResult = opt.value();

    REQUIRE(actualResult == expectedResult);
  }

  SECTION("Copy ctor")
  {
    const frt::optional opt{std::string{lorem_ipsum}};
    const frt::optional opt2(opt);

    REQUIRE(opt == opt2);
  }

  SECTION("Move ctor")
  {
    frt::optional<std::string> opt;
    const frt::optional opt2(std::move(opt));

    REQUIRE_FALSE(opt2.has_value());
  }

  SECTION("Copy operator")
  {
    const frt::optional opt{std::string{lorem_ipsum}};
    frt::optional<std::string> opt2;

    opt2 = opt;

    REQUIRE(opt2 == opt);
  }

  SECTION("Move operator")
  {
    frt::optional<std::string> opt;
    frt::optional<std::string> opt2;

    opt2 = std::move(opt);

    REQUIRE_FALSE(opt2.has_value());
  }

  SECTION("Other optional by copy")
  {
    const std::string expected{lorem_ipsum};

    const frt::optional sv{std::string_view{expected}};
    const frt::optional<std::string> s{sv};

    REQUIRE(s == expected);
  }

  SECTION("Other optional by copy assigment")
  {
    const std::string expected{lorem_ipsum};

    const frt::optional sv{std::string_view{expected}};
    frt::optional<std::string> s;

    s = sv;

    REQUIRE(s == expected);
  }

  SECTION("Other optional by copy assigment (nullopt)")
  {
    const frt::optional<std::string_view> sv;
    frt::optional<std::string> s;

    s = sv;

    REQUIRE_FALSE(s.has_value());
  }

  SECTION("Other optional (bool) by copy")
  {
    const frt::optional opt_zero{0};
    const frt::optional<bool> from_int{opt_zero};

    REQUIRE_FALSE(from_int.value());
  }

  SECTION("Other optional (bool) by copy assigment")
  {
    const frt::optional opt_zero{0};
    frt::optional<bool> from_int;

    from_int = opt_zero;

    REQUIRE_FALSE(from_int.value());
  }

  SECTION("Other optional by move")
  {
    const std::string expected{lorem_ipsum};

    frt::optional sv{lorem_ipsum};
    const frt::optional<std::string> s{std::move(sv)};

    REQUIRE(s == expected);
  }

  SECTION("Other optional by move assigment")
  {
    const std::string expected{lorem_ipsum};

    frt::optional sv{lorem_ipsum};
    frt::optional<std::string> s;

    s = std::move(sv);

    REQUIRE(s == expected);
  }

  SECTION("Other optional by move assigment (nullopt)")
  {
    frt::optional<std::string_view> sv;
    frt::optional<std::string> s;

    s = std::move(sv);

    REQUIRE_FALSE(s.has_value());
  }

  SECTION("Other optional (bool) by move")
  {
    frt::optional opt_zero{0};
    const frt::optional<bool> from_int{std::move(opt_zero)};

    REQUIRE_FALSE(from_int.value());
  }

  SECTION("Other optional (bool) by assigment")
  {
    frt::optional opt_zero{0};
    frt::optional<bool> from_int;

    from_int = std::move(opt_zero);

    REQUIRE_FALSE(from_int.value());
  }

  SECTION("Inplace construction")
  {
    const auto some = "some";
    const auto opt = frt::optional<std::string>{frt::in_place_opt, some};

    const auto &actual = opt.value();
    const std::string expected{some};

    REQUIRE(actual == expected);
  }

  SECTION("Inplace construction (multiple args)")
  {
    const auto opt = frt::optional<test::helpers::TestArgsIl>{frt::in_place_opt, 42, 3.14};

    const auto expected = test::helpers::TestArgsIl{42, 3.14};

    REQUIRE(opt.value() == expected);
  }

  SECTION("Inplace construction from initializer list")
  {
    const auto some = {'L', 'o', 'r', 'e', 'm', ' ', 'i', 'p', 's', 'u', 'm', ' ', 'd', 'o', 'l', 'o', 'r', ' ', 's',
                       'i', 't', ' ', 'a', 'm', 'e', 't', ',', ' ', 'c', 'o', 'n', 's', 'e', 'c', 't', 'e', 't', 'u',
                       'r', ' ', 'a', 'd', 'i', 'p', 'i', 's', 'c', 'i', 'n', 'g', ' ', 'e', 'l', 'i', 't', '.'};

    const auto opt = frt::optional<std::string>{frt::in_place_opt, some};

    const auto &actual = opt.value();
    const std::string expected{some};

    REQUIRE(actual == expected);
  }

  SECTION("Inplace construction from initializer list with extra args")
  {
    const auto il = {std::string{"Lorem ipsum dolor sit amet"}, std::string{"consectetur adipiscing elit"}};
    const auto opt = frt::optional<test::helpers::TestArgsIl>{frt::in_place_opt, il, 42, 3.14};

    const auto expected = test::helpers::TestArgsIl{il, 42, 3.14};

    REQUIRE(opt.value() == expected);
  }

  SECTION("CTAD (int)")
  {
    const frt::optional opt{42};
    REQUIRE(opt.value() == 42);
  }

  SECTION("CTAD (string)")
  {
    const frt::optional opt{std::string{lorem_ipsum}};
    REQUIRE(opt.value() == lorem_ipsum);
  }

  SECTION("From either by copy (value)")
  {
    const std::string expected{lorem_ipsum};
    const frt::either<void, std::string> eitherVal{frt::in_place_right, expected};
    const frt::optional<std::string> opt{eitherVal};

    REQUIRE(opt == expected);
  }

  SECTION("From either by copy (nullopt)")
  {
    const frt::either<void, std::string> eitherVal{frt::in_place_left};
    const frt::optional<std::string> opt{eitherVal};

    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("From either by copy assignment (value to empty)")
  {
    const std::string expected{lorem_ipsum};
    const frt::either<void, std::string> eitherVal{frt::in_place_right, expected};
    frt::optional<std::string> opt;
    opt = eitherVal;

    REQUIRE(opt == expected);
  }

  SECTION("From either by copy assignment (value to value)")
  {
    const std::string expected{lorem_ipsum};
    const frt::either<void, std::string> eitherVal{frt::in_place_right, expected};
    frt::optional<std::string> opt{std::string{"Sed ut perspiciatis unde omnis iste natus error sit voluptatem"}};
    opt = eitherVal;

    REQUIRE(opt == expected);
  }

  SECTION("From either by copy assignment (nullopt to empty)")
  {
    const frt::either<void, std::string> eitherVal{frt::in_place_left};
    frt::optional<std::string> opt;
    opt = eitherVal;

    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("From either by copy assignment (nullopt to value)")
  {
    const std::string expected{lorem_ipsum};
    const frt::either<void, std::string> eitherVal{frt::in_place_left};
    frt::optional<std::string> opt{expected};
    opt = eitherVal;

    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("From either by move (value)")
  {
    const std::string expected{lorem_ipsum};
    frt::either<void, std::string> eitherVal{frt::in_place_right, expected};
    const frt::optional<std::string> opt{std::move(eitherVal)};

    REQUIRE(opt == expected);
  }

  SECTION("From either by move (nullopt)")
  {
    frt::either<void, std::string> eitherVal{frt::in_place_left};
    const frt::optional<std::string> opt{std::move(eitherVal)};

    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("From either by move assignment (value to empty)")
  {
    const std::string expected{lorem_ipsum};
    frt::either<void, std::string> eitherVal{frt::in_place_right, expected};
    frt::optional<std::string> opt;
    opt = std::move(eitherVal);

    REQUIRE(opt == expected);
  }

  SECTION("From either by move assignment (value to value)")
  {
    const std::string expected{lorem_ipsum};
    frt::either<void, std::string> eitherVal{frt::in_place_right, expected};
    frt::optional<std::string> opt{std::string{"Sed ut perspiciatis unde omnis iste natus error sit voluptatem"}};
    opt = std::move(eitherVal);

    REQUIRE(opt == expected);
  }

  SECTION("From either by move assignment (nullopt to empty)")
  {
    frt::either<void, std::string> eitherVal{frt::in_place_left};
    frt::optional<std::string> opt;
    opt = std::move(eitherVal);

    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("From either by move assignment (nullopt to value)")
  {
    const std::string expected{lorem_ipsum};
    frt::either<void, std::string> eitherVal{frt::in_place_left};
    frt::optional<std::string> opt{expected};
    opt = std::move(eitherVal);

    REQUIRE_FALSE(opt.has_value());
  }
}
