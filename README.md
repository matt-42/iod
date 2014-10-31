The IOD Library, Symbol Based Meta-Programming for C++14
==========================


The goal of the IOD library is to brings to C++14 closer to the flexibility
of Javascript without impacting its speed.

# Statically introspectable objects (SIO)

  - Define object without having to declare any struct or class.
  - Static introspection (i.e. without overhead on execution time) on those objects.
  - A fast Json library built using this static introspection to generate a serializer
    and a deserializer dedicated to each SIO.
  - A way to iterate on the member of the object.
  - Apply the
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

# Optional and Named Function Arguments

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
  auto options = D(opts...);
  int optional_arg1 = options.get(_Optional_arg1, 1); // return options.optional_arg1 or 1 if not set.
}

fun(1, _Optional_arg3 = 2); // Set the thirds argument and leave the two others to their default value.
```

# 


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
