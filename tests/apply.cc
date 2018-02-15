#include <iostream>
#include <cassert>
#include <iod/apply.hh>

#include <iod/symbol.hh>
#include "symbols.hh"


int main()
{
  using namespace iod;

  using s::_age;
  using s::_name;

  auto plus = [] (auto a, auto b) { return a + b; };

  int a = 1;
  int b = 2;
  assert(iod::apply(a, [] (int x) { return x; }) == 1);
  assert(iod::apply(a, b, [] (int a, int b) { return a + b; }) == 3);
  assert(iod::apply(a, b, plus) == 3);
  
  auto t = std::make_tuple(1, 2, 3);
  const auto u = std::make_tuple(1, 2, 3);
  iod::apply(t, [] (int, int, int s) { std::cout << s << std::endl; });
  iod::apply(u, 32, [] (int, int, int s, int p) { std::cout << s << "-" << p << std::endl; });
  
  auto o1 = iod::D(_name = "xxx", _age = 12);
  const auto o2 = iod::D(_name = "xxx", _age = 12);
  iod::apply(o1, [] (const char* s, int&) { std::cout << s << std::endl; });
  iod::apply(o2, [] (const char* s, const int&) { std::cout << s << std::endl; });

  iod::proxy_apply(t, [] (int x) { return x + 2; }, [] (int, int, int z) { std::cout << z << std::endl; });

  assert(iod::apply(std::string("A"), std::string("B"), plus) == "AB");

  {
    // Forward
    auto t = std::make_tuple(1, 2, 3);
    auto fun = [] (auto t) { return std::get<0>(t); };
    iod::apply(forward(t), fun);
  }
}
