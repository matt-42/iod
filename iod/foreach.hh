#pragma once

#include <tuple>
#include <iod/sio.hh>
#include <iod/callable_traits.hh>
#include <iod/utils.hh>
#include <iod/apply.hh>

namespace iod
{

  namespace internal
  {

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach_loop_tuple(std::enable_if_t<N == SIZE>*, F f, A&& args_tuple, R&&... results)
    {
      return static_if<sizeof...(R) == 0>(
        [] () {},
        [&] () { return std::make_tuple(results...);}); }

    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach_loop_tuple(std::enable_if_t<N < SIZE>*, F f, A&& args_tuple, R&&... results)
    {
      auto h = [] (auto&& a) -> auto&& // avoid the lambda to convert references to values.
        {
          return std::forward<decltype(std::get<N>(a))>(std::get<N>(a)); 
        };
      typedef decltype(h) H;
      typedef decltype(proxy_apply(args_tuple, std::declval<H>(), f)) return_type;
      
      return static_if<std::is_same<return_type, void>::value>(
        [&] (auto& args_tuple)
             {
               proxy_apply(args_tuple, h, f);
               return foreach_loop_tuple<N + 1, SIZE>(0, f, args_tuple, results...);
             },
        [&] (auto& args_tuple)
             {
               return foreach_loop_tuple<N + 1, SIZE>
                 (0, f, args_tuple, results..., proxy_apply(args_tuple, h, f));
             }, args_tuple);

    }


    template<unsigned N, unsigned SIZE, typename F, typename A, typename... R>
    inline
    auto
    foreach_loop_sio(std::enable_if_t<N == SIZE>*, F f, A&& args_tuple, R&&... results)
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
          return std::forward<decltype(a.template get_nth_attribute<N>())>(a.template get_nth_attribute<N>()); 
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

}
