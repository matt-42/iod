
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
}
