#pragma once

namespace iod
{
  template <bool C>
  struct static_if_;

  template <>
  struct static_if_<true>
  {
    template <typename F, typename G, typename... A>
    decltype(auto) operator()(F then, G, A&&... args)
    {
      return then(std::forward<A>(args)...);
    }
  };

  template <>
  struct static_if_<false>
  {
    template <typename F, typename G, typename... A>
    decltype(auto) operator()(F, G _else, A&&... args)
    {
      return _else(std::forward<A>(args)...);
    }
  };

  template <bool C, typename F, typename G, typename... A>
  decltype(auto) static_if(F _if, G _else, A&&... args)
  {
    return static_if_<C>()(_if, _else, std::forward<A>(args)...);
  }

}
