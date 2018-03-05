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

    template <size_t N, typename F, typename... T>
    decltype(auto) tuple_foreach_i(F f, T&&... ts)
    {
      return f(std::get<N>(ts)...);
    }

    template<typename Ret, typename F, size_t... TSize, size_t... Nargs, typename... T>
    inline void
    tuple_foreach_impl(std::enable_if_t<std::is_same<void, Ret>::value>*,
                       std::index_sequence<TSize...>,
                       F f, T&&... ts)
    {
      return (void)std::initializer_list<int>{
        ((void)tuple_foreach_i<TSize>(f, std::forward<T>(ts)...), 0)...};
    }

    template<typename Ret, typename F, size_t... TSize, size_t... Nargs, typename... T>
    inline decltype(auto)
      tuple_foreach_impl(std::enable_if_t<!std::is_same<void, Ret>::value>*,
                         std::index_sequence<TSize...>,
                         F f, T&&... ts)
    {
      return std::make_tuple(iod::internal::tuple_foreach_i<TSize>(f, std::forward<T>(ts)...)...);
    }


    template <size_t N, typename F, typename... T>
    decltype(auto) sio_foreach_i(F f, T&&... ts)
    {
      return f(ts.template get_nth_member<N>()...);
    }

    template<typename Ret, typename F, size_t... TSize, size_t... Nargs, typename... T>
    inline void
    sio_foreach_impl(std::enable_if_t<std::is_same<void, Ret>::value>*,
                     std::index_sequence<TSize...>,
                     F f, T&&... ts)
    {
      return (void)std::initializer_list<int>{
        ((void)sio_foreach_i<TSize>(f, std::forward<T>(ts)...), 0)...};
    }

    template<typename Ret, typename F, size_t... TSize, size_t... Nargs, typename... T>
    inline decltype(auto)
    sio_foreach_impl(std::enable_if_t<!std::is_same<void, Ret>::value>*,
                     std::index_sequence<TSize...> /*si*/,
                     F f, T&&... ts)
    {
      return D(iod::internal::sio_foreach_i<TSize>(f, std::forward<T>(ts)...)...);
    }
    
    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach2_loop_sio(std::enable_if_t<N == SIZE>*, F, A&&, R&&... results)
    {
      return static_if<sizeof...(R) == 0>(
        [] () {},
        [&] () { return D(results...);});
    }

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach2_loop_sio(std::enable_if_t<N < SIZE>*, F f, A&& args_tuple, R&&... results)
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
               return foreach2_loop_sio<N + 1, SIZE>(0, f, args_tuple, results...);
             },
        [&] (auto& args_tuple)
             {
               return foreach2_loop_sio<N + 1, SIZE>
                 (0, f, args_tuple, results..., proxy_apply(args_tuple, h, f));
             }, args_tuple);

    }


    template<typename F, typename... T>
    inline void tuple_foreach(F, std::tuple<>, T&&...)
    {
    }
    
    template<typename F, typename T1, typename... T>
    inline decltype(auto) tuple_foreach(F f, T1&& t1 , T&&... ts)
    {
      using seq = std::make_index_sequence<std::tuple_size<std::decay_t<T1>>::value>;
      using Ret = decltype(f(std::get<0>(t1), std::get<0>(ts)...));
      return iod::internal::tuple_foreach_impl<Ret>(0, seq{},
                                                    f, t1, std::forward<T>(ts)...);
    }

    template<typename F, typename T1, typename... T>
    inline void sio_foreach(std::enable_if_t<std::remove_reference_t<T1>::_size == 0>*,
                            F /*f*/, T1&& /*t1*/, T&&... /*ts*/)
    {
    }
    
    template<typename F, typename T1, typename... T>
    inline decltype(auto) sio_foreach(std::enable_if_t<std::remove_reference_t<T1>::_size != 0>*,
                                      F f, T1&& t1 , T&&... ts)
    {
      const size_t size = std::remove_reference_t<T1>::_size;
      using seq = std::make_index_sequence<size>;
      using Ret = decltype(f(t1.template get_nth_member<0>(), ts.template get_nth_member<0>()...));
      return iod::internal::sio_foreach_impl<Ret>(0, seq{},
                                                  f, t1, std::forward<T>(ts)...);
    }
    
    template <typename T>
    struct foreach_tuple_caller
    {
      foreach_tuple_caller(T&& t) : t_(t) {}

      template <typename F, size_t... I>
      decltype(auto) run(F f, std::index_sequence<I...>)
      {
        return iod::internal::tuple_foreach(f, std::get<I>(t_)...);        
      }

      template <typename F>
      decltype(auto) operator|(F f)
      {
        return run(f, std::make_index_sequence<std::tuple_size<T>::value>{});
      }

      const T t_;
    };
    

    template <typename T>
    struct foreach_sio_caller
    {
      foreach_sio_caller(T&& t) : t_(t) {}


      template <typename F, size_t... I>
      decltype(auto) run(F f, std::index_sequence<I...>)
      {
        return iod::internal::sio_foreach(0, f, std::get<I>(t_)...);        
      }

      template <typename F>
      decltype(auto) operator|(F f)
      {
        return run(f, std::make_index_sequence<std::tuple_size<T>::value>{});
      }
            
      const T t_;
    };

    template <typename T>
    struct foreach2_sio_caller
    {
      foreach2_sio_caller(T&& t) : t_(t) {}

      template <typename F>
      auto operator|(F f)
      {
        const int size = std::remove_reference_t<decltype(std::get<0>(t_))>::_size;
        return internal::foreach2_loop_sio<0, size>(0, f, t_);
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


  template <typename... S, typename... T>
  auto foreach2(sio<S...>& a1, T&&... args)
  {
    return internal::foreach2_sio_caller<decltype(std::forward_as_tuple(a1, args...))>
      (std::forward_as_tuple(a1, args...));
  }

  template <typename... S, typename... T>
  auto foreach2(const sio<S...>& a1, T&&... args)
  {
    return internal::foreach2_sio_caller<decltype(std::forward_as_tuple(a1, args...))>
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
  sio_iterate_loop(std::enable_if_t<N == SIZE>*, F /*f*/, const O& /*o*/, const P& prev)
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
