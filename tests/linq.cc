#include <iostream>
#include <vector>
#include <algorithm>

#include <iod/sio.hh>
#include <iod/linq.hh>

#include "symbols.hh"

template <typename T>
struct PersonT
{
  int age;
};

int main()
{
  using namespace iod;
  using namespace s;

  typedef decltype(D(_age2 = int(),
                     _age = int(), 
                     _name = std::string()))
    Person;

  {
    Person a = D(_age2 = 3, _age = 10, _name = std::string("Tom"));
    std::vector<Person> persons = {
      D(_age2 = 3, _age = 10, _name = ("Tom")),
      D(_age2 = 12, _age = 1, _name = ("Tim")),
      D(_age2 = 12, _age = 12, _name = ("Tam"))
    };
    
    {
      linq.select()
        .from(persons, _as(_person))
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      linq.select()
        .from(persons, _as = _person)
        .order_by(_person[_age])
        .where(_person[_age] < 12)
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      typedef decltype(D(_age2 = int(), _age = int(), _name = std::string(), _cp = std::string())) Person;
      typedef decltype(D(_name = std::string(), _cp = std::string())) City;

      std::vector<Person> persons = {
        D(_age2 = 3, _age = 10, _name = ("Tom"), _cp = "92320"),
        D(_age2 = 12, _age = 1, _name = ("Tim"), _cp = "75001"),
        D(_age2 = 12, _age = 12, _name = ("Tam"), _cp = "75002")
      };

      std::vector<City> cities = {
        D(_name = "Chatillon", _cp = "92320"),
        D(_name = "Paris 1er", _cp = "75001"),
        D(_name = "Paris 2eme", _cp = "75002")
      };

      linq.select(_name = _person[_name], _city = _city[_name], _cp = _cp, _age = _age2)
        .from(persons, _as(_person))
        .inner_join(cities, _as = _city, _on = (_city[_cp] == _person[_cp]))
        .order_by(_city[_name])
        .where(_person[_age] < 12) |
        [] (const auto& r) { 
        std::cout << r.name << " " << r.city << " " << r.cp <<  std::endl; 
      };
    }

    {
      std::vector<Person> persons = {
        D(_age2 = 3, _age = 10, _name = ("Tom")),
        D(_age2 = 12, _age = 1, _name = ("Tim")),
        D(_age2 = 12, _age = 12, _name = ("Tam"))
      };

      linq.select(_age = (_avg =  _age), _sum = (_sum = _person[_age2]))
        .from(persons, _as(_person)).group_by(_person[_age2]).where(_age > 2)
        |
        [] (const auto& r) { std::cout << "average(age): " << r.age << " "
                                       << "sum(age2): " << r.sum
                                       <<  std::endl; };

    }

  }
}
