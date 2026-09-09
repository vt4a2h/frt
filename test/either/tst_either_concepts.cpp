#include <catch2/catch_all.hpp>

#include <format>
#include <string>
#include <vector>

#include <frt/either.hpp>
#include <frt/format/either-fmt.hpp>

#include <test/helpers/types.hpp>

namespace
{

struct NonDestructible
{
  ~NonDestructible() = delete;
};

struct CallableOneArg
{
  void operator()(int) const;
};

struct CallableTwoArgs
{
  void operator()(int, double) const;
};

struct CallableThreeArgs
{
  void operator()(std::string, int, std::vector<int>) const;
};

struct NothrowCallableOneArg
{
  void operator()(int) noexcept;
};

struct ThrowingCallableOneArg
{
  void operator()(int) noexcept(false);
};

struct NothrowCallableThreeArgs
{
  void operator()(std::string, int, std::vector<int>) noexcept;
};

struct ThrowingCallableThreeArgs
{
  void operator()(std::string, int, std::vector<int>) noexcept(false);
};

struct CallableZeroArgs
{
  void operator()() const;
};

struct NothrowCallableZeroArgs
{
  void operator()() noexcept;
};

struct ThrowingCallableZeroArgs
{
  void operator()() noexcept(false);
};

struct CallableReturnIntOneArg
{
  int operator()(int) const;
};

struct CallableReturnIntZeroArgs
{
  int operator()() const;
};

struct CallableReturnDoubleOneArg
{
  double operator()(int) const;
};

struct CallableReturnDoubleZeroArgs
{
  double operator()() const;
};

struct CallableReturnVoidOneArg
{
  void operator()(int) const;
};

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

TEMPLATE_TEST_CASE_SIG("IsEither", "[either]", ((class T, bool Valid), T, Valid), (int, false), (double, false),
                       (std::string, false), (std::vector<int>, false), (frt::either<int, double>, true),
                       (const frt::either<int, double>, true), (frt::either<int, double> &, true),
                       (const frt::either<int, double> &, true), (frt::either<int, double> &&, true),
                       (frt::either<void, int>, true), (frt::either<int, void>, true), (frt::either<void, void>, true),
                       (frt::either<std::string, std::vector<int>>, true),
                       (const frt::either<std::string, std::vector<int>> &, true))
{
  STATIC_REQUIRE(frt::IsEither<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("is_either_", "[either]", ((class T, bool Valid), T, Valid), (int, false), (double, false),
                       (std::string, false), (std::vector<int>, false), (frt::either<int, double>, true),
                       (frt::either<void, int>, true), (frt::either<int, void>, true), (frt::either<void, void>, true),
                       (frt::either<std::string, std::vector<int>>, true), (const frt::either<int, double>, false),
                       (frt::either<int, double> &, false), (frt::either<int, double> &&, false))
{
  STATIC_REQUIRE(frt::detail::either_::is_either_<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("IsVoid", "[either]", ((class T, bool Valid), T, Valid), (void, true), (const void, true),
                       (volatile void, true), (const volatile void, true), (int, false), (const int, false),
                       (int &, false), (std::string, false), (std::vector<int>, false), (frt::either<void, int>, false),
                       (frt::detail::either_::Void, false))
{
  STATIC_REQUIRE(frt::detail::either_::IsVoid<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("IsNotVoid", "[either]", ((class T, bool Valid), T, Valid), (void, false), (const void, false),
                       (volatile void, false), (const volatile void, false), (int, true), (const int, true),
                       (int &, true), (std::string, true), (std::vector<int>, true), (frt::either<void, int>, true),
                       (frt::detail::either_::Void, true))
{
  STATIC_REQUIRE(frt::detail::either_::IsNotVoid<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("is_unwrapped", "[either]", ((class T, bool Valid), T, Valid), (frt::unwrapped_t, true),
                       (const frt::unwrapped_t, true), (frt::unwrapped_t &, true), (const frt::unwrapped_t &, true),
                       (frt::unwrapped_t &&, true), (decltype(frt::unwrapped), true), (int, false),
                       (const int &, false), (std::string, false), (std::vector<int>, false), (void, false),
                       (frt::in_place_left_t, false), (frt::in_place_right_t, false), (frt::either<int, double>, false))
{
  STATIC_REQUIRE(frt::detail::either_::is_unwrapped_<T> == Valid);
}

TEST_CASE("single_arg_is_unwrapped", "[either]")
{
  SECTION("empty pack") { STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<>); }
  SECTION("single unwrapped argument")
  {
    STATIC_REQUIRE(frt::detail::either_::single_arg_is_unwrapped<frt::unwrapped_t>);
  }
  SECTION("single const unwrapped reference argument")
  {
    STATIC_REQUIRE(frt::detail::either_::single_arg_is_unwrapped<const frt::unwrapped_t &>);
  }
  SECTION("single non-unwrapped argument") { STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<int>); }
  SECTION("single string argument")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<std::string>);
  }
  SECTION("two arguments with first unwrapped")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<frt::unwrapped_t, int>);
  }
  SECTION("two arguments with second unwrapped")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<int, frt::unwrapped_t>);
  }
  SECTION("two unwrapped arguments")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<frt::unwrapped_t, frt::unwrapped_t>);
  }
  SECTION("multiple non-unwrapped arguments")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::single_arg_is_unwrapped<int, double, std::string, std::vector<int>>);
  }
}

