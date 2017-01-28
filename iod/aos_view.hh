#pragma once

#include <cassert>
#include <iterator>
#include "sio_utils.hh"
#include "sio.hh"

namespace iod
{

  // Iterator, mainly for range based loops
  template <typename V>
  struct aos_view_iterator
    : public std::iterator<std::random_access_iterator_tag,
                           std::remove_reference_t<decltype(std::declval<V>()[0])>>
  
  {
  public:

    typedef aos_view_iterator<V> self_t;
    typedef std::remove_reference_t<decltype(std::declval<V>()[0])> value_type;
    aos_view_iterator(const aos_view_iterator& it) : idx(it.idx), v(it.v) {}
    aos_view_iterator(V& _v, int _idx)
      : idx(_idx),
        v(&_v)
    {}

    aos_view_iterator& operator=(const aos_view_iterator& it)
    {
      idx = it.idx;
      v = it.v;
      return *this;
    }
    
    decltype(auto) operator*() { return (*v)[idx]; }
    self_t&  operator++() { ++idx; return *this; }

    int operator-(const self_t& b) { return idx - b.idx; }
    self_t& operator--() { idx--; return *this; }
    bool operator>(const self_t& b) { return idx > b.idx; }
    bool operator>=(const self_t& b) { return idx >= b.idx; }
    bool operator<(const self_t& b) { return idx < b.idx; }
    bool operator<=(const self_t& b) { return idx <= b.idx; }
    bool operator==(const self_t& b) { return idx == b.idx; }
    bool operator!=(const self_t& b) { return idx != b.idx; }
    
    self_t operator+(int b)
    {
      self_t tmp(*this);
      tmp.idx += b;
      return tmp;
    }
    self_t operator-(int b)
    {
      self_t tmp(*this);
      tmp.idx -= b;
      return tmp;
    }
    
    int idx;
    V* v;
  };
 
  
  template <typename T>
  struct has_size_method
  {
    template<typename C> 
    static char test(decltype(&C::size));
    template<typename C>
    static int test(...);
    static const bool value = sizeof(test<T>(0)) == 1;
  };


  template<typename F, typename... T>
  auto bind_first_arg(F f, int a, std::tuple<int, T...>*)
  {
    return [=] (T... args) -> decltype(auto)
    {
      return f(a, std::forward<T>(args)...);
    };
  }
  
  // Array of structures view.
  template <typename... A>
  struct aos_view_
  {
    typedef aos_view_<A...> self_t;
    typedef aos_view_iterator<self_t> iterator;

    aos_view_(int size, A&&... a)
      : hand_set_size(size),
        arrays((
                 typename A::left_t::template variable_type<typename A::right_t>(a.right)
                 )...)
    {
      //void* x = arrays;
      assert(check_sizes() && "All arrays must have the same size.");
    }
    
    aos_view_(A&&... a)
      : hand_set_size(-1),
        arrays((
                 typename A::left_t::template variable_type<typename A::right_t>(a.right)
                 )...)
    {
      assert(check_sizes() && "All arrays must have the same size.");
    }

    bool check_sizes()
    {
      // Check if array have the same sizes.
      int s = size();
      bool res = true;
      foreach(arrays) | [&res, s] (auto m)
      {
        static_if<has_size_method<std::remove_reference_t<decltype(m.value())>>::value>(
          [&res, s] (auto m) {
            res &= (int(m.value().size()) == s);
          },
          [] (auto) {},
          m);
      };
      return res;
    }

    // Get the size of the AOS.
    size_t size() const {
      if (hand_set_size > -1)
        return hand_set_size;

      int res = -1;
      foreach(arrays) | [&res] (auto m)
      {
        static_if<has_size_method<std::remove_reference_t<decltype(m.value())>>::value>(
          [&res] (auto m) { res = m.value().size(); },
          [] (auto) { },
          m);
      };

      assert(res >= 0 && "At least one array with a size method is needed");
      return res;
    }

    // Access to the ith element.
    decltype(auto) operator[](int i) const
    {
      return D_as_reference((const_cast<self_t*>(this)->template access_member<typename A::left_t>(i))...);
    }
    decltype(auto) operator[](int i)
    {
      return D_as_reference((this->template access_member<typename A::left_t>(i))...);
    }

    // iterators for range based for loops.
    auto begin() { return iterator(*this, 0); }
    auto end() { return iterator(*this, size()); }

  private:
    // Helper to access the ith element, whether the set
    // is computed or in memory.
    template <typename S>
    decltype(auto) access_member(int i)
    {
      typedef decltype(S().member_access(arrays)) array_type;

      //return S() = ref(S().member_access(arrays)[i]);

      return S() =
        (static_if<is_callable<array_type>::value>(
              // If m is a function
              [i] (auto&& m) -> decltype(auto) {
                typedef std::remove_reference_t<decltype(m)> F;
                return static_if<(callable_traits<F>::arity == 1)>(
                  // If only one int arg:
                  [i] (auto m) { return m(i); },
                  // If more than one arg:
                  [i] (auto m) { return bind_first_arg(m, i, (callable_arguments_tuple_t<F>*)0); },
                  m);
              },
              [i] (auto&& m) -> decltype(auto) { return m[i];},
              S().member_access(arrays)));
    }

    int hand_set_size;
    sio<typename A::left_t::template variable_type<typename A::right_t>...> arrays;
  };

  // Builder
  template <typename... A>
  auto aos_view(A&&... a)
  {
    return aos_view_<A...>(std::forward<A>(a)...);
  }

  template <typename... A>
  auto aos_view(int size, A&&... a)
  {
    return aos_view_<A...>(size, std::forward<A>(a)...);
  }

  // Sort elements of a view.
  // Only impact in memory arrays.
  template <class C, typename... A>
  void sort(aos_view_<A...>& v, const C& comp)
  {
    //return 1;
  }
}

// sort(view)
// view.push_back(_a = 12, _b = 23);

