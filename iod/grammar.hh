#pragma once

#include <initializer_list>

namespace iod
{


  template <typename T>
  struct grammar_value_type { typedef T type; };
  template <>
  struct grammar_value_type<const char*> { typedef std::string type; };
  template <int N>
  struct grammar_value_type<const char[N]> { typedef std::string type; };
  template <int N>
  struct grammar_value_type<char[N]> { typedef std::string type; };
  template <typename T>
  struct grammar_value_type<std::initializer_list<T> >
  { typedef std::vector<typename grammar_value_type<T>::type> type; };
  template <typename T>
  using grammar_value_type_t =
    typename grammar_value_type<std::remove_const_t<std::remove_reference_t<T>>>::type;

  template <typename T>
  auto make_value(const T& v)
  {
    return grammar_value_type_t<T>(v);
  }

  template <typename T>
  auto make_value(T&& v)
  {
    return grammar_value_type_t<T>(v);
  }

  template <typename E>
  struct Exp {
    constexpr const E& exact() const { return *static_cast<const E*>(this); }
  };

  template <typename E>
  struct symbol;

  template <typename E>
  struct member_accessible;

  template <typename E>
  struct callable;

  template <typename E>
  struct assignable;

  template <typename M, typename... A>
  struct function_call_exp :
    public member_accessible<function_call_exp<M, A...>>,
    public callable<function_call_exp<M, A...>>,
    public assignable<function_call_exp<M, A...>>,
    public Exp<function_call_exp<M, A...>>
  {
    using assignable<function_call_exp<M, A...>>::operator=;
    function_call_exp() {}
    function_call_exp(const M& m, A... a)
      : method(m), args(a...) {}

    M method;
    std::tuple<A...> args;
  };

  template <typename O, typename M>
  struct member_accessor_exp :
    public member_accessible<member_accessor_exp<O, M>>,
    public callable<member_accessor_exp<O, M>>,
    public assignable<member_accessor_exp<O, M>>,
    public Exp<member_accessor_exp<O, M>>
  {
    using assignable<member_accessor_exp<O, M>>::operator=;

    member_accessor_exp() {}
    member_accessor_exp(const O& o, const M& m) : object(o), member(m) {}
    O object;
    M member;
  };

  template <typename L, typename R>
  struct assign_exp : public Exp<assign_exp<L, R>>
  {
    //assign_exp() {}
    assign_exp(L&& l, R&& r) : left(l), right(r) {}
    assign_exp(const L& l, const R& r) : left(l), right(r) {}
    L left;
    R right;
  };

  template <typename E>
  struct member_accessible
  {
  public:
    // Member accessor
    template <typename S>
    constexpr auto operator[](const symbol<S>& s) const
    {
      return member_accessor_exp<E, S>(*static_cast<const E*>(this), s.exact());
    }

  };

  template <typename E>
  struct callable
  {
  public:
    // Direct call.
    template <typename... A>
    constexpr auto operator()(A... args) const
    {
      return function_call_exp<E, grammar_value_type_t<A>...>(*static_cast<const E*>(this),
                                                              grammar_value_type_t<A>(args)...);
    }

  };

  template <typename E>
  struct assignable
  {
  public:

    template <typename L>
    auto operator=(const L& l) const
    {
      return assign_exp<E, grammar_value_type_t<L>>(*static_cast<const E*>(this), grammar_value_type_t<L>(l));
    }

    template <typename T>
    inline auto operator=(const std::initializer_list<T>& l) const
    {
      return assign_exp<E, 
                        grammar_value_type_t<std::initializer_list<T>>>
        (*static_cast<const E*>(this), grammar_value_type_t<std::initializer_list<T>>(l));
    }

    // Special case for initializer_list<const char*> that cannot implicitely build a vector<string>.
    inline auto operator=(const std::initializer_list<const char*>& l) const
    {
      std::vector<std::string> v;
      for (auto s : l) v.push_back(s);

      return assign_exp<E, std::vector<std::string>>
        (*static_cast<const E*>(this), v);
    }

  };

#define iod_query_declare_binary_op(OP, NAME)                           \
  template <typename A, typename B>                                     \
  struct NAME##_exp : public Exp<NAME##_exp<A, B>>                      \
  {                                                                     \
    NAME##_exp()  {}                                                    \
    NAME##_exp(A a, B b) : lhs(a), rhs(b) {}                            \
    typedef A lhs_type;                                                 \
    typedef B rhs_type;                                                 \
                                                                        \
    lhs_type lhs;                                                       \
    rhs_type rhs;                                                       \
  };                                                                    \
  template <typename A, typename B>                                     \
  inline                                                                \
  std::enable_if_t<std::is_base_of<Exp<A>, A>::value or std::is_base_of<symbol<A>, A>::value or \
                   std::is_base_of<Exp<B>, B>::value or std::is_base_of<symbol<B>, B>::value,\
                   NAME##_exp<A, B >>                                                    \
  operator OP (const A& b, const B& a)                                \
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

}
