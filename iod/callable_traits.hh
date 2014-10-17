#pragma once

namespace iod
{
  namespace internal
  {
    template <typename T>
    struct has_parenthesis_operator
    {
      template<typename C> 
      static char test(int x, decltype(&C::operator())* = 0);
      template<typename C>
      static int test(...);
      static const bool value = sizeof(test<T>(0)) == 1;
    };

  }

  // Traits on callable (function, functors and lambda functions).

  // is_callable<F>::value = true if F is callable.
  // callable_arity<F>::value = N if F takes N arguments.
  // callable_arguments_tuple<F>::type = tuple<Arg1, ..., ArgN>

  template <typename F, typename X = void>
  struct callable_traits
  {
    typedef std::false_type is_callable;
    static const int arity = 0;
    typedef std::tuple<> arguments_tuple_type;
    typedef void return_type;
  };

  template <typename F>
  struct callable_traits<F, std::enable_if_t<internal::has_parenthesis_operator<F>::value>>
  {
    typedef callable_traits<decltype(&F::operator())> super;
    typedef std::true_type is_callable;
    static const int arity = super::arity;
    typedef typename super::arguments_tuple_type arguments_tuple_type;
    typedef typename super::return_type return_type;
  };

  template <typename C, typename R, typename... ARGS>
  struct callable_traits<R (C::*)(ARGS...) const>
  {
    typedef std::true_type is_callable;
    static const int arity = sizeof...(ARGS);
    typedef std::tuple<ARGS...> arguments_tuple_type;
    typedef R return_type;
  };

  template <typename R, typename... ARGS>
  struct callable_traits<R(ARGS...)>
  {
    typedef std::true_type is_callable;
    static const int arity = sizeof...(ARGS);
    typedef std::tuple<ARGS...> arguments_tuple_type;
    typedef R return_type;
  };

  // callable_arity<F>::value = N if F takes N arguments.

}
