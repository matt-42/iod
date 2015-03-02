#pragma once

namespace iod
{

  template <typename O, typename R, typename... ARGS>
  auto bind_method(O& o, R (O::*m)(ARGS...))
  {
    return [&o, m] (ARGS... a) { return (o.*m)(std::forward<ARGS>(a)...); };
  }

  template <typename O, typename R, typename... ARGS>
  auto bind_method(O& o, R (O::*m)(ARGS...) const)
  {
    return [&o, m] (ARGS... a) { return (o.*m)(std::forward<ARGS>(a)...); };
  }
  
}
