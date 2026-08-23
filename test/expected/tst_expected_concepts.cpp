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

struct NonDefaultConstructible
{
  explicit NonDefaultConstructible(int) {}
};

template <class T>
struct ExplicitFrom
{
  explicit ExplicitFrom(T) {}
};

struct ThrowingCopy
{
  ThrowingCopy() = default;
  ThrowingCopy(const ThrowingCopy &) noexcept(false) {}
  ThrowingCopy(ThrowingCopy &&) noexcept = default;
  ThrowingCopy &operator=(const ThrowingCopy &) noexcept(false) = default;
  ThrowingCopy &operator=(ThrowingCopy &&) noexcept = default;
};

struct ThrowingMove
{
  ThrowingMove() = default;
  ThrowingMove(const ThrowingMove &) noexcept = default;
  ThrowingMove(ThrowingMove &&) noexcept(false) {}
  ThrowingMove &operator=(const ThrowingMove &) noexcept = default;
  ThrowingMove &operator=(ThrowingMove &&) noexcept(false) = default;
};

struct IncompatibleType
{
};

} // namespace

TEMPLATE_TEST_CASE_SIG("IsExpected", "[expected]", ((class T, bool Valid), T, Valid),
                       (frt::expected<int, double>, true), (const frt::expected<int, double>, true),
                       (frt::expected<int, double> &, true), (const frt::expected<int, double> &, true),
                       (frt::expected<int, double> &&, true), (const frt::expected<int, double> &&, true),
                       (frt::expected<void, double>, true), (const frt::expected<void, double> &, true),
                       (frt::expected<std::string, std::vector<int>>, true), (int, false), (double, false),
                       (std::string, false), (std::vector<int>, false), (void, false),
                       (frt::in_place_expected_t, false), (frt::in_place_unexpected_t, false),
                       (frt::unexpected<int>, false), (frt::either<int, double>, false))
{
  STATIC_REQUIRE(frt::IsExpected<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("is_expected_", "[expected]", ((class T, bool Valid), T, Valid),
                       (frt::expected<int, double>, true), (frt::expected<void, double>, true),
                       (frt::expected<std::string, std::vector<int>>, true), (const frt::expected<int, double>, false),
                       (frt::expected<int, double> &, false), (frt::expected<int, double> &&, false), (int, false),
                       (double, false), (std::string, false), (void, false), (frt::unexpected<int>, false))
{
  STATIC_REQUIRE(frt::detail::expected_::is_expected_<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidValueType", "[expected]", ((class T, bool Valid), T, Valid), (int, true), (double, true),
                       (std::string, true), (std::vector<int>, true), (void, true), (const void, true),
                       (test::helpers::TestArgsIl, true), (const int, true), (int &, false), (const int &, false),
                       (int &&, false), (int[5], false), (int[], false), (frt::unexpected<int>, false),
                       (frt::in_place_expected_t, false), (frt::in_place_unexpected_t, false), (void(), false))
{
  STATIC_REQUIRE(frt::detail::expected_::ValidValueType<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidErrorType", "[expected]", ((class T, bool Valid), T, Valid), (int, true), (double, true),
                       (std::string, true), (std::vector<int>, true), (test::helpers::TestArgsIl, true), (void, false),
                       (const int, false), (volatile int, false), (int &, false), (const int &, false), (int &&, false),
                       (int[5], false), (int[], false), (frt::unexpected<int>, false),
                       (frt::in_place_expected_t, false), (frt::in_place_unexpected_t, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ValidErrorType<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("DifferentTypes", "[expected]", ((class T, class E, bool Valid), T, E, Valid),
                       (int, double, true), (void, int, true), (std::string, int, true),
                       (std::vector<int>, std::string, true), (int, int, false), (const int, int, false),
                       (int, const int, false), (void, void, false))
{
  STATIC_REQUIRE(frt::detail::expected_::DifferentTypes<T, E> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ConstructableFrom", "[expected]", ((class T, class U, bool Valid), T, U, Valid),
                       (int, int, true), (int, double, true), (double, int, true), (std::string, const char *, true),
                       (void, void, true), (void, const void, true), (void, int, false), (int, void, false),
                       (int, std::string, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ConstructableFrom<T, U> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ConvertibleTo", "[expected]", ((class T, class U, bool Valid), T, U, Valid), (int, int, true),
                       (int, double, true), (double, int, true), (void, void, true), (void, const void, true),
                       (std::string, int, false), (void, int, false), (int, void, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ConvertibleTo<T, U> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ConstructableFromAnotherExpectedCref", "[expected]",
                       ((class T, class E, class U, class G, bool Valid), T, E, U, G, Valid),
                       (int, double, int, double, true), (double, int, int, short, true),
                       (std::string, int, const char *, int, true), (void, double, void, int, true),
                       (int, double, void, double, false), (void, double, int, double, false),
                       (int, double, std::string, double, false), (int, double, int, std::string, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ConstructableFromAnotherExpectedCref<T, E, U, G> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ConstructableFromAnotherExpected", "[expected]",
                       ((class T, class E, class U, class G, bool Valid), T, E, U, G, Valid),
                       (int, double, int, double, true), (double, int, int, short, true),
                       (std::string, int, const char *, int, true), (void, double, void, int, true),
                       (int, double, void, double, false), (void, double, int, double, false),
                       (int, double, std::string, double, false), (int, double, int, std::string, false))
{
  STATIC_REQUIRE(frt::detail::expected_::ConstructableFromAnotherExpected<T, E, U, G> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ExplicitFromAnotherCref", "[expected]",
                       ((class T, class E, class U, class G, bool Valid), T, E, U, G, Valid),
                       (int, double, int, double, false), (double, int, int, short, false),
                       (void, double, void, int, false), (std::string, int, const char *, int, false),
                       (ExplicitFrom<int>, double, int, double, true), (int, ExplicitFrom<double>, int, double, true))
{
  STATIC_REQUIRE(frt::detail::expected_::ExplicitFromAnotherCref<T, E, U, G> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ExplicitFromAnother", "[expected]",
                       ((class T, class E, class U, class G, bool Valid), T, E, U, G, Valid),
                       (int, double, int, double, false), (double, int, int, short, false),
                       (void, double, void, int, false), (std::string, int, const char *, int, false),
                       (ExplicitFrom<int>, double, int, double, true), (int, ExplicitFrom<double>, int, double, true))
{
  STATIC_REQUIRE(frt::detail::expected_::ExplicitFromAnother<T, E, U, G> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NoexceptFromAnotherCref", "[expected]",
                       ((class T, class E, class U, class G, bool Valid), T, E, U, G, Valid),
                       (int, double, int, double, true), (void, double, void, int, true),
                       (ThrowingCopy, double, ThrowingCopy, double, false),
                       (int, ThrowingCopy, int, ThrowingCopy, false))
{
  STATIC_REQUIRE(frt::detail::expected_::NoexceptFromAnotherCref<T, E, U, G> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NoexceptFromAnother", "[expected]",
                       ((class T, class E, class U, class G, bool Valid), T, E, U, G, Valid),
                       (int, double, int, double, true), (void, double, void, int, true),
                       (ThrowingMove, double, ThrowingMove, double, false),
                       (int, ThrowingMove, int, ThrowingMove, false))
{
  STATIC_REQUIRE(frt::detail::expected_::NoexceptFromAnother<T, E, U, G> == Valid);
}

TEST_CASE("expected constructors and concepts", "[expected]")
{
  SECTION("default constructible for int value")
  {
    STATIC_REQUIRE(std::is_default_constructible_v<frt::expected<int, double>>);
  }
  SECTION("default constructible for void value")
  {
    STATIC_REQUIRE(std::is_default_constructible_v<frt::expected<void, double>>);
  }
  SECTION("not default constructible when value is not default constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<frt::expected<NonDefaultConstructible, double>>);
  }
  SECTION("copy constructible for int value and double error")
  {
    STATIC_REQUIRE(std::copy_constructible<frt::expected<int, double>>);
  }
  SECTION("move constructible for int value and double error")
  {
    STATIC_REQUIRE(std::move_constructible<frt::expected<int, double>>);
  }
  SECTION("copy assignable for int value and double error")
  {
    STATIC_REQUIRE(std::is_copy_assignable_v<frt::expected<int, double>>);
  }
  SECTION("move assignable for int value and double error")
  {
    STATIC_REQUIRE(std::is_move_assignable_v<frt::expected<int, double>>);
  }
  SECTION("copy constructible for void value") { STATIC_REQUIRE(std::copy_constructible<frt::expected<void, double>>); }
  SECTION("move constructible for void value") { STATIC_REQUIRE(std::move_constructible<frt::expected<void, double>>); }
  SECTION("non-copyable value type is not copy constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<frt::expected<NonCopyable, double>>);
  }
  SECTION("non-copyable value type is move constructible")
  {
    STATIC_REQUIRE(std::is_move_constructible_v<frt::expected<NonCopyable, double>>);
  }
  SECTION("non-copyable error type is not copy constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<frt::expected<int, NonCopyable>>);
  }
  SECTION("non-copyable error type is move constructible")
  {
    STATIC_REQUIRE(std::is_move_constructible_v<frt::expected<int, NonCopyable>>);
  }
  SECTION("non-movable value type is not move constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_move_constructible_v<frt::expected<NonMovable, double>>);
  }
  SECTION("non-movable error type is not move constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_move_constructible_v<frt::expected<int, NonMovable>>);
  }
  SECTION("value constructor with compatible type")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<int, double>, int>);
  }
  SECTION("value constructor with convertible type")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<double, std::string>, int>);
  }
  SECTION("value constructor with incompatible type")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::expected<int, double>, IncompatibleType>);
  }
  SECTION("in_place_expected constructor with NonDefaultConstructible and no args")
  {
    STATIC_REQUIRE_FALSE(
        std::is_constructible_v<frt::expected<NonDefaultConstructible, double>, frt::in_place_expected_t>);
  }
  SECTION("value constructor for void type is not constructible from value")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::expected<void, double>, int>);
  }
  SECTION("bool value expected cannot be constructed from another expected via value ctor")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::expected<bool, double>, frt::expected<int, std::string>>);
  }
  SECTION("unexpected constructor with compatible error")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<int, double>, frt::unexpected<double>>);
  }
  SECTION("unexpected constructor with convertible error")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<int, double>, frt::unexpected<int>>);
  }
  SECTION("unexpected constructor with incompatible error")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::expected<int, double>, frt::unexpected<std::string>>);
  }
  SECTION("in_place_expected constructor with valid args")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<int, double>, frt::in_place_expected_t, int>);
  }
  SECTION("in_place_expected constructor with invalid args")
  {
    STATIC_REQUIRE_FALSE(
        std::is_constructible_v<frt::expected<int, double>, frt::in_place_expected_t, IncompatibleType>);
  }
  SECTION("in_place_expected constructor for void with no args")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<void, double>, frt::in_place_expected_t>);
  }
  SECTION("in_place_expected constructor for void with args is invalid")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::expected<void, double>, frt::in_place_expected_t, int>);
  }
  SECTION("in_place_unexpected constructor with valid args")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::expected<int, double>, frt::in_place_unexpected_t, double>);
  }
  SECTION("in_place_unexpected constructor with invalid args")
  {
    STATIC_REQUIRE_FALSE(
        std::is_constructible_v<frt::expected<int, double>, frt::in_place_unexpected_t, IncompatibleType>);
  }
}
