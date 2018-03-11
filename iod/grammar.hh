#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include <iod/foreach.hh>

namespace iod
{


  template <typename T>
  struct grammar_value_type { typedef T type; };
  template <typename T>
  struct grammar_value_type<std::initializer_list<T> >
  { typedef std::vector<typename grammar_value_type<T>::type> type; };
  template <typename T>
  struct grammar_value_type<const std::initializer_list<T> >
  { typedef std::vector<typename grammar_value_type<T>::type> type; };
  template <typename T>
  struct grammar_value_type<const std::initializer_list<T>&>
  { typedef std::vector<typename grammar_value_type<T>::type> type; };
  template <typename T>
  using grammar_value_type_t =
    typename grammar_value_type<T>::type;

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
  struct array_subscriptable;

  template <typename E>
  struct callable;

  template <typename E>
  struct assignable;

  template <typename E>
  struct array_subscriptable;

  // template <typename E>
  // struct dividable;

  // template <typename E>
  // struct multipliable;
  
  template <typename... S, typename... T>
  auto foreach_prev(std::tuple<S...>& a1, T&&... args);
  template <typename... S, typename... T>
  auto foreach(std::tuple<S...>& a1, T&&... args);

  template <typename F>
  struct has_transform_iterate
  {
    struct test_lambda
    {
      template <typename T, typename U> std::pair<int, int> operator()(T t, U u);
    };

    template<typename G> 
    static char test(int x, decltype(std::declval<G>().transform_iterate(test_lambda(), std::integral_constant<int, 0>()))* = 0);
    template<typename G> static int test(...);
    static const bool value = sizeof(test<F>(0)) == 1;
  };

  // exp_transform_iterate
  // ================================
  
  // Terminals.
  template <typename E, typename F, typename C>
  decltype(auto) exp_transform_iterate(E& exp, F /*map*/,
                             C ctx,
                             std::enable_if_t<!callable_with<F, E&, C>::value and
                             !has_transform_iterate<E>::value>* = 0)
  {
    return std::make_pair(exp, ctx);
  }
  
  template <typename E, typename F, typename C>
  decltype(auto) exp_transform_iterate(E& exp, F map,
                             C ctx,
                             std::enable_if_t<!callable_with<F, E&, C>::value and
                             has_transform_iterate<E>::value>* = 0)
  {
    return exp.transform_iterate(map, ctx);
  }

  template <typename E, typename F, typename C>
  decltype(auto) exp_transform_iterate(E& exp, F map, C ctx,
                             std::enable_if_t<callable_with<F, E&, C>::value>* = 0)
  {
    return map(exp, ctx);
  }

  // end of exp_transform_iterate
  // ================================


  // exp_transform
  // ================================
  
  // Terminals.
  template <typename E, typename F, typename C>
  decltype(auto) exp_transform(E& exp, F /*map*/, C& /*ctx*/,
                     std::enable_if_t<!callable_with<F, E&, C&>::value and
                     !has_transform_iterate<E>::value>* = 0)
  {
    return exp;
  }
  
  template <typename E, typename F, typename C>
  decltype(auto) exp_transform(E& exp, F map, C& ctx,
                     std::enable_if_t<!callable_with<F, E&, C&>::value and
                     has_transform_iterate<E>::value>* = 0)
  {
    return exp.transform(map, ctx);
  }

  template <typename E, typename F, typename C>
  decltype(auto) exp_transform(E& exp, F map, C& ctx,
                     std::enable_if_t<callable_with<F, E&, C&>::value>* = 0)
  {
    return map(exp, ctx);
  }

  // end of exp_transform
  // ================================
  
  // exp_map_reduce
  // ================================
  
  // Terminals.
  template <typename E, typename N, typename C, typename M, typename R>
  decltype(auto) exp_map_reduce(E& /*exp*/, N neutral, C& /*ctx*/,
                      M /*map*/, R /*reduce*/,
                      std::enable_if_t<!callable_with<M, E&, C&>::value and
                      !has_transform_iterate<E>::value>* = 0)
  {
    return neutral;
  }

  template <typename E, typename N, typename C, typename M, typename R>
  decltype(auto) exp_map_reduce(E& exp, N /*neutral*/, C& ctx,
                      M map, R /*reduce*/,
                      std::enable_if_t<callable_with<M, E&, C&>::value>* = 0)
  {
    return map(exp, ctx);
  }
  
  template <typename E, typename N, typename C, typename M, typename R>
  decltype(auto) exp_map_reduce(E& exp, N neutral, C& ctx,
                      M map, R reduce,
                      std::enable_if_t<!callable_with<M, E&, C&>::value and
                      has_transform_iterate<E>::value>* = 0)
  {
    auto t = foreach(exp.children_tuple()) | [&] (auto n) {
      return exp_map_reduce(n, neutral, ctx, map, reduce);
    };
    return apply(t, reduce);
  }

  // end of exp_map_reduce
  // ================================


