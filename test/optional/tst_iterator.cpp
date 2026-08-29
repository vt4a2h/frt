#include <catch2/catch_all.hpp>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include <frt/detail/iterator.hpp>

#include <test/helpers/types.hpp>

namespace
{

const int expectedIntValue = 42;
const int modifiedIntValue = 84;

const std::string expectedStringValue =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
    "magna aliqua";
const std::string modifiedStringValue =
    "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
    "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";

const std::string firstString =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
    "magna aliqua";
const std::string secondString =
    "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
    "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";
const std::string thirdString =
    "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque "
    "corrupti quos dolores";
const std::string fourthString =
    "Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat "
    "facere possimus";
const std::string nonExistentString =
    "Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur";

const std::vector expectedVectorValue{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const std::vector modifiedVectorValue{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

const int initialWidth = 100;
const int initialHeight = 200;
const int modifiedWidth = 300;

const std::ptrdiff_t zeroOffset = 0;
const std::ptrdiff_t oneOffset = 1;
const std::ptrdiff_t twoOffset = 2;
const std::ptrdiff_t threeOffset = 3;
const std::ptrdiff_t minusOneOffset = -1;
const std::ptrdiff_t minusTwoOffset = -2;
const std::size_t expectedRangeSize = 3;

} // namespace

TEST_CASE("Concepts and types", "[optional]")
{
  SECTION("std::contiguous_iterator for int")
  {
    STATIC_REQUIRE(std::contiguous_iterator<frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::contiguous_iterator for const int")
  {
    STATIC_REQUIRE(std::contiguous_iterator<frt::detail::optional_::contiguous_iterator<const int>>);
  }

  SECTION("std::contiguous_iterator for string")
  {
    STATIC_REQUIRE(std::contiguous_iterator<frt::detail::optional_::contiguous_iterator<std::string>>);
  }

  SECTION("std::contiguous_iterator for const string")
  {
    STATIC_REQUIRE(std::contiguous_iterator<frt::detail::optional_::contiguous_iterator<const std::string>>);
  }

  SECTION("std::contiguous_iterator for vector")
  {
    STATIC_REQUIRE(std::contiguous_iterator<frt::detail::optional_::contiguous_iterator<std::vector<int>>>);
  }

  SECTION("std::random_access_iterator for int")
  {
    STATIC_REQUIRE(std::random_access_iterator<frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::bidirectional_iterator for int")
  {
    STATIC_REQUIRE(std::bidirectional_iterator<frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::forward_iterator for int")
  {
    STATIC_REQUIRE(std::forward_iterator<frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::input_iterator for int")
  {
    STATIC_REQUIRE(std::input_iterator<frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::output_iterator for int")
  {
    STATIC_REQUIRE(std::output_iterator<frt::detail::optional_::contiguous_iterator<int>, int>);
  }

  SECTION("not std::output_iterator for const int")
  {
    STATIC_REQUIRE_FALSE(std::output_iterator<frt::detail::optional_::contiguous_iterator<const int>, int>);
  }

  SECTION("std::indirectly_readable for int")
  {
    STATIC_REQUIRE(std::indirectly_readable<frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::indirectly_writable for int")
  {
    STATIC_REQUIRE(std::indirectly_writable<frt::detail::optional_::contiguous_iterator<int>, int>);
  }

  SECTION("std::sentinel_for for int")
  {
    STATIC_REQUIRE(std::sentinel_for<frt::detail::optional_::contiguous_iterator<int>,
                                     frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("std::sized_sentinel_for for int")
  {
    STATIC_REQUIRE(std::sized_sentinel_for<frt::detail::optional_::contiguous_iterator<int>,
                                           frt::detail::optional_::contiguous_iterator<int>>);
  }

  SECTION("iterator_concept member type")
  {
    STATIC_REQUIRE(
        std::same_as<frt::detail::optional_::contiguous_iterator<int>::iterator_concept, std::contiguous_iterator_tag>);
  }

  SECTION("value_type member type")
  {
    STATIC_REQUIRE(std::same_as<frt::detail::optional_::contiguous_iterator<int>::value_type, int>);
  }

  SECTION("difference_type member type")
  {
    STATIC_REQUIRE(std::same_as<frt::detail::optional_::contiguous_iterator<int>::difference_type, std::ptrdiff_t>);
  }

  SECTION("pointer member type")
  {
    STATIC_REQUIRE(std::same_as<frt::detail::optional_::contiguous_iterator<int>::pointer, int *>);
  }

  SECTION("reference member type")
  {
    STATIC_REQUIRE(std::same_as<frt::detail::optional_::contiguous_iterator<int>::reference, int &>);
  }

  SECTION("value_type member type for const string")
  {
    STATIC_REQUIRE(
        std::same_as<frt::detail::optional_::contiguous_iterator<const std::string>::value_type, const std::string>);
  }

  SECTION("pointer member type for const string")
  {
    STATIC_REQUIRE(
        std::same_as<frt::detail::optional_::contiguous_iterator<const std::string>::pointer, const std::string *>);
  }

  SECTION("reference member type for const string")
  {
    STATIC_REQUIRE(
        std::same_as<frt::detail::optional_::contiguous_iterator<const std::string>::reference, const std::string &>);
  }
}

TEST_CASE("Constructors and assignment", "[optional]")
{
  SECTION("default constructor")
  {
    const frt::detail::optional_::contiguous_iterator<int> it;
    const frt::detail::optional_::contiguous_iterator<int> other;
    REQUIRE(it == other);
  }

  SECTION("pointer constructor with non-null pointer (int)")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedIntValue);
  }

  SECTION("pointer constructor with null pointer")
  {
    const frt::detail::optional_::contiguous_iterator<int> it{nullptr};
    const frt::detail::optional_::contiguous_iterator<int> other;
    REQUIRE(it == other);
  }

  SECTION("CTAD (int)")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedIntValue);
  }

  SECTION("CTAD (string)")
  {
    std::string val = expectedStringValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedStringValue);
  }

  SECTION("CTAD (vector)")
  {
    std::vector val = expectedVectorValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedVectorValue);
  }

  SECTION("CTAD (const int)")
  {
    const int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedIntValue);
  }

  SECTION("copy constructor (value)")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    const frt::detail::optional_::contiguous_iterator other{it};
    REQUIRE(*other == expectedIntValue);
  }

  SECTION("copy constructor (equality)")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    const frt::detail::optional_::contiguous_iterator other{it};
    REQUIRE(other == it);
  }

  SECTION("move constructor (value)")
  {
    int val = expectedIntValue;
    frt::detail::optional_::contiguous_iterator it{&val};
    const frt::detail::optional_::contiguous_iterator other{std::move(it)};
    REQUIRE(*other == expectedIntValue);
  }

  SECTION("copy assignment (value)")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    frt::detail::optional_::contiguous_iterator<int> other;
    other = it;
    REQUIRE(*other == expectedIntValue);
  }

  SECTION("copy assignment (equality)")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    frt::detail::optional_::contiguous_iterator<int> other;
    other = it;
    REQUIRE(other == it);
  }

  SECTION("move assignment (value)")
  {
    int val = expectedIntValue;
    frt::detail::optional_::contiguous_iterator it{&val};
    frt::detail::optional_::contiguous_iterator<int> other;
    other = std::move(it);
    REQUIRE(*other == expectedIntValue);
  }
}

TEST_CASE("Dereference and member access", "[optional]")
{
  SECTION("operator* for int")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedIntValue);
  }

  SECTION("operator* for string")
  {
    std::string val = expectedStringValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedStringValue);
  }

  SECTION("operator* for vector")
  {
    std::vector val = expectedVectorValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(*it == expectedVectorValue);
  }

  SECTION("operator* modification for int")
  {
    int val = expectedIntValue;
    frt::detail::optional_::contiguous_iterator it{&val};
    *it = modifiedIntValue;
    REQUIRE(*it == modifiedIntValue);
  }

  SECTION("operator* modification for string")
  {
    std::string val = expectedStringValue;
    frt::detail::optional_::contiguous_iterator it{&val};
    *it = modifiedStringValue;
    REQUIRE(*it == modifiedStringValue);
  }

  SECTION("operator* modification for vector")
  {
    std::vector val = expectedVectorValue;
    frt::detail::optional_::contiguous_iterator it{&val};
    *it = modifiedVectorValue;
    REQUIRE(*it == modifiedVectorValue);
  }

  SECTION("operator* for const int")
  {
    const int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator constIt{&val};
    REQUIRE(*constIt == expectedIntValue);
  }

  SECTION("operator* for const string")
  {
    const std::string val = expectedStringValue;
    const frt::detail::optional_::contiguous_iterator constIt{&val};
    REQUIRE(*constIt == expectedStringValue);
  }

  SECTION("operator-> struct member access")
  {
    test::helpers::Widget widget{initialWidth, initialHeight};
    const frt::detail::optional_::contiguous_iterator it{&widget};
    REQUIRE(it->width == initialWidth);
  }

  SECTION("operator-> struct member modification")
  {
    test::helpers::Widget widget{initialWidth, initialHeight};
    frt::detail::optional_::contiguous_iterator it{&widget};
    it->width = modifiedWidth;
    REQUIRE(it->width == modifiedWidth);
  }

  SECTION("operator-> string member function")
  {
    std::string val = expectedStringValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(it->length() == expectedStringValue.length());
  }

  SECTION("operator-> vector member function")
  {
    std::vector val = expectedVectorValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(it->size() == expectedVectorValue.size());
  }

  SECTION("std::to_address")
  {
    int val = expectedIntValue;
    const frt::detail::optional_::contiguous_iterator it{&val};
    REQUIRE(std::to_address(it) == &val);
  }
}

TEST_CASE("Increment and decrement", "[optional]")
{
  SECTION("prefix increment (value)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*++it == secondString);
  }

  SECTION("prefix increment (address)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(&++it == &it);
  }

  SECTION("postfix increment (returned value)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*it++ == firstString);
  }

  SECTION("postfix increment (advanced value)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it++;
    REQUIRE(*it == secondString);
  }

  SECTION("prefix decrement (value)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
    REQUIRE(*--it == secondString);
  }

  SECTION("prefix decrement (address)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
    REQUIRE(&--it == &it);
  }

  SECTION("postfix decrement (returned value)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
    REQUIRE(*it-- == thirdString);
  }

  SECTION("postfix decrement (decremented value)")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
    it--;
    REQUIRE(*it == secondString);
  }

  SECTION("multiple increments")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    ++it;
    ++it;
    REQUIRE(*it == thirdString);
  }

