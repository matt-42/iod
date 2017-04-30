#include <string>
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
  
  using namespace std::string_literals;
 
  using namespace iod;
  using namespace s;
  
  {
    auto o = D(_name = "John"s, _age = 12, _children = { 1,2,3,4,5 }, _city = D(_name = "Paris"s));
    auto str = json_encode(o);

    std::cout << str << std::endl;
    decltype(o) p;
    json_decode(p, str);
    assert(str == R"json({"name":"John","age":12,"children":[1,2,3,4,5],"city":{"name":"Paris"}})json");
    assert(json_encode(p) == str);

    auto s = R"json({"name":"John","age":12, "children":[1,2,3,4,5],"city":{"name":"Paris"}})json";
    json_decode(p, s);
  }

  {
    auto o = D(
      _name(_optional, _json_key = _my_json_name) = std::string("xxxx")
      );

    std::cout << json_encode(o) << std::endl;
  }

  { // UTF 8
    auto o = D(_name = std::string());
    auto s = R"json({"name":"\u20ac\u20ac\u20ac\u20ac\u20ac"})json";

    json_decode(o, s);
    std::cout << o.name << std::endl;
    assert(o.name.size() == 15 and o.name == "€€€€€"s);
  }

  {
    typedef decltype(D(_name(_json_key = _username) = std::string(),
                       _age(_json_skip) = int(),
                       _city = std::string())) User;
    
    User u("John"s, 23, "NYC"s);
    
    auto str = json_encode(u);
    assert(str == R"({"username":"John","city":"NYC"})");
    std::cout << str << std::endl;
    User u2;
    json_decode(u2, str);
    assert(u2.name == "John" and u2.city == "NYC");
    // Type missmatch: json_decode(u2, R"({"username":"John","city":22})");
    // Missing field: json_decode(u2, R"({"username":"John"})");
    
  }

  // Decode with scheme.
  {
    typedef decltype(D(_name(_json_key = _username) = std::string(),
                       _age = int(),
                       _city = std::string())) user_scheme;

    struct { std::string name, city; int age; } custom_user;

    std::string str = R"({"username":"John","age":12,"city":"NYC"})";

    json_decode<user_scheme>(custom_user, str);

    std::cout << custom_user.name << " " << custom_user.age << " " << custom_user.city  << std::endl;
  }

  // json_string :
  {
    typedef decltype(D(_city = json_string())) city;
    typedef decltype(D(_city = json_string(), _name = std::string())) city2;

    city c;
    city2 c2;

    std::string str = R"({"city": { "age":12,"city":{"age" : { "age" : [[12,12,34]]}}}})";
    json_decode(c, str);
    assert(c.city.str == R"({ "age":12,"city":{"age" : { "age" : [[12,12,34]]}}})");

    str = R"({"city":  "te{}}{}\"st\\"}})";
    json_decode(c, str);
    std::cout << c.city.str << std::endl;
    assert(c.city.str == R"("te{}}{}\"st\\")");

    str = R"({"city":  "test", "name": "john"})";
    json_decode(c2, str);
    assert(c2.city.str == "\"test\"");

    str = R"({"city":  1234, "name": "john"})";
    json_decode(c2, str);
    assert(c2.city.str == "1234");
  }

  // Float vectors.
  {
    typedef decltype(D(_age = { 0.f })) P;

    std::string str = R"({"age":[001.23, 0.23, -0.234, .32, -.32, .345e5]})";

    P o;
    json_decode(o, str);

    std::cout << o.age[0] << std::endl;
    assert(o.age[0] == 1.23f);
    assert(o.age[1] == 0.23f);
    assert(o.age[2] == -0.234f);
    assert(o.age[3] == .32f);
    assert(o.age[4] == -.32f);
    assert(o.age[5] == .345e5f);
    
  }
  // Stringview
  {
    std::string str = R"json({"name":"John","city":"John2","age":12})json";

    auto object = iod::D(_name = stringview(), _city = stringview(), _age = int());
    iod::json_decode(object, str);
    assert(json_encode(object) == str);
  }

  // Escape
  {
    std::string str = R"({"name":"\\ \"age\\\\\" \\"})";
    auto object = iod::D(_name = std::string());
    iod::json_decode(object, str);
    std::cout << json_encode(object) << " == " << str << std::endl; 
    assert(json_encode(object) == str);
  }

  // Encode strings
  {
    auto object = iod::D(_name = "\"");
    assert(iod::json_encode(object) == R"({"name":"\""})");
  }

}
