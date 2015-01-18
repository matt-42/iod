#include <iostream>
#include <cassert>
#include <iod/bind_method.hh>

struct A
{
  int add(int a, float b) { return a + b; }
};

int main()
{
  using namespace iod;

  A a;
  auto f = bind_method(a, &A::add);
  assert(f(1,2) == 3);
  std::cout << f(1,2) << std::endl;
}
