

#include <iod.hh>


int main()
{


auto person = iod(
  *name = "Philippe", // stared fields are serialized.
  *age = 42,

  *cities = {"Paris", "Toronto", "New York City"},
  *cars = {
iod(*name = "Renault", model = "Clio"), // All elements of an array must have the same type.
  iod(*name = "Mercedes", model = "Class A")
  }
  );


}