  SECTION("multiple decrements")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
    --it;
    --it;
    REQUIRE(*it == firstString);
  }
}

TEST_CASE("Arithmetic and compound assignment", "[optional]")
{
  SECTION("operator+= with positive offset (value)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it += twoOffset;
    REQUIRE(*it == thirdString);
  }

  SECTION("operator+= with positive offset (address)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(&(it += twoOffset) == &it);
  }

  SECTION("operator+= with zero offset")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it += zeroOffset;
    REQUIRE(*it == firstString);
  }

  SECTION("operator+= with negative offset")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it += twoOffset;
    it += minusOneOffset;
    REQUIRE(*it == secondString);
  }

  SECTION("operator-= with positive offset (value)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it += threeOffset;
    it -= twoOffset;
    REQUIRE(*it == secondString);
  }

  SECTION("operator-= with positive offset (address)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data() + threeOffset};
    REQUIRE(&(it -= twoOffset) == &it);
  }

  SECTION("operator-= with zero offset")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it += twoOffset;
    it -= zeroOffset;
    REQUIRE(*it == thirdString);
  }

  SECTION("operator-= with negative offset")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it += oneOffset;
    it -= minusTwoOffset;
    REQUIRE(*it == fourthString);
  }

  SECTION("operator+ (iterator + offset)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*(it + twoOffset) == thirdString);
  }

  SECTION("operator+ (iterator + zero offset)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*(it + zeroOffset) == firstString);
  }

  SECTION("operator+ (iterator + negative offset)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*((it + twoOffset) + minusOneOffset) == secondString);
  }

  SECTION("operator+ (offset + iterator)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*(twoOffset + it) == thirdString);
  }

  SECTION("operator+ (zero offset + iterator)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*(zeroOffset + it) == firstString);
  }

  SECTION("operator+ (negative offset + iterator)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*(minusOneOffset + (it + twoOffset)) == secondString);
  }

  SECTION("operator- (iterator - offset)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*((it + threeOffset) - twoOffset) == secondString);
  }

  SECTION("operator- (iterator - zero offset)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*((it + twoOffset) - zeroOffset) == thirdString);
  }

  SECTION("operator- (iterator - negative offset)")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(*((it + oneOffset) - minusTwoOffset) == fourthString);
  }

  SECTION("operator- (iterator - iterator) positive difference")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE((it + threeOffset) - it == threeOffset);
  }

  SECTION("operator- (iterator - iterator) negative difference")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(it - (it + threeOffset) == -threeOffset);
  }

  SECTION("operator- (iterator - iterator) zero difference")
  {
    std::array arr{firstString, secondString, thirdString, fourthString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(it - it == zeroOffset);
  }
}

