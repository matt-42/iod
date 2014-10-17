#include <type_traits>
#include <iostream>
#include <cassert>
#include <iod/callable_traits.hh>

float fun(int, float)
{
  return 1.f;
}

int main()
{
  using namespace iod;

  auto lambda = [] () {};

  typedef callable_traits<int> C_int;
  typedef callable_traits<decltype(fun)> C_fun;
  typedef callable_traits<decltype(lambda)> C_lambda;


  static_assert(internal::has_parenthesis_operator<int>::value == false, "error");
  static_assert(internal::has_parenthesis_operator<decltype(lambda)>::value == true, "error");
  static_assert(internal::has_parenthesis_operator<decltype(fun)>::value == false, "error");

  static_assert(std::is_same<C_int::is_callable, std::false_type>::value, "error");
  static_assert(std::is_same<C_lambda::is_callable, std::true_type>::value, "error");
  static_assert(std::is_same<C_fun::is_callable, std::true_type>::value, "error");

  static_assert(std::is_same<C_int::return_type, void>::value, "error");
  static_assert(std::is_same<C_lambda::return_type, void>::value, "error");
  static_assert(std::is_same<C_fun::return_type, float>::value, "error");

  static_assert(std::is_same<C_int::arguments_tuple_type, std::tuple<>>::value, "error");
  static_assert(std::is_same<C_lambda::arguments_tuple_type, std::tuple<>>::value, "error");
  static_assert(std::is_same<C_fun::arguments_tuple_type, std::tuple<int, float>>::value, "error");

  static_assert(C_int::arity == 0, "error");
  static_assert(C_lambda::arity == 0, "error");
  static_assert(C_fun::arity == 2, "error");
}
