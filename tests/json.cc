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
      _Name(_Optional, _Json_key = _My_json_name) = std::string("xxxx")
      );

    std::cout << json_encode(o) << std::endl;
  }

  { // UTF 8
    auto o = D(_Name = std::string());
    auto s = R"json({"name":"\u00E2\u82AC\u00E2\u82AC\u00E2\u82AC\u00E2\u82AC\u00E2\u82AC"})json";

    json_decode(o, s);
    assert(o.name.size() == 15 and o.name == "€€€€€");
  }

  {
    typedef decltype(D(_Name(_Json_key = _Username) = std::string(),
                       _Age(_Json_skip) = int(),
                       _City = std::string())) User;
    
    User u("John", 23, "NYC");
    
    auto str = json_encode(u);
    assert(str == R"({"username":"John","city":"NYC"})");
    std::cout << str << std::endl;
    User u2;
    json_decode(u2, str);
    assert(u2.name == "John" and u2.city == "NYC");
    // Type missmatch: json_decode(u2, R"({"username":"John","city":22})");
    // Missing field: json_decode(u2, R"({"username":"John"})");
    
  }

}
