#pragma once

#include <tuple>

namespace iod
{

  template <int N, typename T, typename X>
  struct tuple_find_type2;

  template <int N, typename X>
  struct tuple_find_type2<N, std::tuple<>, X> : public std::integral_constant<int, -1> {};

  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, X> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X&, T...>, X> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<const X&, T...>, X> : public std::integral_constant<int, N> {};

  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, X&> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, const X&> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, const X> : public std::integral_constant<int, N> {};
  
  template <int N, typename T1, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<T1, T...>, X> : public tuple_find_type2<N + 1, std::tuple<T...>, X> {};

  template <typename T, typename X>
  struct tuple_find_type : public tuple_find_type2<0, T, X> {};

  template <typename E, typename T>
  auto& tuple_get_by_type(T&& tuple)    
  {
    return std::get<tuple_find_type<std::remove_reference_t<T>,
                                    std::remove_reference_t<E>>::value>(tuple);
  }


  template <typename T, typename U>
  struct tuple_embeds : public std::false_type {};

  template <typename U>
  struct tuple_embeds<std::tuple<>, U> : public std::false_type {};

  template <typename T1, typename... T>
  struct tuple_embeds<std::tuple<T1, T...>, T1> : public std::true_type {};

  template <typename T1, typename... T, typename U>
  struct tuple_embeds<std::tuple<T1, T...>, U> : public tuple_embeds<std::tuple<T...>, U> {};
  
}
