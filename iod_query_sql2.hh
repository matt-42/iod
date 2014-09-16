#ifndef IOD_QUERY_SQL_HH_
# define IOD_QUERY_SQL_HH_

# include <algorithm>
# include <sstream>
# include <tuple>
# include "iod.hh"
# include "iod_query2.hh"


namespace iod_query
{

  namespace sql
  {
    template <typename T>
    inline void
    generate(const T& v, std::stringstream& ss)
    {
      ss << v;
    }

    inline void
    generate(const std::string& v, std::stringstream& ss)
    {
      ss << '"' << v << '"';
    }

    inline void
    generate(const char* v, std::stringstream& ss)
    {
      ss << '"' << v << '"';
    }

    template <typename A>
    inline void
    generate(const qvalue<A>& v, std::stringstream& ss)
    {
      std::cout << "qvalue: " << v.value << std::endl;
      generate(v.value, ss);
    }


    template <typename A>
    inline void
    generate(const Symbol<A>&, std::stringstream& ss)
    {
      ss << A().attribute_name();
    }

#define iod_sql_binary_op(OP, NAME)                             \
    template <typename A, typename B>                           \
    inline void                                                 \
    generate(const NAME##_exp<A, B>& e, std::stringstream& ss)  \
    {                                                           \
    generate(e.lhs, ss);                                        \
    ss << ' ' << #OP << ' ';                                    \
    generate(e.rhs, ss);                                        \
    }

    iod_sql_binary_op(+, plus);
    iod_sql_binary_op(-, minus);
    iod_sql_binary_op(*, mult);
    iod_sql_binary_op(<<, shiftl);
    iod_sql_binary_op(>>, shiftr);
    iod_sql_binary_op(<, inf);
    iod_sql_binary_op(<=, inf_eq);
    iod_sql_binary_op(>, sup);
    iod_sql_binary_op(>=, sup_eq);
    iod_sql_binary_op(==, eq);
    iod_sql_binary_op(!=, neq);
    iod_sql_binary_op(&, logical_and);
    iod_sql_binary_op(^, logical_xor);
    iod_sql_binary_op(|, logical_or);
    iod_sql_binary_op(&&, and);
    iod_sql_binary_op(||, or);

#undef iod_sql_binary_op

    inline void generate(const select_exp&, std::stringstream& ss)
    { ss << "SELECT "; }
    inline void generate(const update_exp&, std::stringstream& ss)
    { ss << "UPDATE "; }
    inline void generate(const insert_exp&, std::stringstream& ss)
    { ss << "INSERT "; }
    inline void generate(const delete_exp&, std::stringstream& ss)
    { ss << "DELETE "; }

    // Where.
    template <typename C>
    void generate(const where_exp<C>& w, std::stringstream& ss)
    { ss << " WHERE "; generate(w.condition, ss); }

    // From
    template <typename C>
    void generate(const from_exp<C>& w, std::stringstream& ss)
    { ss << " FROM " << w.table.name();
    }

    // Orderby
    template <typename C>
    void generate(const orderby_exp<C>& o, std::stringstream& ss)
    { ss << " ORDER BY "; generate(o.order, ss); }

    template <typename... T>
    std::string
    exp_list_to_sql(const exp_list<T...>& e)
    {
      std::stringstream ss;
      generate(e.template get_nth<0>(), ss);
      generate(e.template find<from_tag>(), ss);
      generate(e.template find<where_tag>(), ss);
      return ss.str();
    }
  }

}

#endif