TEST_CASE("contains_zero_or_one_unwrapped", "[either]")
{
  SECTION("empty pack") { STATIC_REQUIRE(frt::detail::either_::contains_zero_or_one_unwrapped<>); }
  SECTION("single non-unwrapped argument")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_zero_or_one_unwrapped<int>);
  }
  SECTION("single unwrapped argument")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_zero_or_one_unwrapped<frt::unwrapped_t>);
  }
  SECTION("two non-unwrapped arguments")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_zero_or_one_unwrapped<int, std::string>);
  }
  SECTION("two arguments with one unwrapped first")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_zero_or_one_unwrapped<frt::unwrapped_t, double>);
  }
  SECTION("two arguments with one unwrapped second")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_zero_or_one_unwrapped<int, frt::unwrapped_t>);
  }
  SECTION("three arguments with one unwrapped in middle")
  {
    STATIC_REQUIRE(
        frt::detail::either_::contains_zero_or_one_unwrapped<std::string, frt::unwrapped_t, std::vector<int>>);
  }
  SECTION("two unwrapped arguments")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::contains_zero_or_one_unwrapped<frt::unwrapped_t, frt::unwrapped_t>);
  }
  SECTION("three arguments with two unwrapped")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::contains_zero_or_one_unwrapped<frt::unwrapped_t, int, frt::unwrapped_t>);
  }
  SECTION("three unwrapped arguments")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::contains_zero_or_one_unwrapped<frt::unwrapped_t, frt::unwrapped_t, frt::unwrapped_t>);
  }
}

TEST_CASE("contains_unwrapped", "[either]")
{
  SECTION("empty pack") { STATIC_REQUIRE_FALSE(frt::detail::either_::contains_unwrapped<>); }
  SECTION("single non-unwrapped argument") { STATIC_REQUIRE_FALSE(frt::detail::either_::contains_unwrapped<int>); }
  SECTION("single unwrapped argument") { STATIC_REQUIRE(frt::detail::either_::contains_unwrapped<frt::unwrapped_t>); }
  SECTION("two non-unwrapped arguments")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::contains_unwrapped<int, std::string>);
  }
  SECTION("two arguments with first unwrapped")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_unwrapped<frt::unwrapped_t, double>);
  }
  SECTION("two arguments with second unwrapped")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_unwrapped<int, frt::unwrapped_t>);
  }
  SECTION("multiple arguments without unwrapped")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::contains_unwrapped<int, double, std::string, std::vector<int>>);
  }
  SECTION("multiple arguments with unwrapped in middle")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_unwrapped<int, std::string, frt::unwrapped_t, std::vector<int>>);
  }
  SECTION("two unwrapped arguments")
  {
    STATIC_REQUIRE(frt::detail::either_::contains_unwrapped<frt::unwrapped_t, frt::unwrapped_t>);
  }
}

