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

  { // UTF 8
    auto o = D(_Name = std::string());
    auto s = R"json({"name":"\u00E2\u82AC\u00E2\u82AC\u00E2\u82AC\u00E2\u82AC\u00E2\u82AC"})json";

    json_decode(o, s);
    assert(o.name.size() == 15 and o.name == "€€€€€");
    std::cout << o.name << " " <<  o.name.size() << std::endl;
  }

}
