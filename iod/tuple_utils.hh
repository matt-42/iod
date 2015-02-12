#pragma once

#include <tuple>
#include <iod/tags.hh>

namespace iod
{

  template <int N, typename T, typename X>
  struct tuple_find_type2;

  // template <int N, typename X>
  // struct tuple_find_type2<N, std::tuple<>, X> : public std::integral_constant<int, -1> {};
  template <int N, typename X>
  struct tuple_find_type2<N, std::tuple<>, X> : public not_found {};

  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, X> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X&, T...>, X> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X&&, T...>, X> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<const X&, T...>, X> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<const X, T...>, X> : public std::integral_constant<int, N> {};

  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, X&> : public std::integral_constant<int, N> {};
  template <int N, typename... T, typename X>
  struct tuple_find_type2<N, std::tuple<X, T...>, X&&> : public std::integral_constant<int, N> {};
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
    return std::get<tuple_find_type<std::remove_const_t<std::remove_reference_t<T>>,
                                    std::remove_const_t<std::remove_reference_t<E>>>::value>(tuple);
  }


  template <typename T, typename U>
  struct tuple_embeds : public std::false_type {};

  template <typename U>
  struct tuple_embeds<std::tuple<>, U> : public std::false_type {};

  template <typename T1, typename... T>
  struct tuple_embeds<std::tuple<T1, T...>, T1> : public std::true_type {};

  template <typename T1, typename... T, typename U>
  struct tuple_embeds<std::tuple<T1, T...>, U> : public tuple_embeds<std::tuple<T...>, U> {};


  template <typename T>
  struct tuple_remove_references;
  template <typename... T>
  struct tuple_remove_references<std::tuple<T...>> { typedef std::tuple<std::remove_reference_t<T>...> type; };

  template <typename T>
  using tuple_remove_references_t = typename tuple_remove_references<T>::type;
  
  template <typename T>
  struct tuple_remove_references_and_const;
  template <typename... T>
  struct tuple_remove_references_and_const<std::tuple<T...>> {
    typedef std::tuple<std::remove_const_t<std::remove_reference_t<T>>...> type;
  };

  template <typename T>
  using tuple_remove_references_and_const_t = typename tuple_remove_references_and_const<T>::type;

  template <typename T, typename U, typename E>
  struct tuple_remove_element2;

  template <typename... T, typename... U, typename E1>
  struct tuple_remove_element2<std::tuple<E1, T...>, std::tuple<U...>, E1> :
    public tuple_remove_element2<std::tuple<T...>, std::tuple<U...>, E1> {};

  template <typename... T, typename... U, typename T1, typename E1>
  struct tuple_remove_element2<std::tuple<T1, T...>, std::tuple<U...>, E1> :
    public tuple_remove_element2<std::tuple<T...>, std::tuple<U..., T1>, E1> {};
    
  template <typename... U, typename E1>
  struct tuple_remove_element2<std::tuple<>, std::tuple<U...>, E1>
  {
    typedef std::tuple<U...> type;
  };

  template <typename T, typename E>
  struct tuple_remove_element : public tuple_remove_element2<T, std::tuple<>, E> {};

  template <typename T, typename... E>
  struct tuple_remove_elements;

  template <typename... T, typename E1, typename... E>
  struct tuple_remove_elements<std::tuple<T...>, E1, E...>
  {
    typedef typename tuple_remove_elements<typename tuple_remove_element<std::tuple<T...>, E1>::type, E...>::type
    type;
  };

  template <typename... T>
  struct tuple_remove_elements<std::tuple<T...>>
  {
    typedef std::tuple<T...> type;
  };

  template <typename T, typename... E>
  using tuple_remove_elements_t = typename tuple_remove_elements<T, E...>::type;
  
}
