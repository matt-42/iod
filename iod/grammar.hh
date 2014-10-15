#pragma once

namespace iod
{
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

  template <typename M, typename... A>
  struct function_call_exp :
    public member_accessible<function_call_exp<M, A...>>,
    public callable<function_call_exp<M, A...>>,
    public Exp<function_call_exp<M, A...>>
  {
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
    public Exp<member_accessor_exp<O, M>>
  {
    member_accessor_exp() {}
    member_accessor_exp(const O& o, const M& m) : object(o), member(m) {}
    O object;
    M member;
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
      return function_call_exp<E, A...>(*static_cast<const E*>(this), args...);
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
