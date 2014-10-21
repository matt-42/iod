#pragma once

#include <tuple>
#include <iod/sio.hh>

namespace iod
{

  namespace internal
  {

    template<unsigned N, unsigned SIZE, typename T, typename F, typename... U>
    inline
    auto
    tuple_apply(std::enable_if_t<(N == SIZE), int>*, T& t, F f, U&&... u)
    {
      return f(std::forward<U>(u)...);
    }

    template<unsigned N, unsigned SIZE, typename T, typename F, typename... U>
    inline
    auto
    tuple_apply(std::enable_if_t<(N < SIZE), int>*, T& t, F f, U&&... u)
    {
      return tuple_apply<N + 1, SIZE, T, F>(0, t, f, u..., std::get<N>(t));
    }

  }


  template<typename F, typename... T>
  inline auto apply(std::tuple<T...>& t, F f)
  {
    return internal::tuple_apply<0, sizeof...(T), std::tuple<T...>>(0, t, f);
  }

  template<typename F, typename... T>
  inline auto apply(const std::tuple<T...>& t, F f)
  {
    return internal::tuple_apply<0, sizeof...(T), const std::tuple<T...>>(0, t, f);
  }



  template <typename F, typename... T>
  auto apply(const sio<T...>& o, F f)
  {
    return f(static_cast<const T*>(&o)->value()...);
  }

  template <typename F, typename... T>
  auto apply(sio<T...>& o, F f)
  {
    return f(static_cast<T*>(&o)->value()...);
  }



  template <typename F, typename... T>
  auto apply_members(const sio<T...>& o, F f)
  {
    return f(*static_cast<const T*>(&o)...);
  }

  template <typename F, typename... T>
  auto apply_members(sio<T...>& o, F f)
  {
    return f(*static_cast<T*>(&o)...);
  }


  // Proxy apply.
 
  namespace internal
  {

    template<unsigned N, unsigned SIZE, typename T, typename F, typename G,  typename... U>
    inline
    auto
    tuple_proxy_apply(std::enable_if_t<(N == SIZE), int>*, T& t, G g, F f, U&&... u)
    {
      return f(g(std::forward<U>(u))...);
    }

    template<unsigned N, unsigned SIZE, typename T, typename F, typename G,  typename... U>
    inline
    auto
    tuple_proxy_apply(std::enable_if_t<(N < SIZE), int>*, T& t, G g, F f, U&&... u)
    {
      return tuple_proxy_apply<N + 1, SIZE, T, F>(0, t, g, f,
                                                  u...,
                                                  std::get<N>(t));
                                                  // std::forward<U>(u)...,
                                                  // std::forward<decltype(std::get<N>(t))>(std::get<N>(t)));
      
    }

  }


  template<typename F, typename G,  typename... T>
  inline auto proxy_apply(std::tuple<T...>& t, G g, F f)
  {
    return internal::tuple_proxy_apply<0, sizeof...(T), std::tuple<T...>>(0, t, g, f);
  }

  template<typename F, typename G,  typename... T>
  inline auto proxy_apply(const std::tuple<T...>& t, G g, F f)
  {
    return internal::tuple_proxy_apply<0, sizeof...(T), const std::tuple<T...>>(0, t, g, f);
  }



  template <typename F, typename G,  typename... T>
  auto proxy_apply(const sio<T...>& o, G g, F f)
  {
    return f(g(static_cast<const T*>(&o)->value())...);
  }

  template <typename F, typename G,  typename... T>
  auto proxy_apply(sio<T...>& o, G g, F f)
  {
    return f(g(static_cast<T*>(&o)->value())...);
  }



  template <typename F, typename G,  typename... T>
  auto proxy_apply_members(const sio<T...>& o, G g, F f)
  {
    return f(g(*static_cast<const T*>(&o))...);
  }

  template <typename F, typename G,  typename... T>
  auto proxy_apply_members(sio<T...>& o, G g, F f)
  {
    return f(g(*static_cast<T*>(&o))...);
  }

}
