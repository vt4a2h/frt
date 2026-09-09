#include <catch2/catch_all.hpp>

#include <concepts>
#include <string>
#include <vector>

#include <frt/all.hpp>

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

struct ThrowingCompare
{
  bool operator==(const ThrowingCompare &) const noexcept(false) { return true; }
  std::partial_ordering operator<=>(const ThrowingCompare &) const noexcept(false)
  {
    return std::partial_ordering::equivalent;
  }
};

struct IncompatibleType
{
};

} // namespace

TEMPLATE_TEST_CASE_SIG("IsOptional", "[optional]", ((class T, bool Valid), T, Valid), (frt::optional<int>, true),
                       (const frt::optional<int>, true), (frt::optional<int> &, true),
                       (const frt::optional<int> &, true), (frt::optional<int> &&, true),
                       (const frt::optional<int> &&, true), (frt::optional<std::string>, true),
                       (const frt::optional<std::vector<int>> &, true), (int, false), (double, false),
                       (std::string, false), (std::vector<int>, false), (void, false), (frt::in_place_opt_t, false),
                       (frt::nullopt_t, false), (frt::either<void, int>, false), (frt::expected<int, double>, false))
{
  STATIC_REQUIRE(frt::IsOptional<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("is_optional_", "[optional]", ((class T, bool Valid), T, Valid), (frt::optional<int>, true),
                       (frt::optional<std::string>, true), (const frt::optional<int>, true),
                       (frt::optional<int> &, true), (frt::optional<int> &&, true), (int, false), (double, false),
                       (std::string, false), (void, false), (frt::nullopt_t, false))
{
  STATIC_REQUIRE(frt::detail::optional_::is_optional_<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("is_tag_", "[optional]", ((class T, bool Valid), T, Valid), (frt::in_place_opt_t, true),
                       (frt::nullopt_t, true), (const frt::in_place_opt_t, true), (const frt::nullopt_t, true),
                       (frt::in_place_opt_t &, true), (frt::nullopt_t &, true), (frt::in_place_opt_t &&, true),
                       (frt::nullopt_t &&, true), (int, false), (std::string, false), (frt::optional<int>, false))
{
  STATIC_REQUIRE(frt::detail::optional_::is_tag_<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidType", "[optional]", ((class T, bool Valid), T, Valid), (int, true), (double, true),
                       (std::string, true), (std::vector<int>, true), (test::helpers::TestArgsIl, true),
                       (const int, true), (void, false), (const void, false), (int &, false), (const int &, false),
                       (int &&, false), (int[5], false), (int[], false), (frt::in_place_opt_t, false),
                       (frt::nullopt_t, false), (void(), false))
{
  STATIC_REQUIRE(frt::detail::optional_::ValidType<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidOtherType", "[optional]", ((class U, class T, bool Valid), U, T, Valid), (int, int, true),
                       (int, double, true), (double, int, true), (const char *, std::string, true),
                       (frt::in_place_opt_t, int, false), (frt::optional<int>, int, false),
                       (frt::optional<int>, bool, false), (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::ValidOtherType<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("CopyConstructibleFromOtherOptional", "[optional]",
                       ((class U, class T, bool Valid), U, T, Valid), (int, double, true), (short, int, true),
                       (const char *, std::string, true), (int, bool, true), (int, int, false),
                       (std::string, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::CopyConstructibleFromOtherOptional<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("MoveConstructibleFromOtherOptional", "[optional]",
                       ((class U, class T, bool Valid), U, T, Valid), (int, double, true), (short, int, true),
                       (const char *, std::string, true), (int, bool, true), (int, int, false),
                       (std::string, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::MoveConstructibleFromOtherOptional<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("EqualityComparable", "[optional]", ((class T, class U, bool Valid), T, U, Valid),
                       (int, int, true), (int, double, true), (std::string, std::string, true),
                       (IncompatibleType, IncompatibleType, false), (int, IncompatibleType, false),
                       (int, std::string, false))
{
  STATIC_REQUIRE(frt::detail::optional_::EqualityComparable<T, U> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ThreeWayComparable", "[optional]", ((class T, class U, bool Valid), T, U, Valid),
                       (int, int, true), (int, double, true), (std::string, std::string, true),
                       (IncompatibleType, IncompatibleType, false), (int, IncompatibleType, false),
                       (int, std::string, false))
{
  STATIC_REQUIRE(frt::detail::optional_::ThreeWayComparable<T, U> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NoexceptEqualityComparable", "[optional]", ((class T, class U, bool Valid), T, U, Valid),
                       (int, int, true), (double, double, true), (std::string, std::string, true),
                       (ThrowingCompare, ThrowingCompare, false), (int, ThrowingCompare, false),
                       (IncompatibleType, IncompatibleType, false))
{
  STATIC_REQUIRE(frt::detail::optional_::NothrowEqualityComparable<T, U> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NoexceptThreeWayComparable", "[optional]", ((class T, class U, bool Valid), T, U, Valid),
                       (int, int, true), (double, double, true), (std::string, std::string, true),
                       (ThrowingCompare, ThrowingCompare, false), (int, ThrowingCompare, false),
                       (IncompatibleType, IncompatibleType, false))
{
  STATIC_REQUIRE(frt::detail::optional_::NothrowThreeWayComparable<T, U> == Valid);
}

TEMPLATE_TEST_CASE_SIG("CopyConstructibleFromEither", "[optional]", ((class U, class T, bool Valid), U, T, Valid),
                       (int, double, true), (const char *, std::string, true), (NonCopyable, NonCopyable, false),
                       (IncompatibleType, int, false), (int, int, true))
{
  STATIC_REQUIRE(frt::detail::optional_::CopyConstructibleFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowCopyConstructibleFromEither", "[optional]",
                       ((class U, class T, bool Valid), U, T, Valid), (int, double, true),
                       (ThrowingCopy, ThrowingCopy, false), (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::NothrowCopyConstructibleFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("CopyAssignableFromEither", "[optional]", ((class U, class T, bool Valid), U, T, Valid),
                       (int, double, true), (const char *, std::string, true), (NonCopyable, NonCopyable, false),
                       (IncompatibleType, int, false), (int, int, true))
{
  STATIC_REQUIRE(frt::detail::optional_::CopyAssignableFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowCopyAssignableFromEither", "[optional]", ((class U, class T, bool Valid), U, T, Valid),
                       (int, double, true), (ThrowingCopy, ThrowingCopy, false), (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::NothrowCopyAssignableFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("MoveConstructibleFromEither", "[optional]", ((class U, class T, bool Valid), U, T, Valid),
                       (int, double, true), (std::string, std::string, true), (NonMovable, NonMovable, false),
                       (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::MoveConstructibleFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowMoveConstructibleFromEither", "[optional]",
                       ((class U, class T, bool Valid), U, T, Valid), (int, double, true),
                       (ThrowingMove, ThrowingMove, false), (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::NothrowMoveConstructibleFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("MoveAssignableFromEither", "[optional]", ((class U, class T, bool Valid), U, T, Valid),
                       (int, double, true), (std::string, std::string, true), (NonMovable, NonMovable, false),
                       (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::MoveAssignableFromEither<U, T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowMoveAssignableFromEither", "[optional]", ((class U, class T, bool Valid), U, T, Valid),
                       (int, double, true), (ThrowingMove, ThrowingMove, false), (IncompatibleType, int, false))
{
  STATIC_REQUIRE(frt::detail::optional_::NothrowMoveAssignableFromEither<U, T> == Valid);
}

TEST_CASE("optional constructors and concepts", "[optional]")
{
  SECTION("default constructible for int value")
  {
    STATIC_REQUIRE(std::is_default_constructible_v<frt::optional<int>>);
  }

  SECTION("default constructible for NonDefaultConstructible value")
  {
    STATIC_REQUIRE(std::is_default_constructible_v<frt::optional<NonDefaultConstructible>>);
  }

  SECTION("copy constructible for int value") { STATIC_REQUIRE(std::copy_constructible<frt::optional<int>>); }

  SECTION("move constructible for int value") { STATIC_REQUIRE(std::move_constructible<frt::optional<int>>); }

  SECTION("copy assignable for int value") { STATIC_REQUIRE(std::is_copy_assignable_v<frt::optional<int>>); }

  SECTION("move assignable for int value") { STATIC_REQUIRE(std::is_move_assignable_v<frt::optional<int>>); }

  SECTION("non-copyable value type is not copy constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<frt::optional<NonCopyable>>);
  }

  SECTION("non-copyable value type is move constructible")
  {
    STATIC_REQUIRE(std::is_move_constructible_v<frt::optional<NonCopyable>>);
  }

  SECTION("non-copyable value type is not copy assignable")
  {
    STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<frt::optional<NonCopyable>>);
  }

  SECTION("non-copyable value type is move assignable")
  {
    STATIC_REQUIRE(std::is_move_assignable_v<frt::optional<NonCopyable>>);
  }

  SECTION("non-movable value type is not move constructible")
  {
    STATIC_REQUIRE_FALSE(std::is_move_constructible_v<frt::optional<NonMovable>>);
  }

  SECTION("non-movable value type is not move assignable")
  {
    STATIC_REQUIRE_FALSE(std::is_move_assignable_v<frt::optional<NonMovable>>);
  }

  SECTION("value constructor with compatible type")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::optional<int>, int>);
  }

  SECTION("value constructor with convertible type")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::optional<double>, int>);
  }

  SECTION("value constructor with incompatible type")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::optional<int>, IncompatibleType>);
  }

  SECTION("nullopt constructor") { STATIC_REQUIRE(std::is_constructible_v<frt::optional<int>, frt::nullopt_t>); }

  SECTION("in_place_opt constructor with valid args")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::optional<int>, frt::in_place_opt_t, int>);
  }

  SECTION("in_place_opt constructor with invalid args")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::optional<int>, frt::in_place_opt_t, IncompatibleType>);
  }

  SECTION("equality comparable for int value") { STATIC_REQUIRE(std::equality_comparable<frt::optional<int>>); }

  SECTION("not equality comparable when value is incompatible")
  {
    STATIC_REQUIRE_FALSE(frt::detail::optional_::EqualityComparable<IncompatibleType, IncompatibleType>);
  }

  SECTION("three-way comparable for int value") { STATIC_REQUIRE(std::three_way_comparable<frt::optional<int>>); }

  SECTION("not three-way comparable when value is incompatible")
  {
    STATIC_REQUIRE_FALSE(frt::detail::optional_::ThreeWayComparable<IncompatibleType, IncompatibleType>);
  }
  SECTION("constructible from const either<void, U> &")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::optional<int>, const frt::either<void, int> &>);
  }

  SECTION("constructible from either<void, U> &&")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::optional<int>, frt::either<void, int> &&>);
  }

  SECTION("constructible from const either with convertible type")
  {
    STATIC_REQUIRE(std::is_constructible_v<frt::optional<double>, const frt::either<void, int> &>);
  }

  SECTION("not constructible from const either with incompatible type")
  {
    STATIC_REQUIRE_FALSE(std::is_constructible_v<frt::optional<int>, const frt::either<void, IncompatibleType> &>);
  }

  SECTION("assignable from const either<void, U> &")
  {
    STATIC_REQUIRE(std::is_assignable_v<frt::optional<int> &, const frt::either<void, int> &>);
  }

  SECTION("assignable from either<void, U> &&")
  {
    STATIC_REQUIRE(std::is_assignable_v<frt::optional<int> &, frt::either<void, int> &&>);
  }

  SECTION("assignable from const either with convertible type")
  {
    STATIC_REQUIRE(std::is_assignable_v<frt::optional<double> &, const frt::either<void, int> &>);
  }

  SECTION("not assignable from const either with incompatible type")
  {
    STATIC_REQUIRE_FALSE(std::is_assignable_v<frt::optional<int> &, const frt::either<void, IncompatibleType> &>);
  }
}
