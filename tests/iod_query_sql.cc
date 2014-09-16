#include <iostream>

#include <iod.hh>
#include <iod_query_sql2.hh>
#include <iod_json.hh>

iod_define_attribute(age);
iod_define_attribute(age2);
iod_define_attribute(name);

int main()
{
  using namespace iod_query;

  typedef decltype(iod(age2 = 12, age = 10, name = "xx")) Person;

  {
    std::vector<Person> persons = {
      iod(age2 = 3, age = 10, name = "Tom"),
      iod(age2 = 12, age = 12, name = "Tim")
    };

    struct
    {
      std::string name() { return "person_table"; }
    } table;

    (R"sql(Select from person_table where age > 12)sql");

    //std::cout << sql::exp_list_to_sql(Select . from(persons) . where(age2 >= age)) << std::endl;
    std::cout << sql::exp_list_to_sql(Update . from(table) . where(age2 + "dsf12")) << std::endl;
  }
}

// expression_list<select, where_expression<sup_expression<Symbol<iod_internals::attribute_name<age_attribute_name, age_attribute, 0> >, qvalue<int> > > >’
