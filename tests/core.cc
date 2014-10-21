#include <iostream>
#include <cassert>
#include <iod/sio.hh>
#include <iod/symbol.hh>

iod_define_symbol(name);
iod_define_symbol(age);
iod_define_symbol(city);
iod_define_symbol(children);

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
    auto o = D(name = "John", age = 12);
    assert(o.name == "John");
    assert(o.age == 12);

    assert(o[name] == "John");
    assert(o[age] == 12);

    assert(o.has(name));
    assert(o.has(age));

    assert(!o.has(city));

    auto p = D(name("John2"), age(123));
    assert(p.name == "John2");
    assert(p.age == 123);

    assert(p[name] == "John2");
    assert(p[age] == 123);

    assert(p.has(name));
    assert(p.has(age));

    assert(!p.has(city));

    assert(p.get(city, "Paris") == std::string("Paris"));
    assert(p.get(name, "Mary") == "John2");

    // Assignement
    o = p;
    assert(o.name == "John2");
    assert(o.age == 123);
  }

  { // Arrays
    auto o = D(name = "John", children = { "a", "b", "c"});
    assert(o.children[2] == "c");
  }

  { // Arrays of iod.
    auto o = D(name = "John",
                 children = { 
                   D(name = "a"),
                   D(name = "b"),
                   D(name = "c"),
                     });
    assert(o.children[2].name == "c");
  }

  { // Cat
    auto a = D(name = "John");
    auto b = D(age = 42);

    auto c = iod::cat(a, b);
    decltype(a) d;

    assert(c.name == "John");
    assert(c.age == 42);
  }

  { // Default constructor when values are default constructible.
    auto a = D(name = "John");
    decltype(a) d;
  }

  { // Attribute value without default constructor should also work.
    auto a = D(name = [] () {});
    // But cannot be default constructed.
    // Do not compile: decltype(a) d;
  }
}
