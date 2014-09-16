#ifndef IOD_QUERY_SQL_HH_
# define IOD_QUERY_SQL_HH_

# include "iod_query.hh"

namespace iod_query
{

  struct sql_database {};


  namespace sql
  {

    struct table
    {
      // table
      // std::vector
    };

    // Syntax :
    // db = iod(people = table<Person>("people"),
    //          cities = table<City>("cities"));

    connection.select | from(db.people) . where(age > 12) . orderby(name);
    connection.update | from(db.people) . where(age > 12) . update(name = "John");

    select | from(vector) . where(age > 12) . set(name = "John");
    Delete . 
    //vector
    // Select(connection, db.people) || where(age > 14) | orderby(name);
    // Update(connection, db.people) || where(id == p.id) | update(name = "John", age = 42);
    // Delete(connection, db.people) || where(age > 14);
    // Insert(connection, db.people)(p);

    template <typename A>
    inline void
    translate(const qvalue<A>& v, std::stringstream& ss)
    {
      ss << v.value;
    }

    template <typename A>
    inline void
    translate(const Symbol<A>&, std::stringstream& ss)
    {
      ss << A::attribute_name();
    }

    template <typename A, typename B>
    inline void
    translate(const sup_expression<A, B>& e, std::stringstream& ss)
    {
      translate(e.lhs, ss);
      ss << " > ";
      translate(e.rhs, ss);
    }


    template <typename T, unsigned I, unsigned N>
    typename std::enable_if<I == N, void>::type
    where_clause(const E& e, std::stringstream& ss)
    {
    }

    template <typename T, unsigned I, unsigned N>
    typename std::enable_if<T::get_type<I>::node_type == where_node, void>::type
    where_clause(const E& e, std::stringstream& ss)
    {
      ss << " where ";
      translate(w.condition);
    }

    template <typename E, unsigned I = 0, unsigned N = E::size>
    inline void
    where_clause(const E& e, std::stringstream& ss)
    {
      where_clause<E, I+1, N>(e, ss);
    }

    // template <typename A, typename B>
    // inline void
    // orderby_clause(const expression_list<E...>& e, std::stringstream& ss)
    // {
    //   e.find<where_expression>
    //     ([] (const where_expression& w)
    //      {
    //        ss << " order by ";
    //        translate(w.order);
    //      });
    // }

    template <typename T, unsigned I, unsigned N>
    typename std::enable_if<I == N, void>::type
    field_list_string(std::stringstream& ss)
    {
      ss << T::nth_type<I>().attribute_name();
    }

    template <typename T, unsigned I = 0, N = T::size>
    void field_list_string(std::stringstream& ss)
    {
      ss << T::nth_type<I>().attribute_name() << ',';
      field_list_string<T, I+1, N>(ss);
    }

    // std::string translate()

    // Operator, launch the evaluation.
    template <typename T, typename... E>
    std::vector<sql_table::type> operator||(const sql_table& table, const expression_list<E...>& exp)
    {
      typedef sql_table::type T;

      std::stringstream ss;

      ss << "SELECT (" << field_list_string<T>() << ") from "
         << table.name() << " ";
      where_clause(exp, ss);
      ss << " ";
      orderby_clause(exp, ss);

      return table.execute(ss.str());
    }

    // Operator, launch the evaluation.
    template <typename T, typename E>
    std::vector<T> operator||(const sql_database& v, const Exp<E>& exp)
    {
      std::vector<T> out;
      table || expression_list<E>(exp);
      return out;
    }

  }

}
