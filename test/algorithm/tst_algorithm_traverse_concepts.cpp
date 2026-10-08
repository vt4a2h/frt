/*
 * FRT -- Result types for modern C++.
 *
 * Copyright Vitaly Fanaskov 2026-present.
 *
 * Use, modification and distribution are subject to the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or a copy at http://www.boost.org/LICENSE_1_0.txt.
 *
 * Project home: https://github.com/vt4a2h/frt.
 */
#include <catch2/catch_all.hpp>

#include <concepts>
#include <string>
#include <utility>
#include <vector>

#include <frt/meta/meta_either_either.hpp>
#include <frt/meta/meta_either_expected.hpp>
#include <frt/meta/meta_either_optional.hpp>

#include <frt/algorithm/traverse.hpp>
#include <frt/either.hpp>
#include <frt/expected.hpp>
#include <frt/optional.hpp>

namespace
{

struct IncompatibleType
{
};

template <class L, class R>
struct DummyResult
{
};

struct ValidFuncRetMeta
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct MissingRightT
{
  using left_t = int;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct MissingLeftT
{
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<int, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct MissingRebindRight
{
  using left_t = int;
  using right_t = double;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct NonTemplateRebindRight
{
  using left_t = int;
  using right_t = double;
  using rebind_right = DummyResult<int, double>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct ConstrainedRebindRight
{
  using left_t = int;
  using right_t = double;

  template <std::floating_point U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct MissingIsRight
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct IncompatibleIsRight
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(IncompatibleType) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct MissingRight
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  int left(DummyResult<int, double> &&) const;
};

struct IncompatibleRight
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(IncompatibleType) const;
  int left(DummyResult<int, double> &&) const;
};

struct MissingLeft
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
};

struct IncompatibleLeft
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(IncompatibleType) const;
};

struct RvalueOnlyFuncRetMeta
{
  using left_t = int;
  using right_t = double;

  template <class U>
  using rebind_right = DummyResult<left_t, U>;

  bool is_right(const DummyResult<int, double> &) const;
  double right(DummyResult<int, double> &&) const;
  int left(DummyResult<int, double> &&) const;
};

struct ValidResMeta
{
  void make_left(int);
  void make_right(double);
};

struct MissingMakeLeft
{
  void make_right(double);
};

struct IncompatibleMakeLeft
{
  void make_left(IncompatibleType);
  void make_right(double);
};

struct MissingMakeRight
{
  void make_left(int);
};

struct IncompatibleMakeRight
{
  void make_left(int);
  void make_right(IncompatibleType);
};

struct EmptyResMeta
{
};

struct RvalueOnlyMakeLeft
{
  void make_left(int &&);
  void make_right(double);
};

struct RvalueOnlyMakeRight
{
  void make_left(int);
  void make_right(double &&);
};

} // namespace

TEMPLATE_TEST_CASE_SIG("ValidMetaEitherFuncRetT", "[algorithm][traverse]",
                       ((class MetaEither, class ResT, bool Valid), MetaEither, ResT, Valid),
                       (frt::meta_either<frt::either<int, double>>, frt::either<int, double>, true),
                       (frt::meta_either<frt::either<std::string, std::vector<int>>>,
                        frt::either<std::string, std::vector<int>>, true),
                       (frt::meta_either<frt::expected<int, double>>, frt::expected<int, double>, true),
                       (frt::meta_either<frt::expected<void, double>>, frt::expected<void, double>, true),
                       (frt::meta_either<frt::optional<int>>, frt::optional<int>, true),
                       (frt::meta_either<frt::optional<std::string>>, frt::optional<std::string>, true),
                       (frt::meta_either<frt::either<int, double>>, int, false),
                       (frt::meta_either<frt::either<int, double>>, frt::expected<int, double>, false),
                       (frt::meta_either<frt::expected<int, double>>, int, false),
                       (frt::meta_either<frt::expected<int, double>>, frt::either<int, double>, false),
                       (frt::meta_either<frt::optional<int>>, int, false),
                       (frt::meta_either<frt::optional<int>>, frt::either<int, int>, false),
                       (int, frt::either<int, double>, false), (void, frt::either<int, double>, false),
                       (std::string, frt::either<int, double>, false),
                       (IncompatibleType, frt::either<int, double>, false))
{
  STATIC_REQUIRE(frt::detail::trav_alg_::ValidMetaEitherFuncRetT<MetaEither, ResT> == Valid);
}

TEMPLATE_TEST_CASE_SIG(
    "ValidMetaEitherFuncRetT requirements", "[algorithm][traverse]",
    ((class MetaEither, class ResT, bool Valid), MetaEither, ResT, Valid),
    (ValidFuncRetMeta, DummyResult<int, double>, true), (MissingRightT, DummyResult<int, double>, false),
    (MissingLeftT, DummyResult<int, double>, false), (MissingRebindRight, DummyResult<int, double>, false),
    (NonTemplateRebindRight, DummyResult<int, double>, false),
    (ConstrainedRebindRight, DummyResult<int, double>, false), (MissingIsRight, DummyResult<int, double>, false),
    (IncompatibleIsRight, DummyResult<int, double>, false), (MissingRight, DummyResult<int, double>, false),
    (IncompatibleRight, DummyResult<int, double>, false), (MissingLeft, DummyResult<int, double>, false),
    (IncompatibleLeft, DummyResult<int, double>, false),
    (RvalueOnlyFuncRetMeta, const DummyResult<int, double> &, false))
{
  STATIC_REQUIRE(frt::detail::trav_alg_::ValidMetaEitherFuncRetT<MetaEither, ResT> == Valid);
}

TEMPLATE_TEST_CASE_SIG(
    "ValidMetaEitherResC", "[algorithm][traverse]",
    ((class MetaEither, class Left, class Right, bool Valid), MetaEither, Left, Right, Valid),
    (frt::meta_either<frt::either<int, std::vector<double>>>, int, std::vector<double>, true),
    (frt::meta_either<frt::either<int, std::vector<double>>>, const int &, std::vector<double> &&, true),
    (frt::meta_either<frt::either<std::string, std::vector<int>>>, const char *, std::vector<int>, true),
    (frt::meta_either<frt::expected<std::vector<int>, double>>, double, std::vector<int>, true),
    (frt::meta_either<frt::expected<std::vector<int>, std::string>>, const char *, std::vector<int>, true),
    (frt::meta_either<frt::optional<std::vector<int>>>, frt::void_t, std::vector<int>, true),
    (int, int, std::vector<int>, false), (void, int, std::vector<int>, false),
    (std::string, int, std::vector<int>, false), (IncompatibleType, int, std::vector<int>, false))
{
  STATIC_REQUIRE(frt::detail::trav_alg_::ValidMetaEitherResC<MetaEither, Left, Right> == Valid);
}

TEMPLATE_TEST_CASE_SIG("ValidMetaEitherResC requirements", "[algorithm][traverse]",
                       ((class MetaEither, class Left, class Right, bool Valid), MetaEither, Left, Right, Valid),
                       (ValidResMeta, int, double, true), (ValidResMeta, const int &, double &&, true),
                       (MissingMakeLeft, int, double, false), (IncompatibleMakeLeft, int, double, false),
                       (MissingMakeRight, int, double, false), (IncompatibleMakeRight, int, double, false),
                       (EmptyResMeta, int, double, false), (RvalueOnlyMakeLeft, const int &, double, false),
                       (RvalueOnlyMakeRight, int, const double &, false))
{
  STATIC_REQUIRE(frt::detail::trav_alg_::ValidMetaEitherResC<MetaEither, Left, Right> == Valid);
}
