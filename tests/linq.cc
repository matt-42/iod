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

  typedef decltype(D(_Age2 = int(),
                     _Age = int(), 
                     _Name = std::string()))
    Person;

  {
    Person a = D(_Age2 = 3, _Age = 10, _Name = std::string("Tom"));
    std::vector<Person> persons = {
      D(_Age2 = 3, _Age = 10, _Name = ("Tom")),
      D(_Age2 = 12, _Age = 1, _Name = ("Tim")),
      D(_Age2 = 12, _Age = 12, _Name = ("Tam"))
    };
    
    {
      linq.select()
        .from(persons, _As(_Person))
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      linq.select()
        .from(persons, _As = _Person)
        .order_by(_Person[_Age])
        .where(_Person[_Age] < 12)
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      typedef decltype(D(_Age2 = int(), _Age = int(), _Name = std::string(), _Cp = std::string())) Person;
      typedef decltype(D(_Name = std::string(), _Cp = std::string())) City;

      std::vector<Person> persons = {
        D(_Age2 = 3, _Age = 10, _Name = ("Tom"), _Cp = "92320"),
        D(_Age2 = 12, _Age = 1, _Name = ("Tim"), _Cp = "75001"),
        D(_Age2 = 12, _Age = 12, _Name = ("Tam"), _Cp = "75002")
      };

      std::vector<City> cities = {
        D(_Name = "Chatillon", _Cp = "92320"),
        D(_Name = "Paris 1er", _Cp = "75001"),
        D(_Name = "Paris 2eme", _Cp = "75002")
      };

      linq.select(_Name = _Person[_Name], _City = _City[_Name], _Cp = _Cp, _Age = _Age2)
        .from(persons, _As(_Person))
        .inner_join(cities, _As(_City), _On(_City[_Cp] == _Person[_Cp]))
        .order_by(_City[_Name])
        .where(_Person[_Age] < 12) |
        [] (const auto& r) { 
        std::cout << r.name << " " << r.city << " " << r.cp <<  std::endl; 
      };
    }

    {
      std::vector<Person> persons = {
        D(_Age2 = 3, _Age = 10, _Name = ("Tom")),
        D(_Age2 = 12, _Age = 1, _Name = ("Tim")),
        D(_Age2 = 12, _Age = 12, _Name = ("Tam"))
      };

      linq.select(_Age = _Avg(_Age), _Sum = _Sum(_Person[_Age2]))
        .from(persons, _As(_Person)).group_by(_Person[_Age2]).where(_Age > 2)
        |
        [] (const auto& r) { std::cout << "average(age): " << r.age << " "
                                       << "sum(age2): " << r.sum
                                       <<  std::endl; };

    }

  }
}