  // exp_evaluate
  // ================================
  
  // Terminals.
  template <typename E, typename M, typename C>
  inline decltype(auto) exp_evaluate(E& exp, M /*eval*/, C& /*ctx*/,
                           std::enable_if_t<!callable_with<M, E&, M, C&>::value and
                           !has_transform_iterate<E>::value>* = 0)
  {
    return exp;
  }

  template <typename E, typename M, typename C>
  inline decltype(auto) exp_evaluate(E& exp, M eval, C& ctx,
                           std::enable_if_t<callable_with<M, E&, M, C&>::value>* = 0)
  {
    return eval(exp, eval, ctx);
  }
  
  template <typename E, typename M, typename C>
  inline decltype(auto) exp_evaluate(E& exp, M eval, C& ctx,
                           std::enable_if_t<!callable_with<M, E&, M, C&>::value and
                           has_transform_iterate<E>::value>* = 0)
  {
    return exp.evaluate(eval, ctx);
  }

  // end of exp_evaluate
  // ================================
  
  template <typename M, typename... A>
  struct function_call_exp;

  template <typename M, typename... A>
  auto make_function_call_exp(M m, A... a)
  {
    return function_call_exp<M, std::decay_t<A>...>(m, a...);
  }
  template <typename M, typename... A>
  auto make_function_call_exp(M m, std::tuple<A...> a)
  {
    return function_call_exp<M, std::decay_t<A>...>(m, a);
  }

  template <typename M, typename... A>
  struct function_call_exp :
    public array_subscriptable<function_call_exp<M, A...>>,
    public callable<function_call_exp<M, A...>>,
    public assignable<function_call_exp<M, A...>>,
    public Exp<function_call_exp<M, A...>>
  {
    using assignable<function_call_exp<M, A...>>::operator=;
    using array_subscriptable<function_call_exp<M, A...>>::operator[];

    function_call_exp() {}
    function_call_exp(const M& m, A&&... a)
      : method(m), args(std::forward<A>(a)...) {}
    function_call_exp(const M& m, std::tuple<A...>& a)
      : method(m), args(a) {}
    function_call_exp(const M& m, const std::tuple<A...>& a)
      : method(m), args(a) {}

    template <typename F>
    auto visit(F f) { f(method); iod::foreach(args) | [&] (auto& m) { f(m); }; }
    template <typename F, typename C>
    auto transform(F f, C ctx) {
      return make_function_call_exp(exp_transform(method, f, ctx),
                                    iod::foreach(args) | [&] (auto& m) { return exp_transform(m, f, ctx); });
    }
    template <typename F, typename C>
    decltype(auto) transform_iterate(F f, C ctx)                                  
    {
      auto l = exp_transform_iterate(method, f, ctx);
      auto as = iod::foreach_prev(args, l) | [&] (auto m, auto& prev) {
        return exp_transform_iterate(m, f, prev.second);
      };
      return std::make_pair(make_function_call_exp(l.first,
                                                  iod::foreach(as) | [] (auto& m) -> auto { return m.first; }),
                       std::get<std::tuple_size<decltype(as)>::value - 1>(as).second);
    }
    auto children_tuple() { return std::tuple_cat(std::make_tuple(method), args); }
    template <typename P, typename C>
    decltype(auto) evaluate(P eval, C& ctx) {
      return apply(foreach(args) | [&] (auto a) -> auto&& { return std::move(exp_evaluate(a, eval, ctx)); },
                             exp_evaluate(method, eval, ctx));
    }

    M method;
    std::tuple<A...> args;
  };

  template <typename O, typename M>
  struct array_subscript_exp :
    public array_subscriptable<array_subscript_exp<O, M>>,
    public callable<array_subscript_exp<O, M>>,
    public assignable<array_subscript_exp<O, M>>,
    public Exp<array_subscript_exp<O, M>>
  {
    using assignable<array_subscript_exp<O, M>>::operator=;

    array_subscript_exp() {}
    array_subscript_exp(const O& o, const M& m) : object(o), member(m) {}

    template <typename F>
    auto visit(F f) { return std::make_tuple(f(object), f(member)); }         
    template <typename F, typename C>
    auto transform(F f, C ctx) {
      auto o = exp_transform(object, f, ctx);
      auto m = exp_transform(member, f, ctx);
      return array_subscript_exp<decltype(o), decltype(m)>(o, m);
    }

    template <typename F, typename C>
    auto transform_iterate(F f, C ctx)                                  
    {                                                                   
      auto l = exp_transform_iterate(object, f, ctx);
      auto r = exp_transform_iterate(member, f, l.second);
      return std::make_pair(array_subscript_exp<decltype(l.first), decltype(r.first)>
                            (l.first, r.first), r.second);
    }

    auto children_tuple() { return std::make_tuple(object, member); }

    template <typename P, typename C>
    inline decltype(auto) evaluate(P eval, C& ctx) {
      return exp_evaluate(object, eval, ctx)[exp_evaluate(member, eval, ctx)];
    }
    
    O object;
    M member;
  };

