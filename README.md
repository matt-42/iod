The IOD Library.
==========================

The IOD library pushes C++14 meta programming to the next level by using
a symbol based paradigm. It provides a compile-time a way to introspect objects and
generate code matching their data structures.

## What is a IOD symbol?

Symbols are at the core of the IOD paradigm. They add to C++ a missing
powerfull feature: The way to statically store in a variable the
access to a object member, the call to a method, and the access to the
string representing the name of this variable. Here is the most simple
but powerfull operator that is now possible with IOD symbols:

```c++
#include <iod/symbol.hh>

namespace s
{
  iod_define_symbol(a, _A); // Refer to members and methods a with symbol _A
  iod_define_symbol(b, _B); // Refer to members and methods b with symbol _B
  iod_define_symbol(c, _C); // Refer to members and methods b with symbol _C
}

int main() {

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
process by converting a file containing a list of symbol into a valid
C++ header containing the symbol definitions.

## Statically introspectable objects (SIO)

Statically introspectable objects 

  - Define POD-like objects without having to declare any struct or class.
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
  foreach(o) | [] (auto& m) { std::cout << m.symbol().name() << ":" << m.value() << std::end; }

  json_encode(o) // => {"name":"John","age":42,"City":"NYC"}
```

## A fast, malloc-free Json Parser.

As of today, all json parsers rely dynamic data structures to
parse and store the json objects. Even if some good parser reduces
dramastically the amount of dynamic memory allocation, they still
suffer from the dynamic paradigm: A single function has to handle the
whole set of possible json objects. This comes to the absurde situation
where a program handling a small set specific json object types
have to use a json parser handling any kind of objects.

The iod library implement the opposite approach: Using
meta-programming and introspection, one tiny specialized json parser
is generated for each object type so it involves no dynamic memory
allocation, and very few conditional branching. This makes its
performances impossible to match in other languages such as C or Java
with using a generic parser.

The only limitation of this design is when using a very large type set
of json objects, the total code size of the generated parsers will be
bigger than a generic dynamic parser.

```c++
auto o = D(_Name = "John", _Age = 42, _City = "NYC");
auto str = json_encode(o) // => {"name":"John","age":42,"City":"NYC"}

decltype(o) o2;
json_decode(o2, str);
```

## Named Optional Function Arguments

In classic C or C++, you would define a function taking optional arguments as :

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
tuples. ```foreach``` is a powerfull primitive for processing tuples
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


## Language integrated queries

To demonstrate the power of the IOD framework, we embeded an
implementation of a subset of the SQL language in the C++ language.
It handles SELECT, FROM, WHERE, INNER_JOIN (with the ON criteria),
ORDER BY, GROUP BY, and aggregators such as average or sum.

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


## Language integrated queries



Javascript Vs C++: Introspection, on the fly object definition, object are maps
Iod makes meta programming very easy.
A new meta-programming paradigm: Symbol-based meta programming.

What is a symbol ?
```c++
struct name_t
{
   const char* name() const { return #SYMBOL; }                 
                                                                        
   template <typename T>                                               
   auto attribute_access(const T& o) const { return o.SYMBOL; } 
                                                                        
   template <typename T, typename... A>                                
   auto method_call(const T& o, A... args) const { return o.SYMBOL(args...); }

   template <typename T>
   struct variable_t { T name; };
};

One of the major advantage of dynamic languages like Javascript over
C++ is that they 


#What is Symbol Based Meta-Programming?

C++ Meta-Programmer rely on types to implement their
meta-algorithms. It usually lead to really deep complex templated
structures hard to read and maintain. Iod greatly simplify
meta-programming by letting the programmer play with symbols instead
of types.



The iod library introduces a new paradigm of C++ metaprograming. Its
goal is to make writing of meta code as simple as plain C++ code.

Its main features are :


 It
also provide a meta-embeded language that you can map to any specific
application domain. As a proof of concept, a subset of SQL has been
embedded in C++, as well as a JSON-Like

**Inline object definitions and declarations** with **static
introspection** for C++11. This library embeds in C++11 a fast
type-safe **json-like** domain specific language. It allows, in one
C++ statement, to define, declare and instantiate a C++ object,
**without any overhead execution time**.

While keeping the performance and type-checking of C++, it adds to C++11 the
flexibility of dynamic languages like javascript where declaring an object or
extending it can be done on the fly. The library also provides **a json
serializer and deserializer**, taking advantage of the static introspection and
meta-programming.

## Compilers

The core of iod relies on C++11 (-std=c++11), except method definitions that use generic lambda
from C++14 (-std=c++1y).
Iod has been successfully compliled with :
  - GCC 4.9
  - Clang 3.4

Previons versions of GCC and Clang with support of C++11 should be able to compile iods without methods.

## Overview of the library
```c++

// Headers
#include "iod.hh"
#include "iod_json.hh"

// Declaration of the attributes used by the inline object definitions.
iod_define_attribute(name);
iod_define_attribute(age);
iod_define_attribute(cars);
iod_define_attribute(model);
iod_define_attribute(cities);
iod_define_attribute(lastname);


int main()
{


  // Inline object definition.
  auto person = iod(
    *name = "Philippe", // Stared fields are serialized.
    *age = 42,
    inc_age = [] (auto& self, int inc) { self.age += inc; }, // Requires C++14.
    *cities = {"Paris", "Toronto", "New York City"},
    *cars = {
        iod(*name = "Renault", model = "Clio"), // All elements of an array must have the same type.
        iod(*name = "Mercedes", model = "Class A")
      }
    );

  // Access to the content of the object.
  std::cout << person.name << std::endl;
  std::cout << person.cars[1].model << std::endl;

  // Serialize an object to json.
  std::string json = iod_to_json(person);
  std::cout << json << std::endl;

  // Load an object from a json string.
  std::string json_string = R"json({"name":"John", "age": 12})json";
  auto test = iod(name = "", age = int());
  iod_from_json(test, json_string);

  // Extend and object. (todo)
  auto extended_person = iod_extend(person, iod(lastname = "Doe"));
}


```

## Contributing

Contributions or suggestions are welcome. Do not hesitate to fill issues, send pull
requests, or discuss by email at matthieu.garrigues@gmail.com.
