#pragma once

#include <utility>

#include "symbol.hh"
#include "grammar.hh"
#include "sio_utils.hh"

namespace iod
{

  template <typename ...T>
  inline auto options(T&&... args);
  
  struct
  {
    template <typename... X>
    decltype(auto) operator() (X&&... t) const { return options(std::forward<X>(t)...); }
  } options_caller;
  
  template <typename S>
  const S& parse_option(const symbol<S>& s)
  {
    return *static_cast<const S*>(&s);
  }

  template <typename... ARGS>
  auto parse_option(const std::tuple<ARGS...>& e)
  {
    return apply(e, options_caller);
  }
  
  template <typename S, typename... ARGS>
  auto parse_option(const function_call_exp<S, ARGS...>& e)
  {
    return make_variable<S>(parse_option(e.args));
  }
  
  template <typename S, typename V>
  auto parse_option(const assign_exp<S, V>& e)
  {
    return make_variable<S>(e.right);
  }
   
  template <typename ...T>
  inline auto options(T&&... args)
  {
    typedef
      sio<std::decay_t<decltype(parse_option(args))>...>
      result_type;

    return result_type(parse_option(args)...);
  }

}
