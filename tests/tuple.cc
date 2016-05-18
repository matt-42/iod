
#include <tuple>

#include <iostream>
#include <cassert>
#include <iod/apply.hh>

#include <iod/tuple_utils.hh>


int main()
{
  using namespace iod;

  std::tuple<int, float, int, double> t;

  tuple_remove_elements_t<decltype(t), int, float> t2;

  assert(std::tuple_size<decltype(t2)>::value == 1);

  std::tuple<int, float, int, double> t3{1, 2, 3, 4};
  auto t4 = tuple_filter<std::is_floating_point>(t3);
  std::cout << std::tuple_size<decltype(t4)>::value << std::endl;
  assert(std::get<0>(t4) == 2);
  assert(std::get<1>(t4) == 4);
  assert(std::tuple_size<decltype(t4)>::value == 2);
}
