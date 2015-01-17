#pragma once

namespace iod
{

  template <typename O, typename R, typename... ARGS>
  auto bind_method(O& o, R (O::*m)(ARGS...))
  {
    return [&] (ARGS&&... a) { return (o.*m)(std::forward<ARGS>(a)...); };
  }
  
}
