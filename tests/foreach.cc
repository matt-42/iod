#include <cassert>
#include <tuple>
#include <iostream>
#include <iod/foreach.hh>

#include <iod/symbol.hh>
#include "symbols.hh"

int main()
{
  using namespace std::string_literals;
  
  using s::_age;
  using s::_name;

  auto x = iod::D(_name = "xxx"s, _age = 12);
  auto t = std::make_tuple(1, 2, 3);
  auto u = std::make_tuple(1, 2, 4);

  iod::foreach(t) | [] (auto& x) { x += 1; };

  assert(t == std::make_tuple(2, 3, 4));

  auto t2 = iod::foreach(u) | [] (auto& x) { return x + 1; };

  //std::cout << std::get<0>(t2_) << std::endl;

  assert(t2 == std::make_tuple(2, 3, 5));

  auto t3 = iod::foreach(t, u) | [] (auto& a, auto& b) { return a + b; };
  assert(std::get<0>(t3) == std::get<0>(u) + std::get<0>(t));
  assert(std::get<1>(t3) == std::get<1>(u) + std::get<1>(t));
  assert(std::get<2>(t3) == std::get<2>(u) + std::get<2>(t));

  auto o1 = iod::D(_name = "xxx"s, _age = 12);
  const auto o2 = iod::D(_name = "xxx"s, _age = 12);

  auto o3 = iod::foreach(o1, o2) | [] (auto& x, auto& y) { return x.symbol() = x.value() + y.value(); };

  assert(o3.name == o1.name + o2.name);
  assert(o3.age == o1.age + o2.age);

}
