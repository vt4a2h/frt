#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/expected.hpp>

using Expected = frt::expected<std::vector<int>, std::string>;
using ExpectedVoid = frt::expected<void, std::string>;

inline static const std::vector expectedValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
inline static const std::string expectedError =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
    "incididunt ut labore et dolore magna aliqua";

template <class T>
concept ArrowInvocable = requires(T &&t) { t.operator->(); };

template <class T>
concept DerefInvocable = requires(T &&t) { *t; };

template <class T>
concept ValueInvocable = requires(T &&t) { t.value(); };

template <class T>
concept ValueOrInvocable = requires(T &&t) { t.value_or(std::vector<int>{}); };

template <class T>
concept ErrorInvocable = requires(T &&t) { t.error(); };

template <class T>
concept ErrorOrInvocable = requires(T &&t) { t.error_or(std::string{}); };

TEST_CASE("Observers (has_value / bool)", "[expected]")
{
  const Expected valueExpected{expectedValue};
  const Expected errorExpected{frt::unexpected{expectedError}};
  const ExpectedVoid voidValueExpected;
  const ExpectedVoid voidErrorExpected{frt::unexpected{expectedError}};

  SECTION("has_value")
  {
    SECTION("returns true for value") { REQUIRE(valueExpected.has_value()); }
    SECTION("returns false for error") { REQUIRE_FALSE(errorExpected.has_value()); }
    SECTION("returns true for void value") { REQUIRE(voidValueExpected.has_value()); }
    SECTION("returns false for void error") { REQUIRE_FALSE(voidErrorExpected.has_value()); }
  }

  SECTION("operator bool")
  {
    SECTION("returns true for value") { REQUIRE(static_cast<bool>(valueExpected)); }
    SECTION("returns false for error") { REQUIRE_FALSE(static_cast<bool>(errorExpected)); }
    SECTION("returns true for void value") { REQUIRE(static_cast<bool>(voidValueExpected)); }
    SECTION("returns false for void error") { REQUIRE_FALSE(static_cast<bool>(voidErrorExpected)); }
  }
}

TEST_CASE("Observers (value / operator* / operator->)", "[expected]")
{
  SECTION("value")
  {
    Expected expected{expectedValue};

    SECTION("returns reference to value (const)")
    {
      const auto &constExpected = expected;
      REQUIRE(constExpected.value() == expectedValue);
    }

    SECTION("returns reference to value (non-const)")
    {
      REQUIRE(expected.value() == expectedValue);

      SECTION("allows modification")
      {
        auto &val = expected.value();
        val.clear();
        REQUIRE(expected.value().empty());
      }
    }

    SECTION("returns reference to value (rvalue)") { REQUIRE(std::move(expected).value() == expectedValue); }

    SECTION("returns reference to value (const rvalue)")
    {
      const auto &constExpected = expected;
      REQUIRE(static_cast<const Expected &&>(constExpected).value() == expectedValue);
    }

    SECTION("invocability for void")
    {
      SECTION("non-invocable for ExpectedVoid &") { STATIC_REQUIRE_FALSE(ValueInvocable<ExpectedVoid &>); }
      SECTION("non-invocable for const ExpectedVoid &") { STATIC_REQUIRE_FALSE(ValueInvocable<const ExpectedVoid &>); }
      SECTION("non-invocable for ExpectedVoid &&") { STATIC_REQUIRE_FALSE(ValueInvocable<ExpectedVoid &&>); }
      SECTION("non-invocable for const ExpectedVoid &&")
      {
        STATIC_REQUIRE_FALSE(ValueInvocable<const ExpectedVoid &&>);
      }
    }
  }

  SECTION("operator*")
  {
    Expected expected{expectedValue};

    SECTION("returns reference to value (const)")
    {
      const auto &constExpected = expected;
      REQUIRE(*constExpected == expectedValue);
    }

    SECTION("returns reference to value (non-const)")
    {
      REQUIRE(*expected == expectedValue);

      SECTION("allows modification")
      {
        auto &val = *expected;
        val.clear();
        REQUIRE((*expected).empty());
      }
    }

    SECTION("returns reference to value (rvalue)") { REQUIRE(*std::move(expected) == expectedValue); }

    SECTION("returns reference to value (const rvalue)")
    {
      const auto &constExpected = expected;
      REQUIRE(*static_cast<const Expected &&>(constExpected) == expectedValue);
    }

    SECTION("invocable for void value (const)")
    {
      const ExpectedVoid voidExpected;
      *voidExpected;
      REQUIRE(voidExpected.has_value());
    }

    SECTION("invocability for void")
    {
      SECTION("invocable for ExpectedVoid &") { STATIC_REQUIRE(DerefInvocable<ExpectedVoid &>); }
      SECTION("invocable for const ExpectedVoid &") { STATIC_REQUIRE(DerefInvocable<const ExpectedVoid &>); }
      SECTION("invocable for ExpectedVoid &&") { STATIC_REQUIRE(DerefInvocable<ExpectedVoid &&>); }
      SECTION("invocable for const ExpectedVoid &&") { STATIC_REQUIRE(DerefInvocable<const ExpectedVoid &&>); }
    }
  }

  SECTION("operator->")
  {
    Expected expected{expectedValue};

    SECTION("returns pointer to value (const)")
    {
      const auto &constExpected = expected;
      REQUIRE(constExpected->size() == expectedValue.size());
    }

    SECTION("returns pointer to value (non-const)")
    {
      REQUIRE(expected->size() == expectedValue.size());

      SECTION("allows modification")
      {
        expected->clear();
        REQUIRE(expected->empty());
      }
    }

    SECTION("invocability for void")
    {
      SECTION("non-invocable for ExpectedVoid &") { STATIC_REQUIRE_FALSE(ArrowInvocable<ExpectedVoid &>); }
      SECTION("non-invocable for const ExpectedVoid &") { STATIC_REQUIRE_FALSE(ArrowInvocable<const ExpectedVoid &>); }
      SECTION("non-invocable for ExpectedVoid &&") { STATIC_REQUIRE_FALSE(ArrowInvocable<ExpectedVoid &&>); }
      SECTION("non-invocable for const ExpectedVoid &&")
      {
        STATIC_REQUIRE_FALSE(ArrowInvocable<const ExpectedVoid &&>);
      }
    }
  }
}

