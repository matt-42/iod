iod
====

**Inline object definitions and declarations** with **static introspection** for
C++11. This library embeds in C++11 a fast type-safe **json-like** domain specific
language. It allows, in one C++ statement, to define, declare and instantiate
a C++ object, **without any overhead execution time**.

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
