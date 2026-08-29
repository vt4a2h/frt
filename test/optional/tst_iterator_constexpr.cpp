#include <catch2/catch_all.hpp>

#include <algorithm>
#include <array>
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

constexpr int expectedIntValue = 42;
constexpr int modifiedIntValue = 84;

constexpr auto makeExpectedStringValue()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}
constexpr auto makeModifiedStringValue()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}

constexpr auto makeFirstString()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}
constexpr auto makeSecondString()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}
constexpr auto makeThirdString()
{
  return std::string{
      "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque "
      "corrupti quos dolores"};
}
constexpr auto makeFourthString()
{
  return std::string{
      "Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat "
      "facere possimus"};
}
constexpr auto makeNonExistentString()
{
  return std::string{
      "Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur"};
}

constexpr auto makeExpectedVectorValue() { return std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; }
constexpr auto makeModifiedVectorValue() { return std::vector{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}; }

constexpr int initialWidth = 100;
constexpr int initialHeight = 200;
constexpr int modifiedWidth = 300;

constexpr std::ptrdiff_t zeroOffset = 0;
constexpr std::ptrdiff_t oneOffset = 1;
constexpr std::ptrdiff_t twoOffset = 2;
constexpr std::ptrdiff_t threeOffset = 3;
constexpr std::ptrdiff_t minusOneOffset = -1;
constexpr std::ptrdiff_t minusTwoOffset = -2;
constexpr std::size_t expectedRangeSize = 3;

} // namespace

