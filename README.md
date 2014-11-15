The IOD Library
==========================

The IOD library enhances C++14 meta programming with a symbol based
paradigm. It provides a compile-time a way to introspect objects and
generate code matching their data structures.

## What is a IOD symbol?

Symbols are at the core of the IOD paradigm. They add to C++ a missing
powerful feature: The way to statically store in a variable the
access to a object member, the call to a method, and the access to the
string representing the name of this variable. Here is the most simple
but powerful operator that is now possible with IOD symbols:

```c++
#include <iod/symbol.hh>

namespace s
{
  iod_define_symbol(a, _A); // Refer to members and methods a with symbol _A
  iod_define_symbol(b, _B); // Refer to members and methods b with symbol _B
  iod_define_symbol(c, _C); // Refer to members and methods b with symbol _C
}

int main() {
  using namespace s;

  auto print_member = [] (auto& obj, auto& m)
                      {
                        std::cout << "obj." << s.name()
                                  << " == " << m.member_access(obj) << std::endl;
                      };

  struct { int a; int b; int c; } obj{21, 42, 84};
  print_member(obj, _A); // Prints "obj.a == 21"
  print_member(obj, _B); // Prints "obj.b == 42"
  print_member(obj, _C); // Prints "obj.c == 84"
}
```

Without symbols (or other similar constructs), it is not possible to
write such a generic print_member function. Without, one would have to
write the three version accessing the three different members.

By convention all the symbols should be place in the namespace ::s and
they name start with _[uppercase character]. And to avoid multiple
definition, guards should be used such as in the following:

```c++
namespace s
{
  #ifndef IOD_SYMBOL__Mysymbol
  #define IOD_SYMBOL__Mysymbol
    iod_define_symbol(mysymbol, _Mysymbol);
  #endif
}
```

The script ```tools/generate_symbol_definitions.sh``` automates this
process by converting a file containing a list of symbols into a valid
C++ header containing the symbol definitions.

## Statically introspectable objects (SIO)

Statically introspectable objects features are:

  - The ability to instantiate C++ objects without having to declare any struct or class.
  - Zero cost static introspection (i.e. without overhead on execution time or memory) on those objects.

```c++
  // Define an object
  auto o = D(_Name = "John", _Age = 42, _City = "NYC");
  // Direct access to its members.
  assert(o.name == "John" && o.age == 42 && o.city == "NYC");

  // Static Introspection. It has no execution cost since these function
  // are computed at compile time.
  assert(o.has(_Name) == true);
  assert(o.has(_FirstName) == false);
  assert(o.has(_FirstName) == false);
  assert(o.size() == 3);

  // Output the structure of the object to std::cout:
  // name:John
  // age:42
  // city:NYC
  foreach(o) | [] (auto& m) { std::cout << m.symbol().name() << ":"
                                        << m.value() << std::end; }

```

## A Fast, Malloc-Free Json Parser / Encoder.

As of today, all json parsers rely dynamic data structures to
parse and store the json objects. Even if some good parser reduces
dramatically the amount of dynamic memory allocation, they still
suffer from the dynamic paradigm: A single function has to handle the
whole set of possible json objects. This comes to the absurd situation
where a program handling a small set specific json object types
have to use a json parser handling any kind of objects.

The iod library implements the opposite approach: Using
meta-programming and introspection, one tiny specialized json parser
is generated for each SIO object type so it involves no dynamic memory
allocation, and very few conditional branching. It directly fill the
object member according to their type without having to store the
object in an intermediate hash map.

This makes its performances impossible to match in other languages
such as C or Java that do not provide static introspection. The
encoder still need optimizations, and the parser is currently from
1.3x to 2.3x faster than the RapidJSON library without explicit use of
SIMD vector instructions.

The only limitation of this design is when using a very large type set
of json objects, the total code size of the generated parsers will be
bigger than a generic dynamic parser.

```c++
auto o = D(_Name = "John", _Age = 42, _City = "NYC");
auto str = json_encode(o) // => str = {"name":"John","age":42,"City":"NYC"}

decltype(o) o2;
json_decode(o2, str);
assert(o2.name == "John"); 
assert(o2.age == 42); 
assert(o2.city == "NYC");
```


## Named Optional Function Arguments

In classic C++, you would define a function taking optional arguments as :

```c++
void fun(int mandatory_arg, int optional_arg1 = 1, int optional_arg2 = 12, int optional_arg3 = 12);
```

This has to drawbacks: First, it is not practical if the user need to
set the third optional argument, and oblige him to remember the place
of each argument. SIOs are a good alternative since they solve both issues:

