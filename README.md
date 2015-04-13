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
#include <iostream>
#include <iod/symbol.hh>

iod_define_symbol(a); // Refer to members and methods a with symbol _a
iod_define_symbol(b); // Refer to members and methods b with symbol _b
iod_define_symbol(c); // Refer to members and methods c with symbol _c

int main() {

  // Symbols are located in namespace s to avoid name clash.
  using s;

  auto print_member = [] (auto& obj, auto& m)
                      {
                        std::cout << "obj." << m.name()
                                  << " == " << m.member_access(obj) << std::endl;
                      };

  struct { int a; int b; int c; } obj{21, 42, 84};
  print_member(obj, _a); // Prints "obj.a == 21"
  print_member(obj, _b); // Prints "obj.b == 42"
  print_member(obj, _c); // Prints "obj.c == 84"
}
```

Without symbols (or other similar constructs), it is not possible to
write such a generic print_member function. Without, one would have to
write the three version accessing the three different members.

By convention all the symbols starts with _[lowercase character]. And
to avoid multiple definition, guards should be used such as in the
following:

```c++
#ifndef IOD_SYMBOL_mysymbol
#define IOD_SYMBOL_mysymbol
  iod_define_symbol(mysymbol);
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
  auto o = D(_name = "John", _age = 42, _city = "NYC");
  // Direct access to its members.
  assert(o.name == "John" && o.age == 42 && o.city == "NYC");

  // Static Introspection. It has no execution cost since these function
  // are computed at compile time.
  assert(o.has(_name) == true);
  assert(o.has(_firstName) == false);
  assert(o.has(_firstName) == false);
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

Note on memory allocation: While the parser does not allocate any
intermediate structure, it allocates the destination object's
members if they are of type std::vector or std::string.

```c++

// The type of the object contains attributes related to its json
// representation such as alternative json keys and whether or not a
// field should not be included in the json representation.
typedef decltype(D(_name(_json_key = _username) = std::string(),
                   _age(_json_skip) = int(),
                   _city = std::string())) User;
  
User u("John", 23, "NYC");

// Encode to a json string.
auto str = json_encode(u);
assert(str == R"({"username":"John","city":"NYC"})");

// Decode a json string representing a user.
User u2; json_decode(u2, str);
assert(u2.name == "John" and u2.city == "NYC");

// The json throw exceptions when some value type mismatch
// the object or when some fields are missing:

json_decode(u2, R"({"username":"John"})");
// Exception: json_decode error: missing field city.

json_decode(u2, R"({"username":"John","city":22})");
//  Exception:
//  Json parse error near name":"John","city":22
                                             ^^^
//  Expected " got 2

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
  int optional_arg1 = options.get(_optional_arg1, 1); // return options.optional_arg1 or 1 if not set.
  int optional_arg2 = options.get(_optional_arg2, 1);
  int optional_arg3 = options.get(_optional_arg3, 1);
}

fun(1, _optional_arg3 = 2); // Set the thirds argument and leave the two others to their default value.
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

auto my_sio = D(_name = "John", _age = 42);
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

The IOD library provides a set of tools to ease the embedding of
embedded domain specific languages (EDSL) framework. It includes an
abstract syntax tree (AST) with symbols and values as terminals. Here
is an example of a simple expression:

```c++
auto exp = _a(23) + _b;
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
std::vector<decltype(D(_name = std::string(), _age() = int(), _city_id = int()))> persons;
std::vector<decltype(D(_id = int(), _name() = std::string(), _zipcode = int()))> cities;
```

The following requests are valid:

```c++
// SELECT * from persons;
linq.select().from(persons) | [] (auto& p) { std::cout << p.name << std::endl; }
```

```c++
// SELECT myname = name from persons WHERE age > 42;
linq.select(_myname = _name).from(persons).where(_age > 42) |
  [] (auto& p) { std::cout << p.myname << std::endl; }
```

```c++
// SELECT name = person.name, city = city.name
//        FROM persons as person
//        INNER JOIN cities as city
//        ON person.city_id == city.id
linq.select(_name = _person[_name], _city = _city[_name])
    .from(persons, _as(_person))
    .inner_join(cities, _as(_city),
              _on(_city[_cp] == _person[_cp])) |
  [] (auto& p) { std::cout << p.name << " lives in " << p.city << std::endl; }
```

```c++
// SELECT age = avg(age), city_id = city_id
//        FROM persons
//        GROUP BY city_id

linq.select(_age = _avg(_age), _city_id = _city_id)
    .from(persons)
    .group_by(_city_id) |
  [] (auto& p) { std::cout << p.age << " is the average age in city " << p.city_id << std::endl; }
```

## Compilers support

IOD relies on the C++14 standard. It has been successfully compiled with :
  - GCC 4.9
  - Clang 3.4

## Contributing

Contributions or suggestions are welcome. Do not hesitate to fill issues, send pull
requests, or discuss by email at matthieu.garrigues@gmail.com.
