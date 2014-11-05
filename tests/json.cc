#include <iostream>
#include <cassert>
#include <iod/foreach.hh>
#include <iod/sio_utils.hh>
#include <iod/sio.hh>
#include <iod/json.hh>
#include <iod/symbol.hh>
#include "symbols.hh"

int main()
{
  using namespace iod;
  using namespace s;

  {
    auto o = D(_Name = "John", _Age = 12, _Children = { 1,2,3,4,5 }, _City = D(_Name = "Paris"));
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
      _Name(_Optional, _Json_symbol = _My_json_name) = std::string("xxxx")
      );

    std::cout << json_encode(o) << std::endl;
  }
}