```c++
template <typename... O>
void fun(int mandatory_arg, const O&... opts)
{
  const auto options = D(opts...);
  int optional_arg1 = options.get(_Optional_arg1, 1); // return options.optional_arg1 or 1 if not set.
  int optional_arg2 = options.get(_Optional_arg2, 1);
  int optional_arg3 = options.get(_Optional_arg3, 1);
}

fun(1, _Optional_arg3 = 2); // Set the thirds argument and leave the two others to their default value.
```

## Foreach for tuple and SIO

While C++11 introduce range based for loops for container such as
std::vector, it does not provide a way to iterate on
tuples. ```foreach``` is a powerful primitive for processing tuples
as well as SIOs.

```c++
auto my_tuple = std::make_tuple(1, "test", 34.f);
// Prints 1 test 34.f.
foreach(my_tuple) | [] (auto& e) { std::cout << e << " "; }

auto my_sio = D(_Name = "John", _Age = 42);
// Prints name John age 42
foreach(my_sio) | [] (auto& m) { std::cout << m.symbol().name() << " " << m.value() << " "; }
```

```foreach``` also allows to iterate on several object of the same length (but different types):

```c++
auto t1 = std::make_tuple(1, "test", 34.f);
auto t2 = std::make_tuple("x", 34.f, "y");
// Prints 1 xxx test 34 34 y
foreach(t1, t2) | [] (auto& a, auto& b) { std::cout << a << " " << b << " "; }
```

Furthermore, it automatically builds a new tuple if the given function
returns a non void value:
```c++
auto t1 = std::make_tuple(1, "A", 34.f);
auto t2 = std::make_tuple(2, "B", 2);
auto t3 = foreach(t1, t2) | [] (auto& a, auto& b) { return a + b; };
// t3 == <3, "AB", 36>
```

## Apply tuples and SIOs to functions

The ```apply``` primitive map elements of a tuple or SIO to a given
function.

```c++
int fun(int x, float y) { return x + y; }

auto tuple = std::make_tuple(1, 2.f);
int res = apply(tuple, fun);
// res == 3
```


## C++ Embedded Domain Specific Languages Framework

The IOD library provides a set of tools to ease the embeding of
embedded domain specific languages (EDSL) framework. It includes an
abstract syntax tree (AST) with symbols and values as terminals. Here
is an example of a simple expression:

```c++
auto exp = _A(23) + _B;
```

This code does nothing except computing the AST type and storing the
value 23.  ```exp``` has to be evaluate to actually compute
something. IOD provides three handy primitives to traverse ASTs:
exp_map_reduce, exp_transform, exp_tranform_iterate. More
documentation will come later. In the meantime, you can check how the
Video++ library implements its image processing C++ EDSL:
https://github.com/matt-42/vpp/blob/master/vpp/core/liie.hh


## Language Integrated Queries

To demonstrate the power of the IOD framework, we embedded an
implementation of a subset of the SQL language in the C++ language.
It handles SELECT, FROM, WHERE, INNER_JOIN (with the ON criteria),
ORDER BY, GROUP BY, and aggregates such as average or sum.

Given two collection:

```c++
std::vector<decltype(D(_Name = std::string(), _Age() = int(), _City_id = int()))> persons;
std::vector<decltype(D(_Id = int(), _Name() = std::string(), _Zipcode = int()))> cities;
```

The following requests are valid:

```c++
// SELECT * from persons;
linq.select().from(persons) | [] (auto& p) { std::cout << p.name << std::endl; }
```

```c++
// SELECT myname = name from persons WHERE age > 42;
linq.select(_Myname = _Name).from(persons).where(_Age > 42) |
  [] (auto& p) { std::cout << p.myname << std::endl; }
```

```c++
// SELECT name = person.name, city = city.name
//        FROM persons as person
//        INNER JOIN cities as city
//        ON person.city_id == city.id
linq.select(_Name = _Person[_Name], _City = _City[_Name])
    .from(persons, _As(_Person))
    .inner_join(cities, _As(_City),
              _On(_City[_Cp] == _Person[_Cp])) |
  [] (auto& p) { std::cout << p.name << " lives in " << p.city << std::endl; }
```

```c++
// SELECT age = avg(age), city_id = city_id
//        FROM persons
//        GROUP BY city_id

linq.select(_Age = _Avg(_Age), _City_id = _City_id)
    .from(persons)
    .group_by(_City_id) |
  [] (auto& p) { std::cout << p.age << " is the average age in city " << p.city_id << std::endl; }
```

## Compilers support

IOD relies on the C++14 standard. It has been successfully compiled with :
  - GCC 4.9
  - Clang 3.4

## Contributing

Contributions or suggestions are welcome. Do not hesitate to fill issues, send pull
requests, or discuss by email at matthieu.garrigues@gmail.com.