  template <typename L, typename R>
  struct assign_exp : public Exp<assign_exp<L, R>>
  {
    typedef L left_t;
    typedef R right_t;

    assign_exp(L&& l, R&& r) : left(l), right(r) {}
    assign_exp(const L& l, R&& r) : left(l), right(std::forward<R>(r)) {}

    template <typename F>
    auto visit(F f) { return std::make_tuple(f(left), f(right)); }

    template <typename F, typename C>
    auto transform(F f, C ctx) {
      auto l = exp_transform(left, f, ctx);
      auto r = exp_transform(right, f, ctx);      
      return assign_exp<decltype(l), decltype(r)>(l, r);
    }

    template <typename F, typename C>
    decltype(auto) transform_iterate(F f, C ctx)                                  
    {                                                                   
      auto l = exp_transform_iterate(left, f, ctx);
      auto r = exp_transform_iterate(right, f, l.second);
      return std::make_pair(assign_exp<decltype(l.first), decltype(r.first)>
                            (l.first, r.first), r.second);
    }
    auto children_tuple() { return std::make_tuple(left, right); }
    template <typename M, typename C>
    inline decltype(auto) evaluate(M eval, C& ctx) {
      return exp_evaluate(left, eval, ctx) = exp_evaluate(right, eval, ctx);
    }
    L left;
    R right;
  };
  
  template <typename E>
  struct array_subscriptable
  {
  public:
    // Member accessor
    template <typename S>
    constexpr auto operator[](const S& s) const
    {
      return array_subscript_exp<E, S>(*static_cast<const E*>(this), s);
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
                                                              std::forward<grammar_value_type_t<A>>(args)...);
    }

  };
 
  template <typename E>
  struct assignable
  {
  public:

    template <typename L>
    auto operator=(L&& l) const
    {
      return assign_exp<E, L>(static_cast<const E&>(*this), std::forward<L>(l));
    }

    template <typename L>
    auto operator=(L&& l)
    {
      return assign_exp<E, L>(static_cast<E&>(*this), std::forward<L>(l));
    }
    
    template <typename T>
    auto operator=(const std::initializer_list<T>& l) const
    {
      return assign_exp<E, std::vector<T>>(static_cast<const E&>(*this), std::vector<T>(l));
    }

  };

#define iod_query_declare_binary_op(OP, NAME)                           \
  template <typename A, typename B>                                     \
  struct NAME##_exp :                                                   \
   public assignable<NAME##_exp<A, B>>,                                 \
  public Exp<NAME##_exp<A, B>>                                          \
  {                                                                     \
    using assignable<NAME##_exp<A, B>>::operator=; \
    NAME##_exp()  {}                                                    \
    NAME##_exp(A a, B b) : lhs(a), rhs(b) {}                            \
    typedef A lhs_type;                                                 \
    typedef B rhs_type;                                                 \
    template <typename F>                                               \
    auto visit(F f) { return std::make_tuple(f(lhs), f(rhs)); }         \
                                                                        \
    template <typename F, typename C>                                   \
    auto transform_iterate(F f, C ctx)                                  \
    {                                                                   \
      auto l = exp_transform_iterate(lhs, f, ctx);                      \
      auto r = exp_transform_iterate(rhs, f, l.second);                 \
      return std::make_pair(NAME##_exp<decltype(l.first), decltype(r.first)>(l.first, r.first), r.second); \
    }                                                                   \
                                                                        \
    template <typename F, typename C>                                   \
    auto transform(F f, C ctx) {                                        \
      auto l = exp_transform(lhs, f, ctx);                              \
      auto r = exp_transform(rhs, f, ctx);                              \
    return NAME##_exp<decltype(l), decltype(r)>(l, r); }                \
    auto children_tuple() { return std::make_tuple(lhs, rhs); }         \
    template <typename M, typename C>                                   \
    inline decltype(auto) evaluate(M eval, C& ctx)  { return exp_evaluate(lhs, eval, ctx) OP  exp_evaluate(rhs, eval, ctx); } \
    lhs_type lhs;                                                       \
    rhs_type rhs;                                                       \
  };                                                                    \
  template <typename A, typename B>                                     \
  inline                                                                \
  std::enable_if_t<std::is_base_of<Exp<A>, A>::value or std::is_base_of<symbol<A>, A>::value or \
                   std::is_base_of<Exp<B>, B>::value or std::is_base_of<symbol<B>, B>::value,\
                   NAME##_exp<A, B >>                                                    \
  operator OP (const A& b, const B& a)                                \
  { return NAME##_exp<std::decay_t<A>, std::decay_t<B>>{b, a}; }

  iod_query_declare_binary_op(+, plus);
  iod_query_declare_binary_op(-, minus);
  iod_query_declare_binary_op(*, mult);
  iod_query_declare_binary_op(/, div);
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

