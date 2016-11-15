#include <iostream>
#include <iod/sio.hh>
#include <iod/sio_utils.hh>
#include <iod/json.hh>
#include "symbols.hh"

int main()
{
  using namespace iod;
  auto a = D(_age = 1,
             _c = 23,
             _name = D(_a = 23));
  auto b = D(_age = 42,
             _cp = 23,
             _name = D(_b = 25));

  
  auto c = deep_merge_2_sios(a, b);
  //auto d = iod::apply(std::make_tuple(a, b), deep_merge_2_sios);
  //auto d = iod::apply(a, b, [] (auto a, auto b) { deep_merge_2_sios(a, b); });
  //auto d = iod::apply(std::make_tuple(a, b), [] (auto a, auto b) { return deep_merge_2_sios(a, b); });
  auto d = deep_merge_sios_in_tuple(std::make_tuple(a, b));
  std::cout << json_encode(c) << std::endl;
  std::cout << json_encode(d) << std::endl;

  assert(c.name.a == 23 and d.name.a == 23);
  assert(c.name.b == 25 and d.name.b == 25);
  assert(c.age == 1 and d.age == 1);
  assert(c.cp  == 23 and d.cp == 23);
}
