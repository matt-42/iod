#include <iostream>
#include <cassert>
#include <iod/sio.hh>
#include <iod/symbol.hh>
#include <iod/symbols.hh>

#include "symbols.hh"

template <typename F, typename... T>
void apply_variable(F f, const iod::sio<T...>& o)
{
  return f(*static_cast<const T*>(&o)...);
}

int main()
{
  using namespace iod;
  using namespace s;
  {
    auto o = D(_Name = "John", _Age = 12);
    assert(o.name == "John");
    assert(o.age == 12);

    assert(o[_Name] == "John");
    assert(o[_Age] == 12);

    assert(o.has(_Name));
    assert(o.has(_Age));

    assert(!o.has(_City));

    auto p = D(_Name("John2"), _Age(123));
    assert(p.name == "John2");
    assert(p.age == 123);

    assert(p[_Name] == "John2");
    assert(p[_Age] == 123);

    assert(p.has(_Name));
    assert(p.has(_Age));

    assert(!p.has(_City));

    assert(p.get(_City, "Paris") == std::string("Paris"));
    assert(p.get(_Name, "Mary") == "John2");

    // Assignement
    o = p;
    assert(o.name == "John2");
    assert(o.age == 123);
  }

  { // Arrays
    auto o = D(_Name = "John", _Children = { "a", "b", "c"});
    assert(o.children[2] == "c");
  }

  { // Arrays of iod.
    auto o = D(_Name = "John",
                 _Children = { 
                   D(_Name = "a"),
                   D(_Name = "b"),
                   D(_Name = "c"),
                     });
    assert(o.children[2].name == "c");
  }

  { // Cat
    auto a = D(_Name = "John");
    auto b = D(_Age = 42);

    auto c = iod::cat(a, b);
    decltype(a) d;

    assert(c.name == "John");
    assert(c.age == 42);
  }

  { // Default constructor when values are default constructible.
    auto a = D(_Name = "John");
    decltype(a) d;
  }

  { // Attribute value without default constructor should also work.
    D(_Name = [] () {});
    // But cannot be default constructed.
    // Do not compile: decltype(a) d;
  }
}
