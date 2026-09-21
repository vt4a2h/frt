#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

using Expected = frt::expected<std::vector<int>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

constexpr auto makeExpectedValue() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeExpectedError()
{
  return std::string{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
                     "incididunt ut labore et dolore magna aliqua"};
}

TEST_CASE("Observers (has_value / bool) constexpr", "[expected][constexpr]")
{
  SECTION("has_value")
  {
    SECTION("returns true for value")
    {
      constexpr auto valid = [] {
        const Expected valueExpected{makeExpectedValue()};
        return valueExpected.has_value();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for error")
    {
      constexpr auto valid = [] {
        const Expected errorExpected{frt::unexpected{makeExpectedError()}};
        return !errorExpected.has_value();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns true for void value")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidValueExpected;
        return voidValueExpected.has_value();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for void error")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidErrorExpected{frt::unexpected{makeExpectedError()}};
        return !voidErrorExpected.has_value();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("operator bool")
  {
    SECTION("returns true for value")
    {
      constexpr auto valid = [] {
        const Expected valueExpected{makeExpectedValue()};
        return static_cast<bool>(valueExpected);
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for error")
    {
      constexpr auto valid = [] {
        const Expected errorExpected{frt::unexpected{makeExpectedError()}};
        return !static_cast<bool>(errorExpected);
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns true for void value")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidValueExpected;
        return static_cast<bool>(voidValueExpected);
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns false for void error")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidErrorExpected{frt::unexpected{makeExpectedError()}};
        return !static_cast<bool>(voidErrorExpected);
      }();
      STATIC_REQUIRE(valid);
    }
  }
}

TEST_CASE("Observers (value / operator* / operator->) constexpr", "[expected][constexpr]")
{
  SECTION("value")
  {
    SECTION("returns reference to value (const)")
    {
      constexpr auto valid = [] {
        const Expected expected{makeExpectedValue()};
        return expected.value() == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to value (non-const)")
    {
      constexpr auto valid = [] {
        Expected expected{makeExpectedValue()};
        return expected.value() == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          Expected expected{makeExpectedValue()};
          auto &val = expected.value();
          val.clear();
          return expected.value().empty();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to value (rvalue)")
    {
      constexpr auto valid = [] {
        Expected expected{makeExpectedValue()};
        return std::move(expected).value() == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to value (const rvalue)")
    {
      constexpr auto valid = [] {
        const Expected expected{makeExpectedValue()};
        return static_cast<const Expected &&>(expected).value() == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("operator*")
  {
    SECTION("returns reference to value (const)")
    {
      constexpr auto valid = [] {
        const Expected expected{makeExpectedValue()};
        return *expected == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to value (non-const)")
    {
      constexpr auto valid = [] {
        Expected expected{makeExpectedValue()};
        return *expected == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          Expected expected{makeExpectedValue()};
          auto &val = *expected;
          val.clear();
          return (*expected).empty();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to value (rvalue)")
    {
      constexpr auto valid = [] {
        Expected expected{makeExpectedValue()};
        return *std::move(expected) == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to value (const rvalue)")
    {
      constexpr auto valid = [] {
        const Expected expected{makeExpectedValue()};
        return *static_cast<const Expected &&>(expected) == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("invocable for void value (const)")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidExpected;

        [[maybe_unused]] auto v = *voidExpected;
        return voidExpected.has_value();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("operator->")
  {
    SECTION("returns pointer to value (const)")
    {
      constexpr auto valid = [] {
        const Expected expected{makeExpectedValue()};
        return expected->size() == makeExpectedValue().size();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns pointer to value (non-const)")
    {
      constexpr auto valid = [] {
        Expected expected{makeExpectedValue()};
        return expected->size() == makeExpectedValue().size();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [] {
          Expected expected{makeExpectedValue()};
          expected->clear();
          return expected->empty();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }
  }
}

TEST_CASE("Observers (error) constexpr", "[expected][constexpr]")
{
  constexpr auto makeModifiedError = [] {
    return std::string{
        "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
        "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
  };

  SECTION("error")
  {
    SECTION("returns reference to error (const)")
    {
      constexpr auto valid = [] {
        const Expected expected{frt::unexpected{makeExpectedError()}};
        return expected.error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to error for void value (const)")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidExpected{frt::unexpected{makeExpectedError()}};
        return voidExpected.error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to error (non-const)")
    {
      constexpr auto valid = [] {
        Expected expected{frt::unexpected{makeExpectedError()}};
        return expected.error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [makeModifiedError] {
          Expected expected{frt::unexpected{makeExpectedError()}};
          auto &err = expected.error();
          err = makeModifiedError();
          return expected.error() == makeModifiedError();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to error for void value (non-const)")
    {
      constexpr auto valid = [] {
        ExpectedVoid voidExpected{frt::unexpected{makeExpectedError()}};
        return voidExpected.error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);

      SECTION("allows modification")
      {
        constexpr auto modifiedValid = [makeModifiedError] {
          ExpectedVoid voidExpected{frt::unexpected{makeExpectedError()}};
          auto &err = voidExpected.error();
          err = makeModifiedError();
          return voidExpected.error() == makeModifiedError();
        }();
        STATIC_REQUIRE(modifiedValid);
      }
    }

    SECTION("returns reference to error (rvalue)")
    {
      constexpr auto valid = [] {
        Expected expected{frt::unexpected{makeExpectedError()}};
        return std::move(expected).error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to error for void value (rvalue)")
    {
      constexpr auto valid = [] {
        ExpectedVoid voidExpected{frt::unexpected{makeExpectedError()}};
        return std::move(voidExpected).error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to error (const rvalue)")
    {
      constexpr auto valid = [] {
        const Expected expected{frt::unexpected{makeExpectedError()}};
        return static_cast<const Expected &&>(expected).error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns reference to error for void value (const rvalue)")
    {
      constexpr auto valid = [] {
        const ExpectedVoid voidExpected{frt::unexpected{makeExpectedError()}};
        return static_cast<const ExpectedVoid &&>(voidExpected).error() == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }
  }
}

TEST_CASE("Observers (*_or) constexpr", "[expected][constexpr]")
{
  SECTION("value_or")
  {
    constexpr auto makeDefaultValue = [] { return std::vector{10, 11, 12, 13, 14}; };

    SECTION("returns value if present (const &)")
    {
      constexpr auto valid = [makeDefaultValue] {
        const Expected expected{makeExpectedValue()};
        return expected.value_or(makeDefaultValue()) == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if error present (const &)")
    {
      constexpr auto valid = [makeDefaultValue] {
        const Expected expected{frt::unexpected{makeExpectedError()}};
        return expected.value_or(makeDefaultValue()) == makeDefaultValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns value if present (&&)")
    {
      constexpr auto valid = [makeDefaultValue] {
        Expected expected{makeExpectedValue()};
        return std::move(expected).value_or(makeDefaultValue()) == makeExpectedValue();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if error present (&&)")
    {
      constexpr auto valid = [makeDefaultValue] {
        Expected expected{frt::unexpected{makeExpectedError()}};
        return std::move(expected).value_or(makeDefaultValue()) == makeDefaultValue();
      }();
      STATIC_REQUIRE(valid);
    }
  }

  SECTION("error_or")
  {
    constexpr auto makeDefaultError = [] {
      return std::string{"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
                         "commodo consequat."};
    };

    SECTION("returns error if present (const &)")
    {
      constexpr auto valid = [makeDefaultError] {
        const Expected expected{frt::unexpected{makeExpectedError()}};
        return expected.error_or(makeDefaultError()) == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns error for void value if present (const &)")
    {
      constexpr auto valid = [makeDefaultError] {
        const ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
        return expected.error_or(makeDefaultError()) == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if value present (const &)")
    {
      constexpr auto valid = [makeDefaultError] {
        const Expected expected{makeExpectedValue()};
        return expected.error_or(makeDefaultError()) == makeDefaultError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value for void value if value present (const &)")
    {
      constexpr auto valid = [makeDefaultError] {
        const ExpectedVoid expected;
        return expected.error_or(makeDefaultError()) == makeDefaultError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns error if present (&&)")
    {
      constexpr auto valid = [makeDefaultError] {
        Expected expected{frt::unexpected{makeExpectedError()}};
        return std::move(expected).error_or(makeDefaultError()) == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns error for void value if present (&&)")
    {
      constexpr auto valid = [makeDefaultError] {
        ExpectedVoid expected{frt::unexpected{makeExpectedError()}};
        return std::move(expected).error_or(makeDefaultError()) == makeExpectedError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value if value present (&&)")
    {
      constexpr auto valid = [makeDefaultError] {
        Expected expected{makeExpectedValue()};
        return std::move(expected).error_or(makeDefaultError()) == makeDefaultError();
      }();
      STATIC_REQUIRE(valid);
    }

    SECTION("returns default value for void value if value present (&&)")
    {
      constexpr auto valid = [makeDefaultError] {
        ExpectedVoid expected;
        return std::move(expected).error_or(makeDefaultError()) == makeDefaultError();
      }();
      STATIC_REQUIRE(valid);
    }
  }
}
