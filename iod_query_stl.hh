#ifndef IOD_QUERY_STL_HH_
# define IOD_QUERY_STL_HH_

# include <algorithm>
# include <tuple>
# include "iod.hh"
# include "iod_query2.hh"


namespace iod_query
{

  // Values
  template <typename A, typename T>
  inline const A&
  evaluate(const qvalue<A>& v, const T&)
  {
    return v.value;
  }

  // Access to attributes.
  template <typename A, typename T>
  inline auto
  evaluate(const Symbol<A>&, const T& elt) -> decltype(elt.template get<typename A::name>())
  {
    return elt.template get<typename A::name>();
  }

#define iod_stl_binary_op(OP, NAME) \
  template <typename A, typename B, typename T>                 \
  inline bool                                                   \
  evaluate(const NAME##_exp<A, B>& e, const T& elt)                 \
  {                                                             \
    return evaluate(e.lhs, elt) OP evaluate(e.rhs, elt);        \
  }

#define iod_stl_unary_op(OP, NAME) \
  template <typename A, typename B, typename T>                 \
  inline bool                                                   \
  evaluate(NAME##_exp<A, B> e, const T& elt)                 \
  {                                                             \
    return OP evaluate(e.rhs, elt);        \
  }

  iod_stl_binary_op(+, plus);
  iod_stl_binary_op(-, minus);
  iod_stl_binary_op(*, mult);
  iod_stl_binary_op(<<, shiftl);
  iod_stl_binary_op(>>, shiftr);
  iod_stl_binary_op(<, inf);
  iod_stl_binary_op(<=, inf_eq);
  iod_stl_binary_op(>, sup);
  iod_stl_binary_op(>=, sup_eq);
  iod_stl_binary_op(==, eq);
  iod_stl_binary_op(!=, neq);
  iod_stl_binary_op(&, logical_and);
  iod_stl_binary_op(^, logical_xor);
  iod_stl_binary_op(|, logical_or);
  iod_stl_binary_op(&&, and);
  iod_stl_binary_op(||, or);

#undef iod_stl_binary_op

  // Where.
  template <typename C, typename V>
  bool where_test(const where_exp<C>& w, V& v)
  {
    return evaluate(w.condition, v);
  }
  template <typename V>
  bool where_test(not_found, V& v) { return true; }

  template <typename E>
  using enable_if_orderby = typename std::enable_if<!has_no_orderby<E>::value, void>::type;
  template <typename E>
  using enable_if_no_orderby = typename std::enable_if<has_no_orderby<E>::value, void>::type;

  // Select iteration with lambda. Serial version.
  // Optimized For exp lists without orderby statement.
  template <typename... T, typename F>
  enable_if_no_orderby<exp_list<T...>>
  operator|(exp_list<select_exp, T...>&& e, F fun)
  {
    //check_exp(e); // Todo.
    auto& v = e.template find<from_tag>().table;

     for (auto& o : v)
       if (where_test(e.template find<where_tag>(), o))
         fun(o);
  }

  // Select iteration with lambda. Serial version.
  // Optimized For exp with an orderby statement.
  template <typename... T, typename F>
  enable_if_orderby<exp_list<T...>>
  operator|(exp_list<select_exp, T...>&& e, F fun)
  {
    //check_exp(e); // Todo.
    auto& v = e.template find<from_tag>().table;
    const auto& order = e.template find<orderby_tag>().order;

    typedef typename std::remove_reference<decltype(v[0])>::type value_type;
    auto tmp = v;
    std::sort(tmp.begin(), tmp.end(),
              [&order] (const value_type& a, const value_type& b)
              { return evaluate(order, a) < evaluate(order, b); });

    for (auto& o : tmp)
      if (where_test(e.template find<where_tag>(), o))
        fun(o);
  }

  // Select iteration with lambda. Parallel version.
  // Optimized For exp with an orderby statement.
  template <typename... T, typename F>
  enable_if_orderby<exp_list<T...>>
  operator||(exp_list<select_exp, T...>&& e, F fun)
  {
    //check_exp(e); // Todo.
    auto& v = e.template find<from_tag>().table;
    const auto& order = e.template find<orderby_tag>().order;

    typedef typename std::remove_reference<decltype(v[0])>::type value_type;
    auto tmp = v;
    std::sort(tmp.begin(), tmp.end(),
              [&order] (const value_type& a, const value_type& b)
              { return evaluate(order, a) < evaluate(order, b); });

    #pragma omp parallel for
    for (int i = 0; i < tmp.size(); i++)
       if (where_test(e.template find<where_tag>(), tmp[i]))
         fun(tmp[i]);
  }

  // Select iteration with lambda. Parallel version.
  // Optimized For exp lists without orderby statement.
  template <typename... T, typename F>
  enable_if_no_orderby<exp_list<T...>>
  operator||(exp_list<select_exp, T...>&& e, F fun)
  {
    //check_exp(e); // Todo.
    auto& v = e.template find<from_tag>().table;

    #pragma omp parallel for
    for (int i = 0; i < v.size(); i++)
       if (where_test(e.template find<where_tag>(), v[i]))
         fun(v[i]);
  }

  // Execute an update statement.
  template <typename... T>
  void
  stl_query_execute(exp_list<update_exp, T...>& e)
  {
    //check_exp(e); // Todo.
    auto to_set = e.template find<set_tag>().attrs;
    auto& v = e.template find<from_tag>().table;

    #pragma omp parallel for
    for (int i = 0; i < v.size(); i++)
       if (where_test(e.template find<where_tag>(), v[i]))
         v[i] = to_set;
  }

  // Execute an insert statement.
  template <typename... T>
  void
  stl_query_execute(exp_list<insert_exp, T...>& e)
  {
    //check_exp(e); // Todo.
    auto& v = e.template find<into_tag>().table;
    typename std::remove_reference<decltype(v[0])>::type n;
    n = e.template find<values_tag>().attrs;
    v.push_back(n);
  }

  // Execute a delete statement.
  template <typename... T>
  void
  stl_query_execute(exp_list<delete_exp, T...>& e)
  {
    //check_exp(e); // Todo.
    auto& v = e.template find<from_tag>().table;
    const auto& w = e.template find<where_tag>();

    typedef typename std::remove_reference<decltype(v[0])>::type value_type;
    auto new_end = std::remove_if(v.begin(), v.end(),
                                  [&] (const value_type& o) -> bool {
                                    return where_test(w, o);
                                  });
    v.erase(new_end, v.end());
  }

}

#endif
