#include "test/helpers/types.hpp"

#include <catch2/catch_all.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <frt/optional.hpp>

namespace
{

using Opt = frt::optional<std::string>;

inline const std::string value1 =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore "
    "magna aliqua";
inline const std::string value2 =
    "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem "
    "aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo.";

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

TEST_CASE("Modifiers", "[optional]")
{
  SECTION("Swap both with value (member swap - first object gets second value)")
  {
    Opt opt1{value1};
    Opt opt2{value2};
    opt1.swap(opt2);
    REQUIRE(opt1.value() == value2);
  }

  SECTION("Swap both with value (member swap - second object gets first value)")
  {
    Opt opt1{value1};
    Opt opt2{value2};
    opt1.swap(opt2);
    REQUIRE(opt2.value() == value1);
  }

  SECTION("Swap both with value (friend swap - first object gets second value)")
  {
    Opt opt1{value1};
    Opt opt2{value2};
    using std::swap;
    swap(opt1, opt2);
    REQUIRE(opt1.value() == value2);
  }

  SECTION("Swap both with value (friend swap - second object gets first value)")
  {
    Opt opt1{value1};
    Opt opt2{value2};
    using std::swap;
    swap(opt1, opt2);
    REQUIRE(opt2.value() == value1);
  }

  SECTION("Swap value and nullopt (member swap - first object becomes empty)")
  {
    Opt opt1{value1};
    Opt opt2;
    opt1.swap(opt2);
    REQUIRE_FALSE(opt1.has_value());
  }

  SECTION("Swap value and nullopt (member swap - second object gets first value)")
  {
    Opt opt1{value1};
    Opt opt2;
    opt1.swap(opt2);
    REQUIRE(opt2.value() == value1);
  }

  SECTION("Swap value and nullopt (friend swap - first object becomes empty)")
  {
    Opt opt1{value1};
    Opt opt2;
    using std::swap;
    swap(opt1, opt2);
    REQUIRE_FALSE(opt1.has_value());
  }

  SECTION("Swap value and nullopt (friend swap - second object gets first value)")
  {
    Opt opt1{value1};
    Opt opt2;
    using std::swap;
    swap(opt1, opt2);
    REQUIRE(opt2.value() == value1);
  }

  SECTION("Swap nullopt and value (member swap - first object gets second value)")
  {
    Opt opt1;
    Opt opt2{value2};
    opt1.swap(opt2);
    REQUIRE(opt1.value() == value2);
  }

  SECTION("Swap nullopt and value (member swap - second object becomes empty)")
  {
    Opt opt1;
    Opt opt2{value2};
    opt1.swap(opt2);
    REQUIRE_FALSE(opt2.has_value());
  }

  SECTION("Swap nullopt and value (friend swap - first object gets second value)")
  {
    Opt opt1;
    Opt opt2{value2};
    using std::swap;
    swap(opt1, opt2);
    REQUIRE(opt1.value() == value2);
  }

  SECTION("Swap nullopt and value (friend swap - second object becomes empty)")
  {
    Opt opt1;
    Opt opt2{value2};
    using std::swap;
    swap(opt1, opt2);
    REQUIRE_FALSE(opt2.has_value());
  }

  SECTION("Swap both nullopt (member swap)")
  {
    Opt opt1;
    Opt opt2;
    opt1.swap(opt2);
    REQUIRE_FALSE(opt1.has_value());
  }

  SECTION("Swap both nullopt (friend swap)")
  {
    Opt opt1;
    Opt opt2;
    using std::swap;
    swap(opt1, opt2);
    REQUIRE_FALSE(opt1.has_value());
  }

  SECTION("reset on populated optional")
  {
    Opt opt{value1};
    opt.reset();
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("reset on empty optional")
  {
    Opt opt;
    opt.reset();
    REQUIRE_FALSE(opt.has_value());
  }

  SECTION("reset (destructor invoked)")
  {
    int destructCount = 0;
    {
      frt::optional<DestructorTracker> opt{frt::in_place_opt, &destructCount};
      opt.reset();
    }
    REQUIRE(destructCount == 1);
  }

  SECTION("Emplace single arg")
  {
    frt::optional<std::string> opt;
    opt.emplace(value1);
    REQUIRE(opt.value() == value1);
  }

  SECTION("Emplace multiple args")
  {
    frt::optional<test::helpers::TestArgsIl> opt;
    opt.emplace(42, 3.14);
    const auto expected = test::helpers::TestArgsIl{42, 3.14};
    REQUIRE(opt.value() == expected);
  }

  SECTION("Emplace with initializer list")
  {
    const auto some = {'s', 'o', 'm', 'e'};
    frt::optional<std::string> opt;
    opt.emplace(some);
    const auto expected = std::string{"some"};
    REQUIRE(opt.value() == expected);
  }

  SECTION("Emplace with initializer list and extra args")
  {
    const auto il = {std::string{"Lorem ipsum"}, std::string{"dolor sit amet"}};
    frt::optional<test::helpers::TestArgsIl> opt;
    opt.emplace(il, 42, 3.14);
    const auto expected = test::helpers::TestArgsIl{il, 42, 3.14};
    REQUIRE(opt.value() == expected);
  }

  SECTION("Emplace replaces existing value")
  {
    frt::optional<std::string> opt{value1};
    opt.emplace(value2);
    REQUIRE(opt.value() == value2);
  }

  SECTION("Emplace returns reference")
  {
    frt::optional<std::string> opt;
    auto &ref = opt.emplace(value1);
    ref.append("!");
    const auto expected = std::string{value1}.append("!");
    REQUIRE(opt.value() == expected);
  }
}
