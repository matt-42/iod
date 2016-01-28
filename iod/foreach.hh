#pragma once

#include <tuple>
#include <iod/callable_traits.hh>
#include <iod/utils.hh>
#include <iod/apply.hh>

namespace iod
{
  template <typename ...T>
  struct sio;

  template <typename ...T>
  inline auto D(T&&... args);
  
  namespace internal
  {

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    decltype(auto)
    foreach_loop_tuple(std::enable_if_t<N == SIZE>*, F, A&&, R&&... results)
    {
      return std::make_tuple(results...);
    }

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    decltype(auto)
    foreach_loop_tuple(std::enable_if_t<N < SIZE>*, F f, A&& args_tuple, R&&... results)
    {
      auto h = [] (auto&& a) -> decltype(auto)
        {
          return std::forward<decltype(std::get<N>(a))>(std::get<N>(a)); 
        };
      typedef decltype(h) H;
      typedef decltype(proxy_apply(args_tuple, std::declval<H>(), f)) return_type;
      
      return static_if<std::is_same<return_type, void>::value>(
        [&] (auto& args_tuple, auto& h, auto& f)
             {
               proxy_apply(args_tuple, h, f);
               return foreach_loop_tuple<N + 1, SIZE>(0, f, args_tuple, std::forward<R>(results)...);
             },
        [&] (auto& args_tuple, auto& h, auto& f) -> decltype(auto)
             {
               return foreach_loop_tuple<N + 1, SIZE>
                 (0, f, args_tuple, std::forward<R>(results)..., proxy_apply(args_tuple, h, f));
             }, args_tuple, h, f);

    }


    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach_loop_sio(std::enable_if_t<N == SIZE>*, F, A&&, R&&... results)
    {
      return static_if<sizeof...(R) == 0>(
        [] () {},
        [&] () { return D(results...);});
    }

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach_loop_sio(std::enable_if_t<N < SIZE>*, F f, A&& args_tuple, R&&... results)
    {
      auto h = [] (auto&& a) -> auto&& // avoid the lambda to convert references to values.
        {
          return std::forward<decltype(a.template get_nth_member<N>())>(a.template get_nth_member<N>()); 
        };
      typedef decltype(h) H;
      typedef decltype(proxy_apply(args_tuple, std::declval<H>(), f)) return_type;
      
      return static_if<std::is_same<return_type, void>::value>(
        [&] (auto& args_tuple)
             {
               proxy_apply(args_tuple, h, f);
               return foreach_loop_sio<N + 1, SIZE>(0, f, args_tuple, results...);
             },
        [&] (auto& args_tuple)
             {
               return foreach_loop_sio<N + 1, SIZE>
                 (0, f, args_tuple, results..., proxy_apply(args_tuple, h, f));
             }, args_tuple);

    }

    
    
    template <typename T>
    struct foreach_tuple_caller
    {
      foreach_tuple_caller(T&& t) : t_(t) {}

      template <typename F>
      auto operator|(F f)
      {
        const int size = std::tuple_size<std::remove_reference_t<decltype(std::get<0>(t_))>>::value;
        return internal::foreach_loop_tuple<0, size>(0, f, t_);
      }

      const T t_;
    };
    
    template <typename T>
    struct foreach_sio_caller
    {
      foreach_sio_caller(T&& t) : t_(t) {}

      template <typename F>
      auto operator|(F f)
      {
        const int size = std::remove_reference_t<decltype(std::get<0>(t_))>::_size;
        return internal::foreach_loop_sio<0, size>(0, f, t_);
      }
      
      const T t_;
    };

  }

  template <typename... S, typename... T>
  auto foreach(std::tuple<S...>& a1, T&&... args)
  {
    return internal::foreach_tuple_caller<decltype(std::forward_as_tuple(a1, args...))>
      (std::forward_as_tuple(a1, args...));
  }

  template <typename... S, typename... T>
  auto foreach(const std::tuple<S...>& a1, T&&... args)
  {
    return internal::foreach_tuple_caller<decltype(std::forward_as_tuple(a1, args...))>
      (std::forward_as_tuple(a1, args...));
  }

  template <typename... S, typename... T>
  auto foreach(sio<S...>& a1, T&&... args)
  {
    return internal::foreach_sio_caller<decltype(std::forward_as_tuple(a1, args...))>
      (std::forward_as_tuple(a1, args...));
  }

  template <typename... S, typename... T>
  auto foreach(const sio<S...>& a1, T&&... args)
  {
    return internal::foreach_sio_caller<decltype(std::forward_as_tuple(a1, args...))>
      (std::forward_as_tuple(a1, args...));
  }


  namespace internal
  {
    
