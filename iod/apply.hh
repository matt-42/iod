#pragma once

#include <tuple>

namespace iod
{
  template <typename ...T>
  struct sio;

  template <typename T>
  struct forward_
  {
    T& t;
  };

  template <typename T>
  struct forward_<const T>
  {
    const T& t;
  };

  template <typename T>
  decltype(auto) forward(T& t) { return forward_<T>{t}; }
  template <typename T>
  decltype(auto) forward(const T& t) { return forward_<const T>{t}; }

  template <int N>
  struct run_apply;
  
  template <int N, int TP>
  struct run_tuple
  {
    template <typename... M, typename... T>
    static decltype(auto) run(std::tuple<M...>& o, T&&... t)
    { return run_tuple<N, TP - 1>::run(o, std::forward<T>(t)..., std::get<sizeof...(M) - TP>(o)); }
    template <typename... M, typename... T>
    static decltype(auto) run(const std::tuple<M...>& o, T&&... t)
    { return run_tuple<N, TP - 1>::run(o, std::forward<T>(t)..., std::get<sizeof...(M) - TP>(o)); }
  };

  template <int N>
  struct run_tuple<N, 0>
  {
    template <typename... M, typename... T>
    static decltype(auto) run(std::tuple<M...>&, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)...); }
    template <typename... M, typename... T>
    static decltype(auto) run(const std::tuple<M...>&, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)...); }
  };
  
  template <int N>
  struct run_apply
  {

    // SIOs
    template <typename... M, typename... T>
    static decltype(auto) run(sio<M...>& o, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)..., static_cast<M*>(&o)->value()...); }
    template <typename... M, typename... T>
    static decltype(auto) run(const sio<M...>& o, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)..., static_cast<const M*>(&o)->value()...); }

    // tuples.
    template <typename... M, typename... T>
    static decltype(auto) run(std::tuple<M...>& o, T&&... t)
    { return run_tuple<N, sizeof...(M)>::run(o, std::forward<T>(t)...); }
    template <typename... M, typename... T>
    static decltype(auto) run(const std::tuple<M...>& o, T&&... t)
    { return run_tuple<N, sizeof...(M)>::run(o, std::forward<T>(t)...); }
  
    // Other types
    template <typename T1, typename... T>
    static decltype(auto) run(T1& t1, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)..., t1); }
    template <typename T1, typename... T>
    static decltype(auto) run(const T1& t1, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)..., t1); }

    // Forward
    template <typename T1, typename... T>
    static decltype(auto) run(forward_<T1>&& t1, T&&... t)
    { return run_apply<N - 1>::run(std::forward<T>(t)..., t1.t); }
    
  };

  template <>
  struct run_apply<0>
  {
    // Finalize the call.
    template <typename F, typename... T>
    static decltype(auto) run(F& f, T&&... t)
    {
      return f(std::forward<T>(t)...);
    }

    template <typename F, typename... T>
    static decltype(auto) run(const F& f, T&&... t)
    {
      return f(std::forward<T>(t)...);
    }

    template <typename F, typename... T>
    static decltype(auto) run(F&& f, T&&... t)
    {
      return f(std::forward<T>(t)...);
    }

  };
  
  template <typename... T>
  decltype(auto) apply(T&&... t)
  {
    return run_apply<sizeof...(T) - 1>::run(std::forward<T>(t)...);
  }
  
  

  template <typename F, typename... T>
  decltype(auto) apply_members(const sio<T...>& o, F f)
  {
    return f(*static_cast<const T*>(&o)...);
  }

  template <typename F, typename... T>
  decltype(auto) apply_members(sio<T...>& o, F f)
  {
    return f(*static_cast<T*>(&o)...);
  }


  // Proxy apply.
 
  namespace internal
  {

    template<unsigned N, unsigned SIZE, typename T, typename F, typename G,  typename... U>
    inline
    decltype(auto)
    tuple_proxy_apply(std::enable_if_t<(N == SIZE), int>*, T&, G g, F f, U&&... u)
    {
      return f(g(std::forward<U>(u))...);
    }

    template<unsigned N, unsigned SIZE, typename T, typename F, typename G,  typename... U>
    inline
    decltype(auto)
    tuple_proxy_apply(std::enable_if_t<(N < SIZE), int>*, T& t, G g, F f, U&&... u)
    {
      return tuple_proxy_apply<N + 1, SIZE, T, F>(0, t, g, f,
                                                  std::forward<U>(u)...,
                                                  std::get<N>(t));
                                                  // std::forward<U>(u)...,
                                                  // std::forward<decltype(std::get<N>(t))>(std::get<N>(t)));
      
    }

  }


  template<typename F, typename G,  typename... T>
  inline decltype(auto) proxy_apply(std::tuple<T...>& t, G g, F f)
  {
    return internal::tuple_proxy_apply<0, sizeof...(T), std::tuple<T...>>(0, t, g, f);
  }

  template<typename F, typename G,  typename... T>
  inline decltype(auto) proxy_apply(const std::tuple<T...>& t, G g, F f)
  {
    return internal::tuple_proxy_apply<0, sizeof...(T), const std::tuple<T...>>(0, t, g, f);
  }



  template <typename F, typename G,  typename... T>
  decltype(auto) proxy_apply(const sio<T...>& o, G g, F f)
  {
    return f(g(static_cast<const T*>(&o)->value())...);
  }

  template <typename F, typename G,  typename... T>
  decltype(auto) proxy_apply(sio<T...>& o, G g, F f)
  {
    return f(g(static_cast<T*>(&o)->value())...);
  }



  template <typename F, typename G,  typename... T>
  decltype(auto) proxy_apply_members(const sio<T...>& o, G g, F f)
  {
    return f(g(*static_cast<const T*>(&o))...);
  }

  template <typename F, typename G,  typename... T>
  decltype(auto) proxy_apply_members(sio<T...>& o, G g, F f)
  {
    return f(g(*static_cast<T*>(&o))...);
  }

}
