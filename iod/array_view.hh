#pragma once

#include "sio_utils.hh"
#include "sio.hh"
#include "symbols.hh"
#include "aos_view.hh"

namespace iod
{

  template <typename V>
  struct array_view_
  {
    typedef array_view_<V> self_t;

    array_view_(V _view) : view(_view) {}

    decltype(auto) operator[](int i) const
    {
      return view[i].elt;
    }

    decltype(auto) operator[](int i)
    {
      return view[i].elt;
    }

    auto size() const { return view.size(); }
    auto begin() { return aos_view_iterator<self_t>(*this, 0); }
    auto end() { return aos_view_iterator<self_t>(*this, view.size()); }

    V view;
  };

  // Builder
  template <typename A>
  auto array_view(int size, A&& a)
  {
    typedef std::remove_reference_t<decltype(s::_elt = std::forward<A>(a))> var_type;
    typedef aos_view_<var_type> aos_type;

    return array_view_<aos_type>(aos_type(size, s::_elt = std::forward<A>(a)));
  }

}
