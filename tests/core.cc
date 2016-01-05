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
    auto o = D(_name = "John", _age = 12);

    static_assert(!std::is_same<decltype(o)::symbol_to_member_type<_name_t>(), member_not_found>::value, "");
    static_assert(std::is_same<decltype(o)::symbol_to_member_type<_children_t>, member_not_found>::value, "");

    assert(o.name == "John");
    assert(o.age == 12);

    assert(o[_name] == "John");
    assert(o[_age] == 12);

    assert(o.has(_name));
    assert(o.has(_age));

    assert(!o.has(_city));

    auto p = D(_name = "John2", _age = 123);
    assert(p.name == "John2");
    assert(p.age == 123);

    assert(p[_name] == "John2");
    assert(p[_age] == 123);

    assert(p.has(_name));
    assert(p.has(_age));

    assert(!p.has(_city));

    assert(p.get(_city, "Paris") == std::string("Paris"));
    assert(p.get(_name, "Mary") == "John2");

    // Assignement
    o = p;
    assert(o.name == "John2");
    assert(o.age == 123);
  }

  { // Arrays
    auto o = D(_name = "John", _children = { "a", "b", "c"});
    assert(o.children[2] == "c");
  }

  { // Arrays of iod.
    auto o = D(_name = "John",
                 _children = { 
                   D(_name = "a"),
                   D(_name = "b"),
                   D(_name = "c"),
                     });
    assert(o.children[2].name == "c");
  }

  { // Cat
    auto a = D(_name = "John");
    auto b = D(_age = 42);

    auto c = iod::cat(a, b);
    decltype(a) d;

    assert(c.name == "John");
    assert(c.age == 42);
  }

  { // Intersect
    auto a = D(_age = 42);
    auto b = D(_age = 41);
    auto i = iod::intersect(a, b);
    assert(i.age == 42);
    assert(i.size() == 1);
  }
  
  { // Default constructor when values are default constructible.
    auto a = D(_name = "John");
    decltype(a) d;
  }

  { // Attribute value without default constructor should also work.
    D(_name = [] () {});
    // But cannot be default constructed.
    // Do not compile: decltype(a) d;
  }

  {
    static_assert(is_sio<decltype(D(_name = std::string()))>::value, "is sio failled");
    static_assert(is_sio<decltype(D(_name = std::string()))&>::value, "is sio failled");
    static_assert(!is_sio<int>::value, "is sio failled");
  }
}
