#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <functional>

#include <iod/tags.hh>
#include <iod/grammar.hh>
#include <iod/type_list_utils.hh>

namespace iod
{
  template <typename ...T>
  struct sio;

  template <typename T, typename ...Tail>
  inline std::vector<T> iod_array(const T& t, Tail... args);

  namespace iod_internals
  {

    // Fill an array from a variable list of arguments.
    template <typename T>
    inline void array_fill(std::vector<T>&)
    {
    }

    template <typename T, typename ...Tail>
    inline void array_fill(const T& a, const Tail&... args, std::vector<T>& array)
    {
      array.push_back(a);
      array_fill(args..., array);
    }

    // Assign the attributes of src to the attributes of dst.
    template <int N, typename D, typename S>
    struct iod_assign
    {
      static inline void run(D& dst, const S& src)
        {
          const auto& src_attr = src.template get_nth<N>();

          typedef typename S::template _nth_attribute<N>::symbol_type symbol_type;
          dst.template get<symbol_type>() = src_attr.value();
          iod_assign<N-1, D, S>::run(dst, src);
        }
    };
    template <typename D, typename S>
    struct iod_assign<-1, D, S>
    {
      static inline void run(D&, const S&)
      {
      }
    };

  }

  template <template <class> class Const,
            int N, typename F, typename... T>
  struct iod_foreach_runner
  {
    static inline void run(F f, Const<T>&... args)
    {
      iod_foreach_runner<Const, N-1, F, T...>::run(f, args...);
      f(args.template get_nth_member<N>()...);
    }
  };

  template <template <class> class Const,
            typename F, typename... T>
  struct iod_foreach_runner<Const, -1, F, T...>
  {
    static inline void run(F, Const<T>&...)
    {
    }
  };

  // Apply a function on all member of the iod objects \args.
  template <typename F, typename T, typename ...U>
  void foreach_attribute(F f, T& arg, U&... tail)
  {
    iod_foreach_runner<std::remove_const_t, T::_size - 1, F, T, U...>::run(f, arg, tail...);
  }

  template <typename F, typename T, typename ...U>
  void foreach_attribute(F f, const T& arg, const U&... tail)
  {
    iod_foreach_runner<std::add_const_t, T::_size - 1, F, T, U...>::run(f, arg, tail...);
  }

  // Apply a function on all member of the iod objects \args.
  template <typename F, typename T, typename ...U>
  void foreach_attribute_value(F f, T& arg, U&... tail)
  {
    auto g = [&f] (auto& m) { return f(m.value()); };
    iod_foreach_runner<std::remove_const_t, T::_size - 1, decltype(g), T, U...>::run(g, arg, tail...);
  }

  template <typename F, typename T, typename ...U>
  void foreach_attribute_value(F f, const T& arg, const U&... tail)
  {
    auto g = [&f] (auto& m) { return f(m.value()); };
    iod_foreach_runner<std::add_const_t, T::_size - 1, decltype(g), T, U...>::run(g, arg, tail...);
  }

  struct member_not_found
  {
    typedef not_found symbol_type;
    typedef int value_type;
    const int& value() const { return x; }
    int& value() { return x; }

    int x;
  };

  template <>
  struct sio<>
  {
    enum { _size = 0, _empty = true };

    template <typename E>
    struct _has { static const bool value = false; };
    
    constexpr sio() { }
    constexpr static int size() { return _size; }
    constexpr static bool empty() { return true; }
    template <typename T>
    constexpr static bool has(T) { return false; }
    template <unsigned N>
    not_found get_nth() const { return not_found(); }
    template <unsigned N>
    member_not_found get_nth_attribute() const { return member_not_found(); }
    template <unsigned N>
    member_not_found get_nth_attribute() { return member_not_found(); }
    template <typename E, typename D>
    D get(const E&, const D default_) const { return default_; }

    auto symbols_as_tuple() const { return std::make_tuple(); }    
  };

  template <typename ...Ms>
  struct sio : public Ms...
  {
    typedef sio<Ms...> self;
    typedef std::tuple<Ms...> tuple_type;

    enum { _size = sizeof...(Ms), _empty = sizeof...(Ms) == 0 };

    // Get the type of the nth member.
    template <std::size_t N>
    using nth_member_type = tl::get_nth_type<N, Ms...>;

    // Get the position of a given symbol    
    template <typename S>
    using symbol_to_member_type = nth_member_type<
      tl::get_type_position<S, typename Ms::symbol_type...>::value
                                  >;
    
    // Get the value type of a diven symbol.
    template <typename S>
    using member_value_type = typename symbol_to_member_type<S>::value_type;

    // template <int V>
    // struct simple_enum { enum { value = V}; };
    
    template <typename S>
    using _has = std::integral_constant<bool, tl::type_list_has<S, typename Ms::symbol_type...>::value>;

    // Constructor.
    inline sio() = default;
    inline sio(self&&) = default;
    inline sio(const self&) = default;
    inline sio(Ms&&... members) : Ms(std::forward<Ms>(members))... {}
    inline sio(const Ms&... members) : Ms(members)... {}

    // Get the member associated with the symbol S.
    // Compile time complexity: O(N)
    template <typename S>
    auto& symbol_to_member(S = S())
    {
      return *static_cast<symbol_to_member_type<S>*>(this);
    }

    template <typename S>
    const auto& symbol_to_member(S = S()) const
    {
      return *static_cast<const symbol_to_member_type<S>*>(this);
    }
    
    // Access to a member from a symbol.
    template <typename E>
    auto& operator[](const E&)
    {
      return symbol_to_member<typename E::symbol_type>().value();
    }

    template <typename E>
    const auto& operator[](const E&) const
    {
      return symbol_to_member<E>().value();
    }

    template <typename E, typename D>
    decltype(auto)
    get(const E& e, const D, std::enable_if_t<_has<E>::value>* = 0) const
    {
      return (*this)[e];
    }

    template <typename E, typename D>
    typename std::enable_if<not _has<E>::value, D>::type
    get(const E&, D default_) const
    {
      return default_;
    }
    
    // Check if the object has this symbol.
    template <typename E>
    constexpr static bool has(const E&)
    {
      return _has<E>::value;
    }

    // Check if the object has this symbol.
    constexpr static int size()
    {
      return _size;
    }

    constexpr static bool empty()
    {
      return _size == 0;
    }

    // Get the nth attribute.
    template <unsigned N>
    decltype(auto) get_nth_member() { return *static_cast<nth_member_type<N>*>(this); }
    template <unsigned N>
    decltype(auto) get_nth_member() const { return *static_cast<const nth_member_type<N>*>(this); }

    template <unsigned N>
    decltype(auto)
    get_nth() const { return get_nth_member<N>().value(); }

    template <unsigned N>
    decltype(auto)
    get_nth() { return get_nth_member<N>().value(); }

    auto&& values_as_tuple() { return std::forward_as_tuple(static_cast<Ms*>(this)->value()...); }
    auto&& values_as_tuple() const { return std::forward_as_tuple(static_cast<const Ms*>(this)->value()...); }

    auto symbols_as_tuple() const { return std::make_tuple(Ms::symbol()...); }
    
    // Assignment.
    self& operator=(self&& o) = default;
    template <typename... Otail>
    self& operator=(const sio<Otail...>& o)
    {
      foreach(o) | [this] (auto& m) { (*this)[m.symbol()] = m.value(); };
      return *this;
    }
    
  };

  
  template <typename R, typename S>
  using has_symbol = typename R::template _has<S>;

} // end of namespace iod.

#include <iod/symbol.hh>
#include <iod/sio_utils.hh>