TEST_CASE("Observers (error)", "[expected]")
{
  const std::string modifiedError =
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";

  SECTION("error")
  {
    Expected expected{frt::unexpected{expectedError}};
    ExpectedVoid voidExpected{frt::unexpected{expectedError}};

    SECTION("returns reference to error (const)")
    {
      const auto &constExpected = expected;
      REQUIRE(constExpected.error() == expectedError);
    }

    SECTION("returns reference to error for void value (const)")
    {
      const auto &constExpected = voidExpected;
      REQUIRE(constExpected.error() == expectedError);
    }

    SECTION("returns reference to error (non-const)")
    {
      REQUIRE(expected.error() == expectedError);

      SECTION("allows modification")
      {
        auto &err = expected.error();
        err = modifiedError;
        REQUIRE(expected.error() == modifiedError);
      }
    }

    SECTION("returns reference to error for void value (non-const)")
    {
      REQUIRE(voidExpected.error() == expectedError);

      SECTION("allows modification")
      {
        auto &err = voidExpected.error();
        err = modifiedError;
        REQUIRE(voidExpected.error() == modifiedError);
      }
    }

    SECTION("returns reference to error (rvalue)") { REQUIRE(std::move(expected).error() == expectedError); }

    SECTION("returns reference to error for void value (rvalue)")
    {
      REQUIRE(std::move(voidExpected).error() == expectedError);
    }

    SECTION("returns reference to error (const rvalue)")
    {
      const auto &constExpected = expected;
      REQUIRE(static_cast<const Expected &&>(constExpected).error() == expectedError);
    }

    SECTION("returns reference to error for void value (const rvalue)")
    {
      const auto &constExpected = voidExpected;
      REQUIRE(static_cast<const ExpectedVoid &&>(constExpected).error() == expectedError);
    }

    SECTION("invocability")
    {
      SECTION("invocable for Expected &") { STATIC_REQUIRE(ErrorInvocable<Expected &>); }
      SECTION("invocable for const Expected &") { STATIC_REQUIRE(ErrorInvocable<const Expected &>); }
      SECTION("invocable for Expected &&") { STATIC_REQUIRE(ErrorInvocable<Expected &&>); }
      SECTION("invocable for const Expected &&") { STATIC_REQUIRE(ErrorInvocable<const Expected &&>); }
      SECTION("invocable for ExpectedVoid &") { STATIC_REQUIRE(ErrorInvocable<ExpectedVoid &>); }
      SECTION("invocable for const ExpectedVoid &") { STATIC_REQUIRE(ErrorInvocable<const ExpectedVoid &>); }
      SECTION("invocable for ExpectedVoid &&") { STATIC_REQUIRE(ErrorInvocable<ExpectedVoid &&>); }
      SECTION("invocable for const ExpectedVoid &&") { STATIC_REQUIRE(ErrorInvocable<const ExpectedVoid &&>); }
    }
  }
}

