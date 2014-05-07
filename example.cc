#include <iostream>

#include "iod.hh"
#include "iod_json.hh"

// Declaration of the attributes used by the inline object definitions.
iod_define_attribute(name);
iod_define_attribute(age);
iod_define_attribute(cars);
iod_define_attribute(model);
iod_define_attribute(cities);

int main()
{

  // Inline object definition.
  auto person = iod(
    name_ = "Philippe",
    age_ = 42,
    cities_ = {"Paris", "Toronto", "New York City"},
    cars_ = {
        iod(name_ = "Renault", model_ = "Clio"),
        iod(name_ = "Mercedes", model_ = "Class A")
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
  auto test = iod(name_ = "", age_ = int());
  iod_from_json(test, json_string);

}
