#include <iostream>
#include <vector>

#include <iod/linq.hh>

#include "symbols.hh"

template <typename T>
struct PersonT
{
  int age;
};

int main()
{
  using namespace std::string_literals;
 
  using namespace iod;
  using namespace s;

  typedef decltype(D(_age2 = int(),
                     _age = int(), 
                     _name = std::string()))
    Person;

  {
    Person a = D(_age2 = 3, _age = 10, _name = std::string("Tom"s));
    std::vector<Person> persons = {
      D(_age2 = 3, _age = 10, _name = ("Tom"s)),
      D(_age2 = 12, _age = 1, _name = ("Tim"s)),
      D(_age2 = 12, _age = 12, _name = ("Tam"s))
    };
    
    {
      linq.select()
        .from(persons, _as(_person))
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      linq.select()
        .from(persons, _as(_person))
        .order_by(_person[_age])
        .where(_person[_age] < 12)
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      typedef decltype(D(_age2 = int(), _age = int(), _name = std::string(), _cp = std::string())) Person;
      typedef decltype(D(_name = std::string(), _cp = std::string())) City;

      std::vector<Person> persons = {
        D(_age2 = 3, _age = 10, _name = ("Tom"s), _cp = "92320"s),
        D(_age2 = 12, _age = 1, _name = ("Tim"s), _cp = "75001"s),
        D(_age2 = 12, _age = 12, _name = ("Tam"s), _cp = "75002"s)
      };

      std::vector<City> cities = {
        D(_name = "Chatillon"s, _cp = "92320"s),
        D(_name = "Paris 1er"s, _cp = "75001"s),
        D(_name = "Paris 2eme"s, _cp = "75002"s)
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

    // Fixme: Fix compilation of aggregates.
    // {
    //   std::vector<Person> persons = {
    //     D(_age2 = 3, _age = 10, _name = ("Tom")),
    //     D(_age2 = 12, _age = 1, _name = ("Tim")),
    //     D(_age2 = 12, _age = 12, _name = ("Tam")),
    //     D(_age2 = 12, _age = 5, _name = ("Tem"))
    //   };

    //   linq.select(_age = _avg(_age), _sum = _sum(_person[_age2]))
    //     .from(persons, _as(_person)).group_by(_person[_age2]).where(_age > 2)
    //     |
    //     [] (const auto& r) { std::cout << "average(age): " << r.age << " "
    //                                    << "sum(age2): " << r.sum
    //                                    <<  std::endl; };

    // }

  }
}
