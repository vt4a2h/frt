#include <catch2/catch_all.hpp>

#include <array>
#include <string>
#include <string_view>
#include <utility>

#include <frt/optional.hpp>

#include <test/helpers/types.hpp>

TEST_CASE("Create constexpr", "[optional][constexpr]")
{
  static constexpr std::string_view lorem_ipsum =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed feugiat lacus ultricies dictum porttitor.";

  SECTION("By default")
  {
    constexpr frt::optional<int> opt;
    STATIC_REQUIRE_FALSE(opt.has_value());
  }

  SECTION("With nullopt_t")
  {
    constexpr frt::optional<int> opt{frt::nullopt};
    STATIC_REQUIRE_FALSE(opt.has_value());
  }

  SECTION("Assign nullopt_t")
  {
    constexpr auto valid = [] {
      frt::optional<int> opt{42};
      opt = frt::nullopt;
      return !opt.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Assign nullopt_t to empty optional")
  {
    constexpr auto valid = [] {
      frt::optional<int> opt;
      opt = frt::nullopt;
      return !opt.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("With value (by copy)")
  {
    constexpr int expectedResult{42};

    constexpr frt::optional opt{expectedResult};

    constexpr auto actualResult = opt.value();

    STATIC_REQUIRE(actualResult == expectedResult);
  }

  SECTION("With value assigment (by copy)")
  {
    constexpr int expectedResult = 42;

    constexpr auto actualResult = [expectedResult = expectedResult]() constexpr {
      frt::optional<int> opt;

      opt = expectedResult;

      return opt.value();
    }();

    STATIC_REQUIRE(actualResult == expectedResult);
  }

  SECTION("Implicit arg conversion")
  {
    constexpr bool checked = [] {
      double expectedValue = 42;

      const auto testFunc = [](frt::optional<double> opt) { return opt.value(); };

      return testFunc(expectedValue) == expectedValue;
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("With value (by move)")
  {
    constexpr std::array expectedResult{'4', '2'};

    constexpr auto actualResult = [expectedResult = expectedResult]() constexpr {
      auto tmp = expectedResult;
      const frt::optional opt{std::move(tmp)};

      return opt.value();
    }();

    STATIC_REQUIRE(actualResult == expectedResult);
  }

  SECTION("With value assigment (by move)")
  {
    constexpr std::array expectedResult{'4', '2'};

    constexpr auto actualResult = [expectedResult = expectedResult]() constexpr {
      frt::optional<std::array<char, 2>> opt;

      auto tmp = expectedResult;
      opt = std::move(tmp);

      return opt.value();
    }();

    STATIC_REQUIRE(actualResult == expectedResult);
  }

  SECTION("Copy ctor")
  {
    constexpr bool checked = [] {
      const frt::optional<std::string> opt{std::string{lorem_ipsum}};
      const frt::optional opt2(opt);
      return opt == opt2;
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("Move ctor")
  {
    constexpr bool checked = [] {
      frt::optional<std::string> opt{std::string{lorem_ipsum}};
      const frt::optional opt2(std::move(opt));
      return opt2.has_value();
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("Copy operator")
  {
    constexpr auto copied = []() constexpr {
      const frt::optional<std::string> opt{std::string{lorem_ipsum}};
      frt::optional<std::string> opt2;

      opt2 = opt;

      return opt2 == opt;
    }();

    STATIC_REQUIRE(copied);
  }

  SECTION("Move operator")
  {
    constexpr auto correctValue = []() constexpr {
      const std::string expected{lorem_ipsum};

      frt::optional<std::string> opt{expected};
      frt::optional<std::string> opt2;

      opt2 = std::move(opt);

      return opt2 == expected;
    }();

    STATIC_REQUIRE(correctValue);
  }

  SECTION("Other optional by copy")
  {
    constexpr bool checked = [] {
      const int expected = 42;

      const frt::optional intOpt{expected};
      const frt::optional<double> doubleOpt{intOpt};

      return doubleOpt == expected;
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("Other optional by copy assigment")
  {
    constexpr auto correctValue = []() constexpr {
      constexpr int expected = 42;

      frt::optional intOpt{expected};
      frt::optional<double> doubleOpt;

      doubleOpt = intOpt;

      return doubleOpt == expected;
    }();

    STATIC_REQUIRE(correctValue);
  }

  SECTION("Other optional by copy assigment (nullopt)")
  {
    constexpr auto hasValue = []() constexpr {
      frt::optional<int> intOpt;
      frt::optional<double> doubleOpt{42};

      doubleOpt = intOpt;

      return doubleOpt.has_value();
    }();

    STATIC_REQUIRE_FALSE(hasValue);
  }

  SECTION("Other optional (bool) by copy")
  {
    constexpr bool checked = [] {
      const frt::optional opt_zero{0};
      const frt::optional<bool> from_int{opt_zero};

      return !from_int.value();
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("Other optional (bool) by copy assigment")
  {
    constexpr auto fromIntVal = []() constexpr {
      const frt::optional opt_zero{0};
      frt::optional<bool> from_int;

      from_int = opt_zero;

      return from_int.value();
    }();

    STATIC_REQUIRE_FALSE(fromIntVal);
  }

  SECTION("Other optional by move")
  {
    constexpr bool checked = [] {
      const int expected = 42;

      frt::optional intOpt{expected};
      const frt::optional<double> doubleOpt{std::move(intOpt)};

      return doubleOpt == expected;
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("Other optional by move assigment")
  {
    constexpr auto correctValue = [] {
      constexpr int expected = 42;

      frt::optional intOpt{expected};
      frt::optional<double> doubleOpt;

      doubleOpt = std::move(intOpt);

      return doubleOpt == expected;
    }();

    STATIC_REQUIRE(correctValue);
  }

  SECTION("Other optional by move assigment (nullopt)")
  {
    constexpr auto hasValue = [] {
      frt::optional<int> intOpt;
      frt::optional<double> doubleOpt{42};

      doubleOpt = std::move(intOpt);

      return doubleOpt.has_value();
    }();

    STATIC_REQUIRE_FALSE(hasValue);
  }

  SECTION("Other optional (bool) by move")
  {
    constexpr bool checked = [] {
      frt::optional opt_zero{0};
      const frt::optional<bool> from_int{std::move(opt_zero)};

      return !from_int.value();
    }();

    STATIC_REQUIRE(checked);
  }

  SECTION("Other optional (bool) by assigment")
  {
    constexpr auto fromIntVal = [] {
      frt::optional opt_zero{0};
      frt::optional<bool> from_int;

      from_int = std::move(opt_zero);

      return from_int.value();
    }();

    STATIC_REQUIRE_FALSE(fromIntVal);
  }

  SECTION("Inplace construction")
  {
    constexpr auto opt = frt::optional<std::string_view>{frt::in_place_opt, lorem_ipsum};

    constexpr auto actual = opt.value();
    constexpr auto expected{lorem_ipsum};

    STATIC_REQUIRE(actual == expected);
  }

  SECTION("Inplace construction (multiple args)")
  {
    constexpr auto valid = [] {
      const auto opt = frt::optional<test::helpers::TestArgsIl>{frt::in_place_opt, 42, 3.14};
      const auto expected = test::helpers::TestArgsIl{42, 3.14};
      return opt.value() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Inplace construction from initializer list")
  {
    constexpr auto valid = [] {
      const auto some = {'L', 'o', 'r', 'e', 'm', ' ', 'i', 'p', 's', 'u', 'm', ' ', 'd', 'o', 'l', 'o', 'r', ' ', 's',
                         'i', 't', ' ', 'a', 'm', 'e', 't', ',', ' ', 'c', 'o', 'n', 's', 'e', 'c', 't', 'e', 't', 'u',
                         'r', ' ', 'a', 'd', 'i', 'p', 'i', 's', 'c', 'i', 'n', 'g', ' ', 'e', 'l', 'i', 't', '.'};

      const auto opt = frt::optional<std::string>{frt::in_place_opt, some};
      const std::string expected{some};

      return opt.value() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("Inplace construction from initializer list with extra args")
  {
    constexpr auto valid = [] {
      const auto il = {std::string{"Lorem ipsum dolor sit amet"}, std::string{"consectetur adipiscing elit"}};
      const auto opt = frt::optional<test::helpers::TestArgsIl>{frt::in_place_opt, il, 42, 3.14};

      const auto expected = test::helpers::TestArgsIl{il, 42, 3.14};

      return opt.value() == expected;
    }();

    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (int)")
  {
    constexpr auto valid = [] {
      const frt::optional opt{42};
      return opt.value() == 42;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (string)")
  {
    constexpr auto valid = [] {
      const frt::optional opt{std::string{lorem_ipsum}};
      return opt.value() == lorem_ipsum;
    }();
    STATIC_REQUIRE(valid);
  }
}
