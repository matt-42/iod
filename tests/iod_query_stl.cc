#include <iostream>

#include <iod.hh>
#include <iod_query_stl.hh>
#include <iod_json.hh>

iod_define_attribute(age);
iod_define_attribute(age2);
iod_define_attribute(name);

int main()
{
  using namespace iod_query;

  // {
  //   auto p = Select . where(age > 12);

  //   // void* x = p;
  //   static_assert(std::is_same<decltype(p),
  //                 expression_list<select_exp, where_expression<decltype(age > 12) > > >::value, "error");

  //   auto q = p.orderby(age);

  //   static_assert(std::is_same<decltype(q),
  //                 expression_list<
  //                 select_exp,
  //                 where_expression<decltype(age > 12) >,
  //                 orderby_expression<decltype(age)>
  //                                    >
  //                                    >::value, "error");
  // }

  // {

  //   typedef decltype(iod(age = 10, name = "xx")) Person;
  //   std::vector<Person> persons;

  //               //get_nth_element_t<3, std::tuple<int, int>> x;
  //              auto p = Select . from(persons);
  //                p.template find<where_tag>();
  //               p.template find<from_tag>();
  // }

  typedef decltype(iod(age2 = int(), age = int(), name = std::string())) Person;

  // Maybe better ?
  // typedef iod_type<age2<int>,
  //                  age<int>,
  //                  name<std::string>> Person;

  {

    std::vector<Person> persons = {
      iod(age2 = 3, age = 10, name = "Tom"),
      iod(age2 = 12, age = 12, name = "Tim")
    };

    //std::cout << is_iod_attribute<int>::value << std::endl;
    static_assert(is_exp<decltype(name == "Tom")>::value, "test");

    Select . from(persons) . where(age2 >= age) |
      [] (Person& p) { std::cout << p.name << std::endl; };
  }

  {
    typedef decltype(iod(*age2 = 12, *age = 10, *name = "xx")) Person;
    std::vector<Person> persons = {
      iod(*age2 = 3, *age = 10, *name = "Tom"),
      iod(*age2 = 12, *age = 12, *name = "Tim")
    };

    Update . from(persons) . where(name == "Tom") . set(name = "my name has changed", age = 42) . execute();

    Select . from(persons) . orderby(age)  |
      [] (Person& p) { std::cout << iod_to_json(p) << std::endl; };

    Insert . into(persons) . values(name = "New people", age = 22222222) . execute();

    std::cout << "insert" << std::endl;

    Select . from(persons)  . orderby(name) ||
      [] (Person& p) { std::cout << iod_to_json(p) << std::endl; };

    std::cout << "delete" << std::endl;

    Delete . from(persons) . where(age == 22222222 || age2 == 3) . execute();
    Select . from(persons)  |
      [] (Person& p) { std::cout << iod_to_json(p) << std::endl; };

  }
}

// expression_list<select, where_expression<sup_expression<Symbol<iod_internals::attribute_name<age_attribute_name, age_attribute, 0> >, qvalue<int> > > >’
