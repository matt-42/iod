#ifndef IOD_QUERY_HH_
# define IOD_QUERY_HH_

# include <algorithm>
# include <tuple>
# include "iod.hh"


namespace iod_query
{

  template <typename A>
  struct Exp
  {
    A& exact() { return *static_cast<A*>(this); }
    const A& exact() const { return *static_cast<const A*>(this); }
  };

  enum { where_class, orderby_class };

  template <typename A>
  struct qvalue
  {
    typedef A value_type;
    qvalue(A a) : value(a) {}
    A value;
  };

  template <typename A, typename B>
  struct inf_exp
  {
    typedef A lhs_type;
    typedef B rhs_type;

    lhs_type lhs;
    rhs_type rhs;
  };

  template <typename A, typename B>
  struct sup_exp
  {
    typedef A lhs_type;
    typedef B rhs_type;

    sup_exp(A a, B b) : lhs(a), rhs(b) {}

    lhs_type lhs;
    rhs_type rhs;
  };

  template <typename A, typename B>
  struct eq_exp
  {
    typedef A lhs_type;
    typedef B rhs_type;

    eq_exp(A a, B b) : lhs(a), rhs(b) {}

    lhs_type lhs;
    rhs_type rhs;
  };

  template <typename C>
  struct where_exp : public Exp<where_exp<C>>
  {
    enum { node_class = where_class };

    typedef C condition_type;
    where_exp(C c) : condition(c) {}

    C condition;
  };

  template <typename O>
  struct orderby_exp : public Exp<orderby_exp<O>>
  {
    enum { node_class = orderby_class };

    typedef O order_type;
    orderby_exp(O o) : order(o) {}

    order_type order;
  };

  // Operator overload to build list of exps.

  template <typename C>
  where_exp<C> where(const C& c) { return where_exp<C>(c); }

  template <typename C>
  orderby_exp<C> orderby(const C& c) { return orderby_exp<C>(c); }

  //exp_list<sup_exp<Symbol<A>, qvalue<int> > >


  // Iterator on expresion list.
  template <unsigned I, unsigned N>
  struct exp_list_iterator
  {
    typedef exp_list_iterator<I + 1, N> next;

    template <typename L>
    static const typename L::template get_type<I>& get(const L& l) { return l.template get<I>(); }
  };

  template <unsigned N>
  struct exp_list_iterator<N, N>
  {
  };


  // List of exps.
  template <typename... T>
  struct exp_list
  {
    typedef exp_list<T...> self;
    typedef exp_list_iterator<0, sizeof...(T)> begin_type;
    typedef exp_list_iterator<sizeof...(T), sizeof...(T)> end_type;
    typedef std::tuple<T...> tuple_type;

    enum { size = sizeof...(T) };

    exp_list() {}
    exp_list(T... args) : list(args...) {}

    template <unsigned N>
    using get_type = typename std::tuple_element<N, tuple_type>::type;

    // Get Nth element of the list.
    template <unsigned N>
    const get_type<N>& get() const { return std::get<N>(list); }

    std::tuple<T...> list;
  };

  // =====================================
  // Evaluation of queries on std::vector.
  // =====================================

  template <typename IT, typename L, typename I, typename O>
  typename std::enable_if<std::is_same<IT, typename L::end_type>::value, void>::type
  evaluate_iterate(L& l, I& in, O& out)
  {
  }

  template <typename IT, typename L, typename I, typename O>
  typename std::enable_if<!std::is_same<IT, typename L::end_type>::value, void>::type
  evaluate_iterate(L& l, I& in, O& out)
  {
    evaluate(IT::get(l), in, out);
    evaluate_iterate<typename IT::next>(l, in, out);
  }

  template <typename T, typename... L>
  void
  evaluate(const exp_list<L...>& e, const std::vector<T>& in, std::vector<T>& out)
  {
    evaluate_iterate<typename exp_list<L...>::begin_type>(e, in, out);
  }

  template <typename T, typename C>
  void
  evaluate(where_exp<C> e, const std::vector<T>& in, std::vector<T>& out)
  {
    C cond = e.condition;
    for (const auto& p : in)
    {
      if (evaluate(cond, p)) out.push_back(p);
    }
  }

  template <typename A, typename T>
  inline A
  evaluate(const qvalue<A>& v, const T&)
  {
    return v.value;
  }

  template <typename A, typename T>
  inline auto
  evaluate(const Symbol<A>&, const T& elt) -> decltype(elt.template get<typename A::name>())
  {
    return elt.template get<typename A::name>();
  }

  template <typename T, typename C>
  void
  evaluate(const orderby_exp<C>& e, const std::vector<T>& in, std::vector<T>& out)
  {
    std::sort(out.begin(), out.end(),
              [&e] (const T& a, const T& b) { return evaluate(e.order, a) < evaluate(e.order, b); });
  }


  template <typename A, typename B, typename T>
  inline bool
  evaluate(sup_exp<A, B> e, const T& elt)
  {
    return evaluate(e.lhs, elt) > evaluate(e.rhs, elt);
  }

  template <typename A, typename B, typename T>
  inline bool
  evaluate(eq_exp<Symbol<A>, Symbol<B>> e, const T& elt) {
    return evaluate(e.lhs, elt) == evaluate(e.rhs, elt);
  }

  template <typename A, typename B, typename T>
  inline bool
  evaluate(eq_exp<Symbol<A>, qvalue<B>> e, const T& elt) { return elt.template get<typename A::name>() == e.rhs.value; }

  template <typename T, typename U = void>
  struct to_query_exp_type
  {
    typedef qvalue<T> type;
  };

  template <typename T>
  struct to_query_exp_type<T, typename std::enable_if<T::is_iod_attribute, void>::type>
  {
    typedef Symbol<T> type;
  };

  template <typename T>
  using to_query_exp_type_t = typename to_query_exp_type<T>::type;



  // Operator, launch the evaluation.
  template <typename T, typename... E>
  std::vector<T> operator||(const std::vector<T>& v, const exp_list<E...>& exp)
  {
    std::vector<T> out;
    evaluate(exp, v, out);
    return out;
  }

  // Operator, launch the evaluation.
  template <typename T, typename E>
  std::vector<T> operator||(const std::vector<T>& v, const Exp<E>& exp)
  {
    return v || exp_list<E>(exp.exact());
  }

  // Operator overload to build list of exps.
  template <typename A, typename B>
  inline sup_exp<Symbol<A>, to_query_exp_type_t<B>> operator>(const Symbol<A>& a, const B& b)
  { return sup_exp<Symbol<A>, to_query_exp_type_t<B>>(a, b); }

  template <typename A, typename B>
  inline eq_exp<Symbol<A>, to_query_exp_type_t<B>> operator==(const Symbol<A>& a, const B& b)
  { return eq_exp<Symbol<A>, to_query_exp_type_t<B>>(a, b); }

}

using iod_query::operator>;
using iod_query::operator==;


#endif
                                                                        \
  // template <typename A, typename B>                                  \
  // inline NAME##_exp<Symbol<A>, to_query_exp_type_t<B>> operator OP (const Symbol<A>& a, const B& b) \
  // { return NAME##_exp<Symbol<A>, to_query_exp_type_t<B>>(a, b); }    \
  // template <typename A, typename B>                                  \
  // inline NAME##_exp<to_query_exp_type_t<B>, Symbol<A>> operator OP (const B& b, const Symbol<A>& a) \
  // { return NAME##_exp<to_query_exp_type_t<B>, Symbol<A>>(b, a); }    \
