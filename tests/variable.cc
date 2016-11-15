#include <iostream>
#include <iod/sio.hh>
#include "symbols.hh"

int main()
{
  using namespace iod;
  
  auto v = D(s::_a = std::string("test"));

  auto v2 = foreach(v) | [] (auto m) {
    return m.symbol() = std::string("test2");
  };
  std::cout << v2.a << std::endl;
}