    template<unsigned N, unsigned SIZE, typename F, typename A, typename P, typename... R>
    inline
    auto
    foreach_loop_tuple_prev(std::enable_if_t<N == SIZE>*, F, A&&,
                            P, R&&... results)
    {
      return static_if<sizeof...(R) == 0>(
        [] () {},
        [&] () { return std::forward_as_tuple(results...);}); }

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach_loop_tuple_prev(std::enable_if_t<N < SIZE>*, F f, A&& args_tuple, R&&... results)
    {
      auto h = [] (auto&& a) -> auto&& // avoid the lambda to convert references to values.
        {
          return std::forward<decltype(std::get<N>(a))>(std::get<N>(a)); 
        };

      auto results_tuple = std::forward_as_tuple(results...);
      auto prev = std::get<std::tuple_size<decltype(results_tuple)>::value - 1>(results_tuple);

      // typedef decltype(h) H;
      typedef decltype(apply(std::tuple_cat(foreach(args_tuple) | h, std::forward_as_tuple(prev)), f)) return_type;
      //typedef decltype(proxy_apply(args_tuple, std::declval<H>(), f)) return_type;


      return static_if<std::is_same<return_type, void>::value>(
        [&] (auto& args_tuple, auto& h, auto& f)
             {
               apply(std::tuple_cat(foreach(args_tuple) | h, std::forward_as_tuple(prev)), f);
               return foreach_loop_tuple_prev<N + 1, SIZE>(0, f, args_tuple, results...);
             },
        [&] (auto& args_tuple, auto& h, auto& f)
             {
               return foreach_loop_tuple_prev<N + 1, SIZE>
                 (0, f, args_tuple, results...,
                  apply(std::tuple_cat(foreach(args_tuple) | h, std::forward_as_tuple(prev)), f));
             }, args_tuple, h, f);
    }

    
    template <typename T, typename P>
    struct foreach_tuple_caller_prev
    {
      foreach_tuple_caller_prev(T&& t, P prev_init) : t_(t), prev_init_(prev_init) {}

      template <typename F>
      auto operator|(F f)
      {
        const int size = std::tuple_size<std::remove_reference_t<decltype(std::get<0>(t_))>>::value;
        return internal::foreach_loop_tuple_prev<0, size>(0, f, t_, prev_init_);
      }

      const T t_;
      P prev_init_;
    };

  }

  

  template <typename P, typename... S, typename... T>
  auto foreach_prev(std::tuple<S...>& a1, P prev_init, T&&... args)
  {
    return internal::foreach_tuple_caller_prev<decltype(std::forward_as_tuple(a1, args...)), P>
      (std::forward_as_tuple(a1, args...), prev_init);
  }

  template <typename P, typename... S, typename... T>
  auto foreach_prev(const std::tuple<S...>& a1, P prev_init, T&&... args)
  {
    return internal::foreach_tuple_caller_prev<decltype(std::forward_as_tuple(a1, args...)), P>
      (std::forward_as_tuple(a1, args...), prev_init);
  }

  // template <typename... S, typename... T, typename F>
  // auto foreach(F f, sio<S...>& a1, T&&... args)
  // {
  //   return internal::foreach_loop_sio<0, sizeof...(S)>(0, f, std::forward_as_tuple(a1, args...));
  // }

  // template <typename... S, typename... T, typename F>
  // auto foreach(F f, const sio<S...>& a1, T&&... args)
  // {
  //   return internal::foreach_loop_sio<0, sizeof...(S)>(0, f, std::forward_as_tuple(a1, args...));
  // }

  template<unsigned N, unsigned SIZE, typename F, typename O, typename P>
  inline
  auto
  sio_iterate_loop(std::enable_if_t<N < SIZE>*, F f, const O& o, const P& prev)
  {
    auto new_prev = f(o.template get_nth_member<N>(), prev);
    return sio_iterate_loop<N+1, SIZE>(0, f, o, new_prev);
  }

  template<unsigned N, unsigned SIZE, typename F, typename O, typename P>
  inline
  auto
  sio_iterate_loop(std::enable_if_t<N == SIZE>*, F f, const O& o, const P& prev)
  {
    return prev;
  }

  template <typename O, typename P>
  struct sio_iterate_caller
  {
    sio_iterate_caller(O o, P init) : o_(o), init_(init) {}

    template <typename F>
    auto operator|(F f)
      {
        const int size = O::size();
        return sio_iterate_loop<0, size>(0, f, o_, init_);
      }

    const O o_;
    P init_;
  };
  
  template <typename O, typename I>
  auto sio_iterate(O o, I init)
  {
    return sio_iterate_caller<O, I>(o, init);
  }

}