TEST_CASE("Subscript", "[optional]")
{
  SECTION("operator[] index 0")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(it[zeroOffset] == firstString);
  }

  SECTION("operator[] index 1")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(it[oneOffset] == secondString);
  }

  SECTION("operator[] index 2")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE(it[twoOffset] == thirdString);
  }

  SECTION("operator[] modification")
  {
    std::array arr{firstString, secondString, thirdString};
    frt::detail::optional_::contiguous_iterator it{arr.data()};
    it[oneOffset] = modifiedStringValue;
    REQUIRE(it[oneOffset] == modifiedStringValue);
  }

  SECTION("operator[] negative index -1")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE((it + twoOffset)[minusOneOffset] == secondString);
  }

  SECTION("operator[] negative index -2")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator it{arr.data()};
    REQUIRE((it + twoOffset)[minusTwoOffset] == firstString);
  }

  SECTION("operator[] on const iterator")
  {
    const std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator constIt{arr.data()};
    REQUIRE(constIt[oneOffset] == secondString);
  }
}

TEST_CASE("Comparison", "[optional]")
{
  std::array arr{firstString, secondString, thirdString};
  const frt::detail::optional_::contiguous_iterator it1{arr.data()};
  const frt::detail::optional_::contiguous_iterator it2{arr.data()};
  const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
  const frt::detail::optional_::contiguous_iterator<std::string> defaultIt1;
  const frt::detail::optional_::contiguous_iterator<std::string> defaultIt2;

  SECTION("operator== same iterator") { REQUIRE(it1 == it1); }

  SECTION("operator== iterators to same element") { REQUIRE(it1 == it2); }

  SECTION("operator== iterators to different elements") { REQUIRE_FALSE(it1 == it3); }

  SECTION("operator== both default constructed") { REQUIRE(defaultIt1 == defaultIt2); }

  SECTION("operator== default constructed and non-null") { REQUIRE_FALSE(defaultIt1 == it1); }

  SECTION("operator!= same iterator") { REQUIRE_FALSE(it1 != it1); }

  SECTION("operator!= iterators to different elements") { REQUIRE(it1 != it3); }

  SECTION("operator!= default constructed and non-null") { REQUIRE(defaultIt1 != it1); }

  SECTION("operator< true when pointing before") { REQUIRE(it1 < it3); }

  SECTION("operator< false when pointing after") { REQUIRE_FALSE(it3 < it1); }

  SECTION("operator< false when pointing to same") { REQUIRE_FALSE(it1 < it1); }

  SECTION("operator<= true when pointing before") { REQUIRE(it1 <= it3); }

  SECTION("operator<= true when pointing to same") { REQUIRE(it1 <= it1); }

  SECTION("operator<= false when pointing after") { REQUIRE_FALSE(it3 <= it1); }

  SECTION("operator> true when pointing after") { REQUIRE(it3 > it1); }

  SECTION("operator> false when pointing before") { REQUIRE_FALSE(it1 > it3); }

  SECTION("operator> false when pointing to same") { REQUIRE_FALSE(it1 > it1); }

  SECTION("operator>= true when pointing after") { REQUIRE(it3 >= it1); }

  SECTION("operator>= true when pointing to same") { REQUIRE(it1 >= it1); }

  SECTION("operator>= false when pointing before") { REQUIRE_FALSE(it1 >= it3); }

  SECTION("operator<=> three-way equal") { REQUIRE((it1 <=> it2) == std::strong_ordering::equal); }

  SECTION("operator<=> three-way less") { REQUIRE((it1 <=> it3) == std::strong_ordering::less); }

  SECTION("operator<=> three-way greater") { REQUIRE((it3 <=> it1) == std::strong_ordering::greater); }
}

