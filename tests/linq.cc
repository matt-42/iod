#include <iostream>
#include <vector>
#include <algorithm>

#include <iod/iod2.hh>
#include <iod/iod_query_stl2.hh>

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

// namespace iod
// {
//   template <typename T>
//   struct iod_object
//   {
//     int age;
//   };

// }
// person[method]()
// function_call<member_accessor, args...> -> method_call
// function_call<symbol, args...> -> sql function call

int main()
{
  using iod::iod;

  // struct Person
  // {
  //   int age2;
  //   int age;
  //   std::string name;
  // };

   typedef decltype(iod(s::age2 = int(), s::age = int(), s::name = std::string())) Person;

  {
    Person a = iod(s::age2 = 3, s::age = 10, s::name = std::string("Tom"));
    std::vector<Person> persons = {
      iod(s::age2 = 3, s::age = 10, s::name = ("Tom")),
      iod(s::age2 = 12, s::age = 1, s::name = ("Tim")),
      iod(s::age2 = 12, s::age = 12, s::name = ("Tam"))
    };

    //std::vector
    // std::vector<Person> persons = {
    //   { 3, 10, "Tom" },
    //   { 12, 12, "Tim" },
    //   { 12, 12, "Tam" }
    // };

    // std::sort(persons.begin(), persons.end(), [] (const auto& a, const auto& b)
    //           {
    //             return a.age2 < b.age2;
    //           });
    // Select . from(persons) . where(age2 >= age)
    //using namespace iod::s;
    using namespace s;

    {
      iod::stl_query<decltype(iod())> q(iod());
      q.select()
        .from(persons, as(s::person))
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      std::cout << "----" << std::endl;
      iod::stl_query<decltype(iod())> q(iod());
      q.select()
        .from(persons, as = s::person)
        .order_by(s::person[s::age])
        .where(s::person[s::age] < 12)
        |
        [] (const auto& r) { std::cout << r.person.name << " " << r.person.age <<  std::endl; };
    }

    {
      typedef decltype(iod(s::age2 = int(), s::age = int(), s::name = std::string(), s::cp = std::string())) Person;
      typedef decltype(iod(s::name = std::string(), s::cp = std::string())) City;

      std::vector<Person> persons = {
        iod(s::age2 = 3, s::age = 10, s::name = ("Tom"), s::cp = "92320"),
        iod(s::age2 = 12, s::age = 1, s::name = ("Tim"), s::cp = "75001"),
        iod(s::age2 = 12, s::age = 12, s::name = ("Tam"), s::cp = "75002")
      };

      std::vector<City> cities = {
        iod(s::name = "Chatillon", s::cp = "92320"),
        iod(s::name = "Paris 1er", s::cp = "75001"),
        iod(s::name = "Paris 2eme", s::cp = "75002")
      };

      using namespace s;
      std::cout << "----" << std::endl;
      iod::stl_query<decltype(iod())> q(iod());
      q.select(name = person[name], city = city[name], cp = cp, age = age2)
        .from(persons, as(person))
        .inner_join(cities, as(city), on(city[cp] == person[cp]))
        .order_by(city[name])
        .where(person[age] < 12)
        |
        [] (const auto& r) { std::cout << r.name << " " << r.city << " " << r.cp <<  std::endl; };
    }

    {
      std::vector<Person> persons = {
        iod(s::age2 = 3, s::age = 10, s::name = ("Tom")),
        iod(s::age2 = 12, s::age = 1, s::name = ("Tim")),
        iod(s::age2 = 12, s::age = 12, s::name = ("Tam"))
      };

      iod::linq.select(age = avg(age), sum = sum(person[age2]))
        .from(persons, as(person)).group_by(person[age2]).where(age > 2)
        |
        //[] (const auto& r) { std::cout << r.person.name << " " << r.person.age << " " << r.city.name <<  std::endl; };
        [] (const auto& r) { std::cout << "average(age): " << r.age << " "
                                       << "sum(age2): " << r.sum
                                       <<  std::endl; };

    }

    {
      // q.select(name = sum(person[age]), city = concat(city[name]))

      // q.select(name = person[name], city = city[name], cp = city[cp])
      //   .from(persons, as(person))
      //   .inner_join(cities, as(city), on(city[cp] == person[cp]))
      //   .order_by(city[name])
      //   .where(person[age] < 12)

      // q.select(name = person[name], city = city[name], cp = city[cp])
      //   .from(persons, as = person)
      //   .inner_join(cities, as = city, on = city[cp] == person[cp])
      //   .order_by(city[name])
      //   .where(person[age] < 12)

    }
    // std::cout << q.select(name = person[name], age = person[age])
    //   .from(persons, as(s::person))
    //   .order_by(s::person[s::name])
    //       .where(s::person[s::age] == 12).to_array().size() << std::endl;

    //s::toto = s::person[s::name]
    // void* r = iod::stl_query_internals::compute_request_record_type(
    //   q.select(s::toto = s::person[s::name]).where(s::age == 12).from(persons, iod::s::as = s::person).order_by(s::name).query);

    //auto v =  q.select().where(s::age == 12).from(persons, iod::s::as(s::person)).order_by(s::name).to_array();
    //void* x = v[0];
    // for (auto x : v) std::cout << x.name << std::endl;
    //std::cout << iod::evaluate(s::age == 12, iod(s::age = 12)) << std::endl;
  }


  // {
  //    typedef iod::iod_object<age_variable<int>> Person;

  //    // typedef PersonT<age_variable<int>> Person;

  //   Person a;
  //   std::vector<Person> persons = {
  //     a,a,a
  //   };

  //   std::sort(persons.begin(), persons.end(), [] (const auto& a, const auto& b) -> bool
  //             {
  //               return a.age < b.age;
  //             });
  // }

  // {
  //   typedef decltype(iod(*age2 = 12, *age = 10, *name = "xx")) Person;
  //   std::vector<Person> persons = {
  //     iod(*age2 = 3, *age = 10, *name = "Tom"),
  //     iod(*age2 = 12, *age = 12, *name = "Tim")
  //   };

  //   Update . from(persons) . where(name == "Tom") . set(name = "my name has changed", age = 42) . execute();

  //   Select . from(persons) . orderby(age)  |
  //     [] (Person& p) { std::cout << iod_to_json(p) << std::endl; };

  //   Insert . into(persons) . values(name = "New people", age = 22222222) . execute();

  //   std::cout << "insert" << std::endl;

  //   Select . from(persons)  . orderby(name) ||
  //     [] (Person& p) { std::cout << iod_to_json(p) << std::endl; };

  //   std::cout << "delete" << std::endl;

  //   Delete . from(persons) . where(age == 22222222 || age2 == 3) . execute();
  //   Select . from(persons)  |
  //     [] (Person& p) { std::cout << iod_to_json(p) << std::endl; };

  // }
}

// expression_list<select, where_expression<sup_expression<Symbol<iod_internals::attribute_name<age_attribute_name, age_attribute, 0> >, qvalue<int> > > >’
