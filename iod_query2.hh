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


  // Not found struct.
  // Return by meta functions when an element is not found.
  struct not_found { enum { tag = -1 }; };

  template <typename A>
  using is_exp = std::is_base_of<Exp<A>, A>;

  enum tags {
    where_tag,
    orderby_tag,
    from_tag,
    set_tag,
    into_tag,
    values_tag,

    select_tag,
    update_tag,
    insert_tag,
    delete_tag
  };

  struct select_exp { enum { tag = select_tag }; };
  struct update_exp { enum { tag = update_tag }; };
  struct insert_exp { enum { tag = insert_tag }; };
  struct delete_exp { enum { tag = delete_tag }; };

  template <typename A>
  struct qvalue
  {
    typedef A value_type;
    qvalue(const A& a) : value(a) {}
    const A& value;
  };

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

  template <typename A>
  struct to_query_exp_type<A, typename std::enable_if<is_exp<A>::value, void>::type>
  {
    typedef A type;
  };

  template <typename T>
  using to_query_exp_type_t = typename to_query_exp_type<T>::type;


#define iod_query_declare_binary_op(OP, NAME)                           \
  template <typename A, typename B>                                     \
  struct NAME##_exp : public Exp<NAME##_exp<A, B>>                      \
  {                                                                     \
    NAME##_exp(A a, B b) : lhs(a), rhs(b) {}                            \
    typedef A lhs_type;                                                 \
    typedef B rhs_type;                                                 \
                                                                        \
    lhs_type lhs;                                                       \
    rhs_type rhs;                                                       \
  };                                                                    \
  template <typename A, typename B>                                     \
  inline                                                                \
  typename                                                              \
  std::enable_if<is_iod_attribute<A>::value || is_iod_attribute<B>::value,          \
                 NAME##_exp<to_query_exp_type_t<A>, to_query_exp_type_t<B> > \
                 >::type                                                \
  operator OP (const A& b, const B& a)                                  \
  { return NAME##_exp<to_query_exp_type_t<A>, to_query_exp_type_t<B>>{b, a}; } \
                                                                        \
  template <typename A, typename B>                                     \
  inline                                                                \
  typename                                                              \
  std::enable_if<is_exp<A>::value && is_exp<B>::value,                  \
                 NAME##_exp<A, B >                                      \
                 >::type                                                \
  operator OP (const A& b, const B& a)                                  \
  { return NAME##_exp<A, B>{b, a}; }

  iod_query_declare_binary_op(+, plus);
  iod_query_declare_binary_op(-, minus);
  iod_query_declare_binary_op(*, mult);
  iod_query_declare_binary_op(<<, shiftl);
  iod_query_declare_binary_op(>>, shiftr);
  iod_query_declare_binary_op(<, inf);
  iod_query_declare_binary_op(<=, inf_eq);
  iod_query_declare_binary_op(>, sup);
  iod_query_declare_binary_op(>=, sup_eq);
  iod_query_declare_binary_op(==, eq);
  iod_query_declare_binary_op(!=, neq);
  iod_query_declare_binary_op(&, logical_and);
  iod_query_declare_binary_op(^, logical_xor);
  iod_query_declare_binary_op(|, logical_or);
  iod_query_declare_binary_op(&&, and);
  iod_query_declare_binary_op(||, or);

  template <typename C>
  struct where_exp : public Exp<where_exp<C>>
  {
    enum { tag = where_tag };

    typedef C condition_type;
    where_exp(C c) : condition(c) {}

    C condition;
  };

  template <typename O>
  struct orderby_exp : public Exp<orderby_exp<O>>
  {
    enum { tag = orderby_tag };
    typedef O order_type;
    orderby_exp(O o) : order(o) {}
    order_type order;
  };
  template <typename E> using has_no_orderby = std::is_same<typename E::template find_type<orderby_tag>, not_found>;

  template <typename C>
  struct from_exp : public Exp<from_exp<C>>
  {
    enum { tag = from_tag };
    typedef C table_type;
    from_exp(C& c) : table(c) {}
    C& table;
  };

  template <typename C>
  struct into_exp : public Exp<into_exp<C>>
  {
    enum { tag = into_tag };
    typedef C table_type;
    into_exp(C& c) : table(c) {}
    C& table;
  };

  template <typename... T>
  struct set_exp : public Exp<set_exp<T...>>
  {
    enum { tag = set_tag };
    set_exp(T... t) : attrs(t...) {}
    iod_object<T...> attrs;
  };


  template <typename... T>
  struct values_exp : public Exp<values_exp<T...>>
  {
    enum { tag = values_tag };
    values_exp(T... t) : attrs(t...) {}
    iod_object<T...> attrs;
  };

  // struct find_tag_pos
  // Find the element with tag TAG.
  template <unsigned N, unsigned S, typename E, unsigned TAG, typename X = void>
  struct find_tag_pos
  {
    typedef typename find_tag_pos<N+1, S, E, TAG>::type type;
  };

  template <unsigned S, typename E, unsigned TAG>
  struct find_tag_pos<S, S, E, TAG, void>
  {
    struct type { enum { pos = -1 }; }; // Not found.
  };


  // struct get_nth_element
  // Find the nth element of a tuple.
  // Return not_found is I < 0 or I > tuple_size.
  template< std::size_t I, class T >
  struct get_nth_element;

  template< std::size_t I, class Head, class... Tail >
  struct get_nth_element<I, std::tuple<Head, Tail...>>
    : get_nth_element<I-1, std::tuple<Tail...>> { };

  template< class Head, class... Tail >
  struct get_nth_element<0, std::tuple<Head, Tail...>> {
    typedef Head type;
  };

  template<std::size_t I >
  struct get_nth_element<I, std::tuple<>> {
    typedef not_found type;
  };

  template<unsigned N, typename E>
  using get_nth_element_t = typename get_nth_element<N, E>::type;

  template<int N, typename E>
  struct get_helper
  {
    static auto run(E& e)
      -> get_nth_element_t<N, E>&
    {
      return std::get<N>(e);
    }

    static auto run(const E& e)
      -> const get_nth_element_t<N, E>&
    {
      return std::get<N>(e);
    }
  };

  template<typename E>
  struct get_helper<-1, E>
  {
    static auto run(E& e)
      -> not_found
    {
      not_found();
    }
    static auto run(const E& e)
      -> not_found
    {
      not_found();
    }
  };

  template<int N, typename E>
  get_nth_element_t<N, E>& get_(E& e)
  {
    return get_helper<N, E>::run(e);
  }

  template<int N, typename E>
  const get_nth_element_t<N, E>& get_(const E& e)
  {
    return get_helper<N, E>::run(e);
  }

  template <unsigned N, unsigned S, typename E, unsigned TAG>
  struct find_tag_pos<N, S, E, TAG,
                      typename std::enable_if<get_nth_element_t<N, typename E::tuple_type>::tag == TAG, void>::type>
  {
    struct type { enum { pos = N }; }; // Type found at position N.
  };

  template <int N, typename E>
  struct pos_to_type { typedef typename E::template get_type<N> type; };
  template <typename E>
  struct pos_to_type<-1, E> { typedef not_found type; };

  // List of exps.
  template <typename... T>
  struct exp_list
  {
    typedef exp_list<T...> self;
    // typedef exp_list_iterator<0, sizeof...(T)> begin_type;
    // typedef exp_list_iterator<sizeof...(T), sizeof...(T)> end_type;
    typedef std::tuple<T...> tuple_type;

    enum { size = sizeof...(T) };

    exp_list() {}
    exp_list(T... args) : list(args...) {}
    exp_list(std::tuple<T...> t) : list(t) {}

    // Get Nth element of the list.
    template <int N>
    using get_type = typename std::tuple_element<N, tuple_type>::type;

    template <unsigned TAG>
    using tag_pos = typename find_tag_pos<0, size, self, TAG>::type;

    template <unsigned TAG>
    using find_type = typename pos_to_type<tag_pos<TAG>::pos, self>::type;

    template <unsigned TAG>
    find_type<TAG> find()
    {
      return get_<tag_pos<TAG>::pos, tuple_type>(list);
    }

    template <unsigned TAG>
    const find_type<TAG>& find() const
    {
      return get_<tag_pos<TAG>::pos, tuple_type>(list);
    }

    template <unsigned N>
    get_type<N>& get_nth()
    {
      return get_<N, tuple_type>(list);
    }

    template <unsigned N>
    const get_type<N>& get_nth() const
    {
      return get_<N, tuple_type>(list);
    }

    template <unsigned TAG>
    struct has_tag
    {
      enum { value = tag_pos<TAG>::pos != -1 };
    };

    template <typename P>
    using push_type = exp_list<T..., P>;

    template <typename B>
    exp_list<T..., B> push(B b)
    {
      return exp_list<T..., B>(std::tuple_cat(list, std::tuple<B>(b)));
    }

    // Query exps.
    template <typename C>
    push_type<where_exp<C>> where(const C& c) { return push(where_exp<C>(c)); }
    template <typename C>
    push_type<orderby_exp<C>> orderby(const C& c) { return push(orderby_exp<C>(c)); }
    template <typename C>
    push_type<from_exp<C>> from(C& c) { return push(from_exp<C>(c)); }
    template <typename C>
    push_type<into_exp<C>> into(C& c) { return push(into_exp<C>(c)); }
    template <typename... C>
    push_type<set_exp<C...>> set(const C&... c) { return push(set_exp<C...>(c...)); }
    template <typename... C>
    push_type<values_exp<C...>> values(const C&... c) { return push(values_exp<C...>(c...)); }

    // Conversion to a vector. Shortcut for stl_query.
    template <typename E>
    operator std::vector<E>()
    {
      static_assert(std::is_same<get_type<0>, select_exp>::value,
                    "Only SELECT requests can be converted in vectors");

      std::vector<E> out;
      *this | [&] (E& p) { out.push_back(p); };
      return out;
    }

    // Execute a stl query.
    void execute()
    {
      stl_query_execute(*this);
    }

    std::tuple<T...> list;
  };


  exp_list<select_exp> Select;
  exp_list<update_exp> Update;
  exp_list<insert_exp> Insert;
  exp_list<delete_exp> Delete;
  // struct update : public exp_list<update> {} Update;
  // struct delete_ : public exp_list<delete_> {} Delete;
  // struct insert : public exp_list<insert> {} Insert;


  // Operator overload to build list of exps.
  // template <typename A, typename B>
  // inline sup_exp<Symbol<A>, to_query_exp_type_t<B>> operator>(const Symbol<A>& a, const B& b)
  // { return sup_exp<Symbol<A>, to_query_exp_type_t<B>>(a, b); }

  // template <typename A, typename B>
  // inline eq_exp<Symbol<A>, to_query_exp_type_t<B>> operator==(const Symbol<A>& a, const B& b)
  // { return eq_exp<Symbol<A>, to_query_exp_type_t<B>>(a, b); }

}

using iod_query::operator>;
using iod_query::operator==;

#endif
