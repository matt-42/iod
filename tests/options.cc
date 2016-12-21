#include <cassert>
#include <iod/options.hh>

#include "symbols.hh"

using namespace s;
using namespace iod;

template <typename... O>
void function(O&&... opts)
{
  auto options = iod::options(opts...);

  assert(options.age.x == 42);
  assert(options.opt1.opt2.opt3.x == 42);

}

// Non copyable
struct A
{
  A() : x(42) {};
  A(const A& e) = delete;
  A(const A&& e) : x(e.x) {};
  A& operator=(const A& e) = delete;
  A& operator=(const A&& e) { x = e.x; return *this; };

  int x;
};

int main()
{
  A a;

  function(_age = a, _opt1(_opt2(_opt3 = a)));
  function(_age = A(), _opt1(_opt2(_opt3 = a)));
}