TEST_CASE("is_invocable_with_extra_args", "[either]")
{
  SECTION("invocable without extra args")
  {
    STATIC_REQUIRE(frt::detail::either_::is_invocable_with_extra_args<CallableOneArg, int>);
  }
  SECTION("not invocable with wrong type without extra args")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::is_invocable_with_extra_args<CallableOneArg, std::string>);
  }
  SECTION("invocable with extra args without unwrapped")
  {
    STATIC_REQUIRE(frt::detail::either_::is_invocable_with_extra_args<CallableTwoArgs, int, double>);
  }
  SECTION("not invocable when extra args missing")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::is_invocable_with_extra_args<CallableTwoArgs, int>);
  }
  SECTION("not invocable with too many extra args")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::is_invocable_with_extra_args<CallableTwoArgs, int, double, int>);
  }
  SECTION("invocable with three args without unwrapped")
  {
    STATIC_REQUIRE(
        frt::detail::either_::is_invocable_with_extra_args<CallableThreeArgs, std::string, int, std::vector<int>>);
  }
  SECTION("invocable with unwrapped as second argument")
  {
    STATIC_REQUIRE(frt::detail::either_::is_invocable_with_extra_args<CallableTwoArgs, double, int, frt::unwrapped_t>);
  }
  SECTION("invocable with unwrapped in middle position")
  {
    STATIC_REQUIRE(frt::detail::either_::is_invocable_with_extra_args<CallableThreeArgs, int, std::string,
                                                                      frt::unwrapped_t, std::vector<int>>);
  }
  SECTION("not invocable with unwrapped replacing with incompatible type")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::is_invocable_with_extra_args<CallableThreeArgs, IncompatibleType, std::string,
                                                           frt::unwrapped_t, std::vector<int>>);
  }
  SECTION("not invocable with unwrapped when arg count mismatch")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::is_invocable_with_extra_args<CallableThreeArgs, int, std::string, frt::unwrapped_t>);
  }
}

TEST_CASE("is_nothrow_invocable_with_extra_args", "[either]")
{
  SECTION("nothrow invocable without extra args")
  {
    STATIC_REQUIRE(frt::detail::either_::is_nothrow_invocable_with_extra_args<NothrowCallableOneArg, int>);
  }
  SECTION("not nothrow when function throws without extra args")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::is_nothrow_invocable_with_extra_args<ThrowingCallableOneArg, int>);
  }
  SECTION("not nothrow when not invocable")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::is_nothrow_invocable_with_extra_args<NothrowCallableOneArg, std::string>);
  }
  SECTION("nothrow invocable with extra args without unwrapped")
  {
    STATIC_REQUIRE(frt::detail::either_::is_nothrow_invocable_with_extra_args<NothrowCallableThreeArgs, std::string,
                                                                              int, std::vector<int>>);
  }
  SECTION("not nothrow when function throws with extra args without unwrapped")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::is_nothrow_invocable_with_extra_args<ThrowingCallableThreeArgs, std::string, int,
                                                                   std::vector<int>>);
  }
  SECTION("nothrow invocable with unwrapped argument")
  {
    STATIC_REQUIRE(
        frt::detail::either_::is_nothrow_invocable_with_extra_args<NothrowCallableThreeArgs, int, std::string,
                                                                   frt::unwrapped_t, std::vector<int>>);
  }
  SECTION("not nothrow when function throws with unwrapped argument")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::is_nothrow_invocable_with_extra_args<ThrowingCallableThreeArgs, int, std::string,
                                                                   frt::unwrapped_t, std::vector<int>>);
  }
  SECTION("not nothrow when type mismatch with unwrapped argument")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::is_nothrow_invocable_with_extra_args<NothrowCallableThreeArgs, IncompatibleType,
                                                                   std::string, frt::unwrapped_t, std::vector<int>>);
  }
}

TEMPLATE_TEST_CASE_SIG("same_right_alternative", "[either]", ((class E1, class E2, bool Valid), E1, E2, Valid),
                       (frt::either<int, double>, frt::either<int, double>, true),
                       (frt::either<int, double>, frt::either<std::string, double>, true),
                       (frt::either<int, double>, const frt::either<std::vector<int>, double> &, true),
                       (frt::either<int, double> &, frt::either<void, double> &&, true),
                       (frt::either<int, void>, frt::either<std::string, void>, true),
                       (frt::either<void, void>, frt::either<int, void>, true),
                       (frt::either<int, double>, frt::either<int, int>, false),
                       (frt::either<int, double>, frt::either<int, std::string>, false),
                       (frt::either<int, void>, frt::either<int, double>, false),
                       (frt::either<int, double>, frt::either<int, void>, false))
{
  STATIC_REQUIRE(frt::detail::either_::same_right_alternative<E1, E2> == Valid);
}

TEMPLATE_TEST_CASE_SIG("same_left_alternative", "[either]", ((class E1, class E2, bool Valid), E1, E2, Valid),
                       (frt::either<int, double>, frt::either<int, double>, true),
                       (frt::either<int, double>, frt::either<int, std::string>, true),
                       (frt::either<int, double>, const frt::either<int, std::vector<double>> &, true),
                       (frt::either<int, double> &, frt::either<int, void> &&, true),
                       (frt::either<void, int>, frt::either<void, std::string>, true),
                       (frt::either<void, void>, frt::either<void, int>, true),
                       (frt::either<int, double>, frt::either<double, double>, false),
                       (frt::either<int, double>, frt::either<std::string, double>, false),
                       (frt::either<void, int>, frt::either<int, int>, false),
                       (frt::either<int, double>, frt::either<void, double>, false))
{
  STATIC_REQUIRE(frt::detail::either_::same_left_alternative<E1, E2> == Valid);
}