TEST_CASE("Standard algorithms and ranges", "[optional]")
{
  SECTION("std::ranges::distance")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator begin{arr.data()};
    const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
    REQUIRE(std::ranges::distance(begin, end) == expectedRangeSize);
  }

  SECTION("std::ranges::find found")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator begin{arr.data()};
    const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
    REQUIRE(*std::ranges::find(begin, end, secondString) == secondString);
  }

  SECTION("std::ranges::find not found")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator begin{arr.data()};
    const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
    REQUIRE(std::ranges::find(begin, end, nonExistentString) == end);
  }

  SECTION("std::ranges::for_each")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator begin{arr.data()};
    const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
    std::size_t count = 0;
    std::ranges::for_each(begin, end, [&count](const std::string &) { ++count; });
    REQUIRE(count == expectedRangeSize);
  }

  SECTION("std::ranges::subrange size")
  {
    std::array arr{firstString, secondString, thirdString};
    const frt::detail::optional_::contiguous_iterator begin{arr.data()};
    const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
    const std::ranges::subrange range{begin, end};
    REQUIRE(range.size() == expectedRangeSize);
  }

  SECTION("std::ranges::copy")
  {
    const std::array src{firstString, secondString, thirdString};
    std::array<std::string, 3> dst;
    const frt::detail::optional_::contiguous_iterator begin{src.data()};
    const frt::detail::optional_::contiguous_iterator end{src.data() + src.size()};
    const frt::detail::optional_::contiguous_iterator out{dst.data()};
    std::ranges::copy(begin, end, out);
    REQUIRE(dst == src);
  }
}