TEST_CASE("Observers (*_or)", "[expected]")
{
  SECTION("value_or")
  {
    const std::vector defaultValue{10, 11, 12, 13, 14};

    SECTION("returns value if present (const &)")
    {
      const Expected expected{expectedValue};
      REQUIRE(expected.value_or(defaultValue) == expectedValue);
    }

    SECTION("returns default value if error present (const &)")
    {
      const Expected expected{frt::unexpected{expectedError}};
      REQUIRE(expected.value_or(defaultValue) == defaultValue);
    }

    SECTION("returns value if present (&&)")
    {
      Expected expected{expectedValue};
      REQUIRE(std::move(expected).value_or(defaultValue) == expectedValue);
    }

    SECTION("returns default value if error present (&&)")
    {
      Expected expected{frt::unexpected{expectedError}};
      REQUIRE(std::move(expected).value_or(defaultValue) == defaultValue);
    }

    SECTION("invocability for void")
    {
      SECTION("non-invocable for ExpectedVoid &") { STATIC_REQUIRE_FALSE(ValueOrInvocable<ExpectedVoid &>); }
      SECTION("non-invocable for const ExpectedVoid &")
      {
        STATIC_REQUIRE_FALSE(ValueOrInvocable<const ExpectedVoid &>);
      }
      SECTION("non-invocable for ExpectedVoid &&") { STATIC_REQUIRE_FALSE(ValueOrInvocable<ExpectedVoid &&>); }
      SECTION("non-invocable for const ExpectedVoid &&")
      {
        STATIC_REQUIRE_FALSE(ValueOrInvocable<const ExpectedVoid &&>);
      }
    }
  }

  SECTION("error_or")
  {
    const std::string defaultError =
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.";

    SECTION("returns error if present (const &)")
    {
      const Expected expected{frt::unexpected{expectedError}};
      REQUIRE(expected.error_or(defaultError) == expectedError);
    }

    SECTION("returns error for void value if present (const &)")
    {
      const ExpectedVoid expected{frt::unexpected{expectedError}};
      REQUIRE(expected.error_or(defaultError) == expectedError);
    }

    SECTION("returns default value if value present (const &)")
    {
      const Expected expected{expectedValue};
      REQUIRE(expected.error_or(defaultError) == defaultError);
    }

    SECTION("returns default value for void value if value present (const &)")
    {
      const ExpectedVoid expected;
      REQUIRE(expected.error_or(defaultError) == defaultError);
    }

    SECTION("returns error if present (&&)")
    {
      Expected expected{frt::unexpected{expectedError}};
      REQUIRE(std::move(expected).error_or(defaultError) == expectedError);
    }

    SECTION("returns error for void value if present (&&)")
    {
      ExpectedVoid expected{frt::unexpected{expectedError}};
      REQUIRE(std::move(expected).error_or(defaultError) == expectedError);
    }

    SECTION("returns default value if value present (&&)")
    {
      Expected expected{expectedValue};
      REQUIRE(std::move(expected).error_or(defaultError) == defaultError);
    }

    SECTION("returns default value for void value if value present (&&)")
    {
      ExpectedVoid expected;
      REQUIRE(std::move(expected).error_or(defaultError) == defaultError);
    }

    SECTION("invocability")
    {
      SECTION("invocable for Expected &") { STATIC_REQUIRE(ErrorOrInvocable<Expected &>); }
      SECTION("invocable for const Expected &") { STATIC_REQUIRE(ErrorOrInvocable<const Expected &>); }
      SECTION("invocable for Expected &&") { STATIC_REQUIRE(ErrorOrInvocable<Expected &&>); }
      SECTION("invocable for const Expected &&") { STATIC_REQUIRE(ErrorOrInvocable<const Expected &&>); }
      SECTION("invocable for ExpectedVoid &") { STATIC_REQUIRE(ErrorOrInvocable<ExpectedVoid &>); }
      SECTION("invocable for const ExpectedVoid &") { STATIC_REQUIRE(ErrorOrInvocable<const ExpectedVoid &>); }
      SECTION("invocable for ExpectedVoid &&") { STATIC_REQUIRE(ErrorOrInvocable<ExpectedVoid &&>); }
      SECTION("invocable for const ExpectedVoid &&") { STATIC_REQUIRE(ErrorOrInvocable<const ExpectedVoid &&>); }
    }
  }
}