TEST_CASE("InvocableV", "[either]")
{
  SECTION("invocable with non-void arg") { STATIC_REQUIRE(frt::detail::either_::InvocableV<CallableOneArg, int>); }
  SECTION("not invocable with incompatible arg")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::InvocableV<CallableOneArg, std::string>);
  }
  SECTION("not invocable with void when expecting arg")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::InvocableV<CallableOneArg, void>);
  }
  SECTION("invocable with void arg") { STATIC_REQUIRE(frt::detail::either_::InvocableV<CallableZeroArgs, void>); }
  SECTION("not invocable with non-void when expecting zero args")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::InvocableV<CallableZeroArgs, int>);
  }
  SECTION("non-callable type is not invocable")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::InvocableV<IncompatibleType, int>);
  }
}

TEST_CASE("NothrowInvocableV", "[either]")
{
  SECTION("nothrow invocable with non-void arg")
  {
    STATIC_REQUIRE(frt::detail::either_::NothrowInvocableV<NothrowCallableOneArg, int>);
  }
  SECTION("throwing with non-void arg")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::NothrowInvocableV<ThrowingCallableOneArg, int>);
  }
  SECTION("nothrow invocable with void arg")
  {
    STATIC_REQUIRE(frt::detail::either_::NothrowInvocableV<NothrowCallableZeroArgs, void>);
  }
  SECTION("throwing with void arg")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::NothrowInvocableV<ThrowingCallableZeroArgs, void>);
  }
  SECTION("not invocable is not nothrow invocable")
  {
    STATIC_REQUIRE_FALSE(frt::detail::either_::NothrowInvocableV<NothrowCallableOneArg, std::string>);
  }
}

TEST_CASE("SameReturnType", "[either]")
{
  SECTION("both non-void args returning same type")
  {
    STATIC_REQUIRE(frt::detail::either_::SameReturnType<CallableReturnIntOneArg, int, CallableReturnIntOneArg, int>);
  }
  SECTION("both non-void args returning different types")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::SameReturnType<CallableReturnIntOneArg, int, CallableReturnDoubleOneArg, int>);
  }
  SECTION("left void arg and right non-void arg returning same type")
  {
    STATIC_REQUIRE(frt::detail::either_::SameReturnType<CallableReturnIntZeroArgs, void, CallableReturnIntOneArg, int>);
  }
  SECTION("left non-void arg and right void arg returning same type")
  {
    STATIC_REQUIRE(frt::detail::either_::SameReturnType<CallableReturnIntOneArg, int, CallableReturnIntZeroArgs, void>);
  }
  SECTION("both void args returning same type")
  {
    STATIC_REQUIRE(
        frt::detail::either_::SameReturnType<CallableReturnIntZeroArgs, void, CallableReturnIntZeroArgs, void>);
  }
  SECTION("both returning void")
  {
    STATIC_REQUIRE(frt::detail::either_::SameReturnType<CallableReturnVoidOneArg, int, CallableZeroArgs, void>);
  }
  SECTION("one returning void and other returning non-void")
  {
    STATIC_REQUIRE_FALSE(
        frt::detail::either_::SameReturnType<CallableReturnVoidOneArg, int, CallableReturnIntOneArg, int>);
  }
}

TEMPLATE_TEST_CASE_SIG("FormattableAlternative", "[either]", ((class T, bool Valid), T, Valid), (int, true),
                       (double, true), (std::string, true), (void, true), (const void, true), (IncompatibleType, false))
{
  STATIC_REQUIRE(frt::detail::either_::FormattableAlternative<T, char> == Valid);
}

