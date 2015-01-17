#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <functional>

#include <iod/tags.hh>
#include <iod/grammar.hh>

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
      f(args.template get_nth_attribute<N>()...);
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

  struct attribute_not_found
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
    const attribute_not_found& get_nth_attribute() const { return *(attribute_not_found*)(0); }
    template <unsigned N>
    attribute_not_found& get_nth_attribute() { return *(attribute_not_found*)(0); }
    template <typename E, typename D>
    D get(const E&, const D default_) const { return default_; }

    auto symbols_as_tuple() const { return std::make_tuple(); }    
  };

  template <typename T, typename ...Tail>
  struct sio<T, Tail...> : public T, public sio<Tail...>
  {
    typedef sio<T, Tail...> self;
    typedef sio<Tail...> super;
    typedef std::tuple<T, Tail...> tuple_type;

    enum { _size = 1 + sizeof...(Tail), _empty = 0 };

    template <unsigned N, typename U, typename P = void>
    struct _nth_attribute_ {};

    template <unsigned N, typename U>
    struct _nth_attribute_<N, U, std::enable_if_t<(N < _size)>>
    { typedef typename std::tuple_element<N, U>::type type; };

    template <unsigned N, typename U>
    struct _nth_attribute_<N, U, std::enable_if_t<(N >= _size)>>
    { typedef attribute_not_found type; };

    template <unsigned N>
    using _nth_attribute = typename _nth_attribute_<N, std::tuple<T, Tail...>>::type;

    // -----------------------------------------
    // Retrive type of attribute with symbol S.
    // -----------------------------------------
    template <unsigned N, typename C, typename S>
    struct _attribute_;

    template <typename C, typename S>
    struct _attribute_<_size, C, S>
    {
      typedef attribute_not_found type;
    };

    template <unsigned N, typename S>
    struct _attribute_<N, S, S>
    {
      typedef _nth_attribute<N> type;
    };

    template <unsigned N, typename C, typename S>
    struct _attribute_
    {
      typedef typename _attribute_<N+1, typename _nth_attribute<N+1>::symbol_type, S>::type type;
    };

    template <typename S>
    using _attribute = typename _attribute_<0, typename _nth_attribute<0>::symbol_type, S>::type;

    template <typename S>
    using attribute_value_type = typename _attribute_<0, typename _nth_attribute<0>::symbol_type, S>::type::value_type;

    template <int V>
    struct simple_enum { enum { value = V}; };
    
    template <typename E>
    using _has = simple_enum<not std::is_same<_attribute<E>, attribute_not_found>::value>;

    // Constructor.
    inline sio() = default;

    inline sio(const T&& attr, const Tail&&... tail)
      : T(attr),
        sio<Tail...>(std::forward<const Tail>(tail)...)
      {
      }

    inline sio(const T& attr, const Tail&... tail)
      : T(attr),
        sio<Tail...>(std::forward<const Tail>(tail)...)
      {
      }

    // Get the attribute associated with the symbol S.
    template <typename S>
    auto& symbol_to_attribute(S = S())
    {
      return *static_cast<_attribute<S>*>(this);
    }

    template <typename S>
    const auto& symbol_to_attribute(S = S()) const
    {
      return *static_cast<const _attribute<S>*>(this);
    }
    
    // Access to a member from a symbol.
    template <typename E>
    auto& operator[](const E&)
    {
      return symbol_to_attribute<typename E::symbol_type>().value();
    }

    template <typename E>
    const auto& operator[](const E&) const
    {
      return symbol_to_attribute<E>().value();
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

    // Access to the super attribute.
    super& get_super() { return *this; }
    const super& get_super() const { return *this; }

    // Get the nth attribute.
    template <unsigned N>
    typename std::enable_if<N!=0, _nth_attribute<N>&>::type
    get_nth_attribute() { return get_super().template get_nth_attribute<N-1>(); }

    template <unsigned N>
    typename std::enable_if<N==0, _nth_attribute<N>&>::type
    get_nth_attribute() { return *static_cast<T*> (this); }

    template <unsigned N>
    typename std::enable_if<N!=0, const _nth_attribute<N>&>::type
    get_nth_attribute() const { return get_super().template get_nth_attribute<N-1>(); }

    template <unsigned N>
    typename std::enable_if<N==0, const _nth_attribute<N>&>::type
    get_nth_attribute() const { return *static_cast<const T*> (this); }

    template <unsigned N>
    const typename _nth_attribute<N>::value_type&
    get_nth() const { return get_nth_attribute<N>().value(); }

    template <unsigned N>
    typename _nth_attribute<N>::value_type&
    get_nth() { return get_nth_attribute<N>().value(); }

    auto&& values_as_tuple() { return std::forward_as_tuple(static_cast<T*>(this)->value(),
                                                            static_cast<Tail*>(this)->value()...); }
    auto&& values_as_tuple() const { return std::forward_as_tuple(static_cast<const T*>(this)->value(),
                                                                  static_cast<const Tail*>(this)->value()...); }

    auto symbols_as_tuple() const { return std::make_tuple(static_cast<const T*>(this)->symbol(),
                                                           static_cast<const Tail*>(this)->symbol()...); }
    
    // Assignment.
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
