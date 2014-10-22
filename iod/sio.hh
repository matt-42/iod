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

  template <typename ...T>
  inline auto D(T&&... args);

  template <typename T, typename ...Tail>
  inline std::vector<T> iod_array(const T& t, Tail... args);

  namespace iod_internals
  {

    // Fill an array from a variable list of arguments.
    template <typename T>
    inline void array_fill(std::vector<T>& array)
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
      static inline void run(D& dst, const S& src)
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
    static inline void run(F f, Const<T>&... args)
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
    D get(const E& e, const D default_) const { return default_; }
    
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
    typename std::enable_if<_has<E>::value, attribute_value_type<E>>::type
    get(const E& e, const D default_) const
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

    // Assignment.
    template <typename O, typename... Otail>
    self& operator=(const sio<O, Otail...>& o)
    {
      foreach_attribute_value([] (auto& a, const auto& b) { a = b; }, *this, o);
      return *this;
    }
  
  };

  template <typename R, typename S>
  using has_symbol = typename R::template _has<S>;


  // template <typename T>
  // auto&& exp_to_variable(T&& t)
  // {
  //   return t;
  // }

  template <typename E>
  struct variable;

  template <typename V>
  const V& exp_to_variable(const variable<V>& v)
  {
    return *static_cast<const V*>(&v);
  }

  template <typename S>
  const S& exp_to_variable(const symbol<S>& s)
  {
    return *static_cast<const S*>(&s);
  }

  template <typename S, typename T>
  auto exp_to_variable(const function_call_exp<S, T>& c)
  {
    return typename S::template variable_type<T>(std::get<0>(c.args));
  }

  template <typename S, typename V>
  auto exp_to_variable(const assign_exp<S, V>& e)
  {
    typedef V vtype;
    return typename S::template variable_type<vtype>(e.right);
  }

  template <typename S, typename V, typename... ARGS>
  auto exp_to_variable(const assign_exp<function_call_exp<S, ARGS...>, V>& e)
  {
    typedef V vtype;
    return typename S::template variable_type<vtype, decltype(D(std::declval<ARGS>()...))>(e.right);
  }

  template <typename ...T>
  inline auto D(T&&... args)
  {
    typedef
      sio<std::remove_const_t<std::remove_reference_t<decltype(exp_to_variable(args))>>...>
      result_type;

    return result_type(exp_to_variable(args)...);
  }

  template <int N>
  struct transform_runner
  {
    template <typename F, typename O>
    static inline auto run(O o, F f)
    {
      return cat(transform_runner<N-1>::run(o, f),
                 f(o.template get_nth_attribute<N>()));
    }
  };

  // template <>
  // struct transform_runner<0>
  // {
  //   template <typename F, typename O>
  //   static inline auto run(const O& o, F f)
  //   {
  //     return D(f(o.template get_nth_attribute<0>()));
  //   }
  // };

  // template <>
  // struct transform_runner<-1>
  // {
  //   template <typename F, typename O>
  //   static inline auto run(const O& o, F f)
  //   {
  //     return D();
  //   }
  // };

  // template <typename F, typename O>
  // auto transform(const O& o, F f)
  // {
  //   return transform_runner<O::_size - 1>::run(o, f);
  // }

  template <typename F, typename... T>
  void apply_variables(F f, const sio<T...>& o)
  {
    return f(*static_cast<const T*>(&o)...);
  }

  template <typename F, typename... T>
  void apply_values(F f, const sio<T...>& o)
  {
    return f(static_cast<const T*>(&o)->value()...);
  }

  template <unsigned I, unsigned S, unsigned O>
  struct paste
  {
    template <typename ...T, typename ...U>
    static void run(sio<T...>& a,
                    const sio<U...>& b)
    {
      a.template get_nth<I + O>() = b.template get_nth<I>();
      paste<I+1, S, O>::run(a, b);
    }
  };

  template <unsigned S, unsigned O>
  struct paste<S, S, O>
  {
    template <typename ...T, typename ...U>
    static void run(sio<T...>& a,
                    const sio<U...>& b)
    {
    }
  };

  template <typename ...T, typename ...U>
  inline auto cat(const sio<T...>& a,
                  const sio<U...>& b)
  {
    return sio<T..., U...>(*static_cast<const T*>(&a)...,
                                  *static_cast<const U*>(&b)...);
  }

  template <typename ...T, typename V>
  inline auto cat(const sio<T...>& a,
                  const V& variable)
  {
    return sio<T..., decltype(exp_to_variable(variable))>
      (*static_cast<const T*>(&a)..., exp_to_variable(variable));
  }

  template <typename T, typename ...Tail>
  inline std::vector<T> iod_array(const T& t, Tail... args)
  {
    std::vector<T> res;
    res.reserve(1 + sizeof...(args));
    iod_internals::array_fill<T, Tail...>(t, args..., res);
    return res;
  }

} // end of namespace iod.

#include <iod/symbol.hh>