TEMPLATE_TEST_CASE_SIG("std::formattable for either", "[either]", ((class T, bool Valid), T, Valid),
                       (frt::either<int, double>, true), (frt::either<int, std::string>, true),
                       (frt::either<void, std::string>, true), (frt::either<int, void>, true),
                       (frt::either<void, void>, true), (frt::either<IncompatibleType, int>, false),
                       (frt::either<int, IncompatibleType>, false),
                       (frt::either<IncompatibleType, IncompatibleType>, false),
                       (frt::either<frt::either<int, double>, std::string>, true),
                       (frt::either<frt::either<IncompatibleType, double>, std::string>, false))
{
  STATIC_REQUIRE(std::formattable<T, char> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidType", "[either]", ((class T, bool Valid), T, Valid), (int, true), (const int, true),
                       (volatile int, true), (const volatile int, true), (double, true), (std::string, true),
                       (std::vector<int>, true), (void, true), (const void, true), (volatile void, true),
                       (const volatile void, true), (test::helpers::TestArgsIl, true), (frt::either<int, double>, true),
                       (frt::either<void, void>, true), (int &, false), (const int &, false), (int &&, false),
                       (const int &&, false), (int[5], false), (int[], false), (const int[3], false),
                       (frt::in_place_left_t, false), (const frt::in_place_left_t, false),
                       (frt::in_place_right_t, false), (const frt::in_place_right_t, false), (frt::unwrapped_t, false),
                       (const frt::unwrapped_t, false), (frt::detail::either_::Void, false),
                       (const frt::detail::either_::Void, false), (void(), false), (NonDestructible, false))
{
  STATIC_REQUIRE(frt::detail::either_::ValidType<T> == Valid);
}

TEMPLATE_TEST_CASE_SIG("CopyConstructibleFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, std::string, int, const char *, true),
                       (int, std::string, int, const char *, true), (double, int, int, int, true),
                       (NonCopyable, double, NonCopyable, double, false),
                       (double, NonCopyable, double, NonCopyable, false),
                       (IncompatibleType, double, int, double, false), (int, IncompatibleType, int, double, false))
{
  STATIC_REQUIRE(frt::detail::either_::CopyConstructibleFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> ==
                 Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowCopyConstructibleFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, int, int, int, true),
                       (ThrowingCopy, double, ThrowingCopy, double, false),
                       (double, ThrowingCopy, double, ThrowingCopy, false),
                       (IncompatibleType, double, int, double, false))
{
  STATIC_REQUIRE(
      frt::detail::either_::NothrowCopyConstructibleFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> == Valid);
}

TEMPLATE_TEST_CASE_SIG("MoveConstructibleFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, std::string, int, std::string, true),
                       (int, std::string, int, const char *, true), (double, int, int, int, true),
                       (NonMovable, double, NonMovable, double, false), (double, NonMovable, double, NonMovable, false),
                       (IncompatibleType, double, int, double, false))
{
  STATIC_REQUIRE(frt::detail::either_::MoveConstructibleFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> ==
                 Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowMoveConstructibleFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, int, int, int, true),
                       (ThrowingMove, double, ThrowingMove, double, false),
                       (double, ThrowingMove, double, ThrowingMove, false),
                       (IncompatibleType, double, int, double, false))
{
  STATIC_REQUIRE(
      frt::detail::either_::NothrowMoveConstructibleFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> == Valid);
}

TEMPLATE_TEST_CASE_SIG("CopyAssignableFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, std::string, int, const char *, true),
                       (int, std::string, int, const char *, true), (double, int, int, int, true),
                       (NonCopyable, double, NonCopyable, double, false),
                       (double, NonCopyable, double, NonCopyable, false),
                       (IncompatibleType, double, int, double, false), (int, IncompatibleType, int, double, false))
{
  STATIC_REQUIRE(frt::detail::either_::CopyAssignableFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> ==
                 Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowCopyAssignableFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, int, int, int, true),
                       (ThrowingCopy, double, ThrowingCopy, double, false),
                       (double, ThrowingCopy, double, ThrowingCopy, false),
                       (IncompatibleType, double, int, double, false))
{
  STATIC_REQUIRE(frt::detail::either_::NothrowCopyAssignableFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> ==
                 Valid);
}

TEMPLATE_TEST_CASE_SIG("MoveAssignableFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, std::string, int, std::string, true),
                       (int, std::string, int, const char *, true), (double, int, int, int, true),
                       (NonMovable, double, NonMovable, double, false), (double, NonMovable, double, NonMovable, false),
                       (IncompatibleType, double, int, double, false))
{
  STATIC_REQUIRE(frt::detail::either_::MoveAssignableFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> ==
                 Valid);
}

TEMPLATE_TEST_CASE_SIG("NothrowMoveAssignableFromAnotherEither", "[either]",
                       ((class Left, class Right, class AnotherLeft, class AnotherRight, bool Valid), Left, Right,
                        AnotherLeft, AnotherRight, Valid),
                       (int, double, int, double, false), (double, int, int, int, true),
                       (ThrowingMove, double, ThrowingMove, double, false),
                       (double, ThrowingMove, double, ThrowingMove, false),
                       (IncompatibleType, double, int, double, false))
{
  STATIC_REQUIRE(frt::detail::either_::NothrowMoveAssignableFromAnotherEither<Left, Right, AnotherLeft, AnotherRight> ==
                 Valid);
}
