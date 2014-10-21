#include <iostream>
#include <vector>
#include <algorithm>

#include <iod/sio.hh>
#include <iod/linq.hh>

iod_define_symbol(age);
iod_define_symbol(age2);
iod_define_symbol(name);
iod_define_symbol(toto);
iod_define_symbol(person);
iod_define_symbol(city);
iod_define_symbol(cp);

template <typename T>
struct PersonT
{
  int age;
};

int main()
{
  using namespace iod;
  using namespace s;

  typedef decltype(D(age2 = int(),
                     age = int(), 
                     name = std::string()))
    Person;

  {
    Person a = D(age2 = 3, age = 10, name = std::string("Tom"));
    std::vector<Person> persons = {
      D(age2 = 3, age = 10, name = ("Tom")),
      D(age2 = 12, age = 1, name = ("Tim")),
      D(age2 = 12, age = 12, name = ("Tam"))
    };

    {
      linq.select()
        .from(persons, as(person))
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      linq.select()
        .from(persons, as = person)
        .order_by(person[age])
        .where(person[age] < 12)
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      typedef decltype(D(age2 = int(), age = int(), name = std::string(), cp = std::string())) Person;
      typedef decltype(D(name = std::string(), cp = std::string())) City;

      std::vector<Person> persons = {
        D(age2 = 3, age = 10, name = ("Tom"), cp = "92320"),
        D(age2 = 12, age = 1, name = ("Tim"), cp = "75001"),
        D(age2 = 12, age = 12, name = ("Tam"), cp = "75002")
      };

      std::vector<City> cities = {
        D(name = "Chatillon", cp = "92320"),
        D(name = "Paris 1er", cp = "75001"),
        D(name = "Paris 2eme", cp = "75002")
      };

      linq.select(name = person[name], city = city[name], cp = cp, age = age2)
        .from(persons, as(person))
        .inner_join(cities, as(city), on(city[cp] == person[cp]))
        .order_by(city[name])
        .where(person[age] < 12) |
        [] (const auto& r) { 
        std::cout << r.name << " " << r.city << " " << r.cp <<  std::endl; 
      };
    }

    {
      std::vector<Person> persons = {
        D(age2 = 3, age = 10, name = ("Tom")),
        D(age2 = 12, age = 1, name = ("Tim")),
        D(age2 = 12, age = 12, name = ("Tam"))
      };

      linq.select(age = avg(age), sum = sum(person[age2]))
        .from(persons, as(person)).group_by(person[age2]).where(age > 2)
        |
        [] (const auto& r) { std::cout << "average(age): " << r.age << " "
                                       << "sum(age2): " << r.sum
                                       <<  std::endl; };

    }

  }
}