TEST_CASE("Constructors and assignment (constexpr)", "[optional][constexpr]")
{
  SECTION("default constructor")
  {
    constexpr auto valid = [] {
      const frt::detail::optional_::contiguous_iterator<int> it;
      const frt::detail::optional_::contiguous_iterator<int> other;
      return it == other;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("pointer constructor with non-null pointer (int)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("pointer constructor with null pointer")
  {
    constexpr auto valid = [] {
      const frt::detail::optional_::contiguous_iterator<int> it{nullptr};
      const frt::detail::optional_::contiguous_iterator<int> other;
      return it == other;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (int)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (string)")
  {
    constexpr auto valid = [] {
      std::string val = makeExpectedStringValue();
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == makeExpectedStringValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (vector)")
  {
    constexpr auto valid = [] {
      std::vector val = makeExpectedVectorValue();
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == makeExpectedVectorValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("CTAD (const int)")
  {
    constexpr auto valid = [] {
      const int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (value)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      const frt::detail::optional_::contiguous_iterator other{it};
      return *other == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy constructor (equality)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      const frt::detail::optional_::contiguous_iterator other{it};
      return other == it;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("move constructor (value)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      frt::detail::optional_::contiguous_iterator it{&val};
      const frt::detail::optional_::contiguous_iterator other{std::move(it)};
      return *other == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (value)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      frt::detail::optional_::contiguous_iterator<int> other;
      other = it;
      return *other == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("copy assignment (equality)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      frt::detail::optional_::contiguous_iterator<int> other;
      other = it;
      return other == it;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("move assignment (value)")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      frt::detail::optional_::contiguous_iterator it{&val};
      frt::detail::optional_::contiguous_iterator<int> other;
      other = std::move(it);
      return *other == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Dereference and member access (constexpr)", "[optional][constexpr]")
{
  SECTION("operator* for int")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* for string")
  {
    constexpr auto valid = [] {
      std::string val = makeExpectedStringValue();
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == makeExpectedStringValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* for vector")
  {
    constexpr auto valid = [] {
      std::vector val = makeExpectedVectorValue();
      const frt::detail::optional_::contiguous_iterator it{&val};
      return *it == makeExpectedVectorValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* modification for int")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      frt::detail::optional_::contiguous_iterator it{&val};
      *it = modifiedIntValue;
      return *it == modifiedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* modification for string")
  {
    constexpr auto valid = [] {
      std::string val = makeExpectedStringValue();
      frt::detail::optional_::contiguous_iterator it{&val};
      *it = makeModifiedStringValue();
      return *it == makeModifiedStringValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* modification for vector")
  {
    constexpr auto valid = [] {
      std::vector val = makeExpectedVectorValue();
      frt::detail::optional_::contiguous_iterator it{&val};
      *it = makeModifiedVectorValue();
      return *it == makeModifiedVectorValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* for const int")
  {
    constexpr auto valid = [] {
      const int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator constIt{&val};
      return *constIt == expectedIntValue;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator* for const string")
  {
    constexpr auto valid = [] {
      const std::string val = makeExpectedStringValue();
      const frt::detail::optional_::contiguous_iterator constIt{&val};
      return *constIt == makeExpectedStringValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-> struct member access")
  {
    constexpr auto valid = [] {
      test::helpers::Widget widget{initialWidth, initialHeight};
      const frt::detail::optional_::contiguous_iterator it{&widget};
      return it->width == initialWidth;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-> struct member modification")
  {
    constexpr auto valid = [] {
      test::helpers::Widget widget{initialWidth, initialHeight};
      frt::detail::optional_::contiguous_iterator it{&widget};
      it->width = modifiedWidth;
      return it->width == modifiedWidth;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-> string member function")
  {
    constexpr auto valid = [] {
      std::string val = makeExpectedStringValue();
      const frt::detail::optional_::contiguous_iterator it{&val};
      return it->length() == makeExpectedStringValue().length();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-> vector member function")
  {
    constexpr auto valid = [] {
      std::vector val = makeExpectedVectorValue();
      const frt::detail::optional_::contiguous_iterator it{&val};
      return it->size() == makeExpectedVectorValue().size();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("std::to_address")
  {
    constexpr auto valid = [] {
      int val = expectedIntValue;
      const frt::detail::optional_::contiguous_iterator it{&val};
      return std::to_address(it) == &val;
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Increment and decrement (constexpr)", "[optional][constexpr]")
{
  SECTION("prefix increment (value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *++it == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("prefix increment (address)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      return &++it == &it;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("postfix increment (returned value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *it++ == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("postfix increment (advanced value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it++;
      return *it == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("prefix decrement (value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
      return *--it == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("prefix decrement (address)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
      return &--it == &it;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("postfix decrement (returned value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
      return *it-- == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("postfix decrement (decremented value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
      it--;
      return *it == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("multiple increments")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      ++it;
      ++it;
      return *it == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("multiple decrements")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data() + twoOffset};
      --it;
      --it;
      return *it == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Arithmetic and compound assignment (constexpr)", "[optional][constexpr]")
{
  SECTION("operator+= with positive offset (value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it += twoOffset;
      return *it == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+= with positive offset (address)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      return &(it += twoOffset) == &it;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+= with zero offset")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it += zeroOffset;
      return *it == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+= with negative offset")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it += twoOffset;
      it += minusOneOffset;
      return *it == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-= with positive offset (value)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it += threeOffset;
      it -= twoOffset;
      return *it == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-= with positive offset (address)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data() + threeOffset};
      return &(it -= twoOffset) == &it;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-= with zero offset")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it += twoOffset;
      it -= zeroOffset;
      return *it == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator-= with negative offset")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it += oneOffset;
      it -= minusTwoOffset;
      return *it == makeFourthString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+ (iterator + offset)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *(it + twoOffset) == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+ (iterator + zero offset)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *(it + zeroOffset) == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+ (iterator + negative offset)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *((it + twoOffset) + minusOneOffset) == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+ (offset + iterator)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *(twoOffset + it) == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+ (zero offset + iterator)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *(zeroOffset + it) == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator+ (negative offset + iterator)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *(minusOneOffset + (it + twoOffset)) == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator- (iterator - offset)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *((it + threeOffset) - twoOffset) == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator- (iterator - zero offset)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *((it + twoOffset) - zeroOffset) == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator- (iterator - negative offset)")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return *((it + oneOffset) - minusTwoOffset) == makeFourthString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator- (iterator - iterator) positive difference")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return (it + threeOffset) - it == threeOffset;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator- (iterator - iterator) negative difference")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return it - (it + threeOffset) == -threeOffset;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator- (iterator - iterator) zero difference")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString(), makeFourthString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return it - it == zeroOffset;
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Subscript (constexpr)", "[optional][constexpr]")
{
  SECTION("operator[] index 0")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return it[zeroOffset] == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator[] index 1")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return it[oneOffset] == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator[] index 2")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return it[twoOffset] == makeThirdString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator[] modification")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      frt::detail::optional_::contiguous_iterator it{arr.data()};
      it[oneOffset] = makeModifiedStringValue();
      return it[oneOffset] == makeModifiedStringValue();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator[] negative index -1")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return (it + twoOffset)[minusOneOffset] == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator[] negative index -2")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it{arr.data()};
      return (it + twoOffset)[minusTwoOffset] == makeFirstString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator[] on const iterator")
  {
    constexpr auto valid = [] {
      const std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator constIt{arr.data()};
      return constIt[oneOffset] == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Comparison (constexpr)", "[optional][constexpr]")
{
  SECTION("operator== same iterator")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      return it1 == it1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator== iterators to same element")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it2{arr.data()};
      return it1 == it2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator== iterators to different elements")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it1 == it3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator== both default constructed")
  {
    constexpr auto valid = [] {
      const frt::detail::optional_::contiguous_iterator<std::string> defaultIt1;
      const frt::detail::optional_::contiguous_iterator<std::string> defaultIt2;
      return defaultIt1 == defaultIt2;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator== default constructed and non-null")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator<std::string> defaultIt1;
      return defaultIt1 == it1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator!= same iterator")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      return it1 != it1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator!= iterators to different elements")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it1 != it3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator!= default constructed and non-null")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator<std::string> defaultIt1;
      return defaultIt1 != it1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator< true when pointing before")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it1 < it3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator< false when pointing after")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it3 < it1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator< false when pointing to same")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      return it1 < it1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator<= true when pointing before")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it1 <= it3;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator<= true when pointing to same")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      return it1 <= it1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator<= false when pointing after")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it3 <= it1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator> true when pointing after")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it3 > it1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator> false when pointing before")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it1 > it3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator> false when pointing to same")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      return it1 > it1;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator>= true when pointing after")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it3 >= it1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator>= true when pointing to same")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      return it1 >= it1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator>= false when pointing before")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return it1 >= it3;
    }();
    STATIC_REQUIRE_FALSE(valid);
  }

  SECTION("operator<=> three-way equal")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it2{arr.data()};
      return (it1 <=> it2) == std::strong_ordering::equal;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator<=> three-way less")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return (it1 <=> it3) == std::strong_ordering::less;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("operator<=> three-way greater")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator it1{arr.data()};
      const frt::detail::optional_::contiguous_iterator it3{arr.data() + twoOffset};
      return (it3 <=> it1) == std::strong_ordering::greater;
    }();
    STATIC_REQUIRE(valid);
  }
}

TEST_CASE("Standard algorithms and ranges (constexpr)", "[optional][constexpr]")
{
  SECTION("std::ranges::distance")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator begin{arr.data()};
      const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
      return std::ranges::distance(begin, end) == expectedRangeSize;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("std::ranges::find found")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator begin{arr.data()};
      const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
      return *std::ranges::find(begin, end, makeSecondString()) == makeSecondString();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("std::ranges::find not found")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator begin{arr.data()};
      const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
      return std::ranges::find(begin, end, makeNonExistentString()) == end;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("std::ranges::for_each")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator begin{arr.data()};
      const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
      std::size_t count = 0;
      std::ranges::for_each(begin, end, [&count](const std::string &) { ++count; });
      return count == expectedRangeSize;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("std::ranges::subrange size")
  {
    constexpr auto valid = [] {
      std::array arr{makeFirstString(), makeSecondString(), makeThirdString()};
      const frt::detail::optional_::contiguous_iterator begin{arr.data()};
      const frt::detail::optional_::contiguous_iterator end{arr.data() + arr.size()};
      const std::ranges::subrange range{begin, end};
      return range.size() == expectedRangeSize;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("std::ranges::copy")
  {
    constexpr auto valid = [] {
      const std::array src{makeFirstString(), makeSecondString(), makeThirdString()};
      std::array<std::string, 3> dst;
      const frt::detail::optional_::contiguous_iterator begin{src.data()};
      const frt::detail::optional_::contiguous_iterator end{src.data() + src.size()};
      const frt::detail::optional_::contiguous_iterator out{dst.data()};
      std::ranges::copy(begin, end, out);
      return dst == src;
    }();
    STATIC_REQUIRE(valid);
  }
}
