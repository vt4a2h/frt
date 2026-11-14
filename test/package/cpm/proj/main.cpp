#include <frt/all.hpp>

#include <vector>

int main()
{
  frt::either<int, std::string> e1{frt::in_place_right, "***"};
  frt::either<int, std::string> e2{frt::in_place_right, "***"};

  std::vector<frt::either<int, std::string>> vec;
  vec.push_back(std::move(e1));
  vec.push_back(std::move(e2));

  auto eitherOfVec = frt::algorithm::sequence(vec);

  return eitherOfVec.is_right() ? EXIT_SUCCESS : EXIT_FAILURE;
}
