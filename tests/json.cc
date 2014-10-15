#include <iostream>
#include <cassert>
#include <iod/iod.hh>
#include <iod/json.hh>

iod_define_symbol(name);
iod_define_symbol(age);
iod_define_symbol(city);
iod_define_symbol(children);
iod_define_symbol(my_json_name);

int main()
{
  using namespace iod;
  using namespace s;
  {
    auto o = D(name = "John", age = 12, children = { 1,2,3,4,5 }, city = D(name = "Paris"));
    auto str = json_encode(o);

    decltype(o) p;
    json_decode(p, str);
    assert(str == R"json({"name":"John","age":12,"children":[1,2,3,4,5],"city":{"name":"Paris"}})json");
    assert(json_encode(p) == str);

    auto s = R"json({"name":"John","age":12, "children":[1,2,3,4,5],"city":{"name":"Paris"}})json";
    json_decode(p, s);
  }

  {
    auto o = D(
      name = std::string("xxxx"), optional, (json_symbol = my_json_name)
      );

    std::cout << json_encode(o) << std::endl;
  }
}
