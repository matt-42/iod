#include <iostream>

#include "iod.hh"
#include "iod_json.hh"

// Declaration of the attributes used by the inline object definitions.
iod_define_attribute(name);
iod_define_attribute(age);
iod_define_attribute(cars);
iod_define_attribute(model);
iod_define_attribute(cities);
iod_define_attribute(lastname);
iod_define_attribute(inc_age);

int main()
{

  // Inline object definition.
  auto person = iod(
    *name = "Philippe", // stared fields are serialized.
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

  person(inc_age, 10); // Call the inc_age method with arguments (person, 10).

  // Serialize an object to json.
  std::string json = iod_to_json(person);
  std::cout << json << std::endl;

  // Load an object from a json string.
  std::string json_string = R"json({"name":"John", "age": 12})json";
  auto test = iod(*name = "", *age = int());
  iod_from_json(test, json_string);

  // Extend and object. (todo)
  // auto extended_person = iod_extend(person, iod(lastname_ = "Doe"));
}
