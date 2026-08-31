#include "test/helpers/types.hpp"

#include <catch2/catch_all.hpp>

#include <string>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::string>;

constexpr auto makeValue1()
{
  return std::string{
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
      "magna aliqua"};
}
constexpr auto makeValue2()
{
  return std::string{
      "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
      "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo."};
}

struct DestructorTracker
{
  int *destructCount_{};
  constexpr ~DestructorTracker()
  {
    if (destructCount_) {
      ++(*destructCount_);
    }
  }
};

} // namespace

TEST_CASE("Modifiers constexpr", "[optional][constexpr]")
{
  SECTION("Swap both with value (member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2{makeValue2()};
      opt1.swap(opt2);
      return opt1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap both with value (member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2{makeValue2()};
      opt1.swap(opt2);
      return opt2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap both with value (friend swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2{makeValue2()};
      using std::swap;
      swap(opt1, opt2);
      return opt1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap both with value (friend swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2{makeValue2()};
      using std::swap;
      swap(opt1, opt2);
      return opt2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap value and nullopt (member swap - first object becomes empty)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2;
      opt1.swap(opt2);
      return !opt1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap value and nullopt (member swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2;
      opt1.swap(opt2);
      return opt2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap value and nullopt (friend swap - first object becomes empty)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2;
      using std::swap;
      swap(opt1, opt2);
      return !opt1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap value and nullopt (friend swap - second object gets first value)")
  {
    constexpr auto valid = [] {
      Opt opt1{makeValue1()};
      Opt opt2;
      using std::swap;
      swap(opt1, opt2);
      return opt2.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap nullopt and value (member swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      Opt opt1;
      Opt opt2{makeValue2()};
      opt1.swap(opt2);
      return opt1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap nullopt and value (member swap - second object becomes empty)")
  {
    constexpr auto valid = [] {
      Opt opt1;
      Opt opt2{makeValue2()};
      opt1.swap(opt2);
      return !opt2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap nullopt and value (friend swap - first object gets second value)")
  {
    constexpr auto valid = [] {
      Opt opt1;
      Opt opt2{makeValue2()};
      using std::swap;
      swap(opt1, opt2);
      return opt1.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap nullopt and value (friend swap - second object becomes empty)")
  {
    constexpr auto valid = [] {
      Opt opt1;
      Opt opt2{makeValue2()};
      using std::swap;
      swap(opt1, opt2);
      return !opt2.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap both nullopt (member swap)")
  {
    constexpr auto valid = [] {
      Opt opt1;
      Opt opt2;
      opt1.swap(opt2);
      return !opt1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Swap both nullopt (friend swap)")
  {
    constexpr auto valid = [] {
      Opt opt1;
      Opt opt2;
      using std::swap;
      swap(opt1, opt2);
      return !opt1.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("reset on populated optional")
  {
    constexpr auto valid = [] {
      Opt opt{makeValue1()};
      opt.reset();
      return !opt.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("reset on empty optional")
  {
    constexpr auto valid = [] {
      Opt opt;
      opt.reset();
      return !opt.has_value();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("reset (destructor invoked)")
  {
    constexpr auto valid = [] {
      int destructCount = 0;
      {
        frt::optional<DestructorTracker> opt{frt::in_place_opt, &destructCount};
        opt.reset();
      }
      return destructCount == 1;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Emplace single arg")
  {
    constexpr auto valid = [] {
      frt::optional<std::string> opt;
      opt.emplace(makeValue1());
      return opt.value() == makeValue1();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Emplace multiple args")
  {
    constexpr auto valid = [] {
      frt::optional<test::helpers::TestArgsIl> opt;
      opt.emplace(42, 3.14);
      const auto expected = test::helpers::TestArgsIl{42, 3.14};
      return opt.value() == expected;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Emplace with initializer list")
  {
    constexpr auto valid = [] {
      const auto some = {'s', 'o', 'm', 'e'};
      frt::optional<std::string> opt;
      opt.emplace(some);
      const auto expected = std::string{"some"};
      return opt.value() == expected;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Emplace with initializer list and extra args")
  {
    constexpr auto valid = [] {
      const auto il = {std::string{"Lorem ipsum"}, std::string{"dolor sit amet"}};
      frt::optional<test::helpers::TestArgsIl> opt;
      opt.emplace(il, 42, 3.14);
      const auto expected = test::helpers::TestArgsIl{il, 42, 3.14};
      return opt.value() == expected;
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Emplace replaces existing value")
  {
    constexpr auto valid = [] {
      frt::optional<std::string> opt{makeValue1()};
      opt.emplace(makeValue2());
      return opt.value() == makeValue2();
    }();
    STATIC_REQUIRE(valid);
  }

  SECTION("Emplace returns reference")
  {
    constexpr auto valid = [] {
      frt::optional<std::string> opt;
      auto &ref = opt.emplace(makeValue1());
      ref.append("!");
      const auto expected = std::string{makeValue1()}.append("!");
      return opt.value() == expected;
    }();
    STATIC_REQUIRE(valid);
  }
}
