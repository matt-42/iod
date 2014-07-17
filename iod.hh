#ifndef IOD_API_HH_
# define IOD_API_HH_

#include <string>
#include <vector>
#include <tuple>
#include <functional>

template <typename ...T>
struct iod_object;

template <typename ...T>
inline iod_object<T...> iod(T&&... args);

template <typename T, typename ...Tail>
inline std::vector<T> iod_array(const T& t, Tail... args);

template <typename T>
struct Symbol
{
  Symbol() {}
  Symbol(T) {}
};

template <typename A>
using is_symbol = std::is_base_of<A,Symbol<A>>;

namespace iod_internals
{

  template <typename T>
  struct transform_type
  {
    typedef T type;
  };

  template <>
  struct transform_type<const char*>
  {
    typedef std::string type;
  };


  template <int N>
  struct transform_type<const char[N]>
  {
    typedef std::string type;
  };


  template <typename T>
  struct transform_type<std::initializer_list<T> >
  {
    typedef std::vector<T> type;
  };

  template <typename T>
  using transform_type_t = typename transform_type<T>::type;

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

  template <template <int> class N, template <class, class, int> class A, int serializable>
  struct attribute_name : Symbol<attribute_name<N, A, serializable>>
  {
    typedef N<0> name;

    attribute_name() {}

    template <typename T>
    using tt = transform_type_t<typename std::remove_reference<T>::type>;

    template <typename T>
    inline
    typename std::enable_if<std::is_same<tt<T>, T>::value,  A<tt<T>, name, serializable> >::type
    operator=(T&& t)
    {
      return A<tt<T>, name, serializable>(std::forward<T>(t));
    }

    template <typename T>
    inline
    typename std::enable_if<not std::is_same<tt<T>, T>::value,  A<tt<T>, name, serializable> >::type
    operator=(T&& t)
    {
      return A<tt<T>, name, serializable>(tt<T>(t));
    }

    template <typename T>
    inline A<std::initializer_list<T>, name, serializable> operator=(std::initializer_list<T> t)
    {
      return A<std::initializer_list<T>, name, serializable>(std::forward<std::initializer_list<T>>(t));
    }

    N<1> operator*() { return N<1>(); }

  };

  // Assign the attributes of src to the attributes of dst.
  template <int N, typename D, typename S>
  struct iod_assign
  {
    static inline void run(D& dst, const S& src)
    {
      const auto& src_attr = src.template get_nth<N>();

      typedef typename S::template nth_type<N>::symbol_type symbol_type;
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

template <>
struct iod_object<>
{
  template <typename S>
  int
  find_symbol();
};

template <typename T, typename ...Tail>
struct iod_object<T, Tail...> : public T, public iod_object<Tail...>
{
  typedef iod_object<T, Tail...> self;
  typedef iod_object<Tail...> super;

  typedef T attr_type;

  enum { size = sizeof...(Tail) };

  typedef std::tuple<T, Tail...> tuple_type;

  template <unsigned N>
  using nth_type = typename std::tuple_element<N, std::tuple<T, Tail...>>::type;

  // -----------------------------------------
  // Retrive type of attribute with symbol S.
  template <unsigned N, typename C, typename S>
  struct attribute_type_;

  template <unsigned N, typename S>
  struct attribute_type_<N, S, S>
  {
    typedef nth_type<N> type;
  };

  template <unsigned N, typename C, typename S>
  struct attribute_type_
  {
    typedef typename attribute_type_<N+1, typename nth_type<N+1>::symbol_type, S>::type type;
  };

  template <typename S>
  using attribute_type = typename attribute_type_<0, typename nth_type<0>::symbol_type, S>::type;

  template <typename S>
  using attribute_value_type = typename attribute_type_<0, typename nth_type<0>::symbol_type, S>::type::value_type;

  // -----------------------------------------

  inline iod_object()
    : T(),
      iod_object<Tail...>()
  {
  }

  inline iod_object(const T&& attr, const Tail&&... tail)
    : T(attr),
      iod_object<Tail...>(std::forward<const Tail>(tail)...)
  {
  }

  super& get_super()
  {
    return *this;
  }

  const super& get_super() const
  {
    return *this;
  }

  // Get the attribute associated with the symbol S.
  template <typename S>
  attribute_type<S>& symbol_to_attribute(S = S())
  {
    return *static_cast<attribute_type<S>*>(this);
  }

  template <typename S>
  const attribute_type<S>& symbol_to_attribute(S = S()) const
  {
    return *static_cast<const attribute_type<S>*>(this);
  }

  // Call to methods.
  template <typename S, typename ...Args>
  void operator()(S, const Args... args)
  {
    symbol_to_attribute<S>().value()(*this, args...);
  }

  // Get an attribute with a symbol.
  // Example: o.get(name), o.get<decltype(name)>()
  template <typename S>
  auto get(S = S()) -> attribute_value_type<S>&
  {
    return symbol_to_attribute<S>().value();
  }
  template <typename S>
  auto get(S = S()) const -> const attribute_value_type<S>&
  {
    return symbol_to_attribute<S>().value();
  }

  // Get the nth attribute.
  template <unsigned N>
  typename std::enable_if<N!=0, nth_type<N>&>::type
  get_nth() { return get_super().get_nth<N-1>(); }

  template <unsigned N>
  typename std::enable_if<N==0, nth_type<N>&>::type
  get_nth() { return T::value(); }

  template <unsigned N>
  typename std::enable_if<N!=0, const nth_type<N>&>::type
  get_nth() const { return get_super().get_nth<N-1>(); }

  template <unsigned N>
  typename std::enable_if<N==0, const nth_type<N>&>::type
  get_nth() const { return *this; }

  // Example: o.get(name), o.get<decltype(name)>()

  template <typename O, typename... Otail>
  self& operator=(const iod_object<O, Otail...>& o)
  {
    iod_internals::iod_assign<sizeof...(Otail), self, iod_object<O, Otail...> >::run(*this, o);
    return *this;
  }


};

template <typename ...T>
inline iod_object<T...> iod(T&&... args)
{
  return iod_object<T...>(std::forward<T>(args)...);
}

template <typename T, typename ...Tail>
inline std::vector<T> iod_array(const T& t, Tail... args)
{
  std::vector<T> res;
  res.reserve(1 + sizeof...(args));
  iod_internals::array_fill<T, Tail...>(t, args..., res);
  return res;
}

#define iod_define_attribute(NAME)                                      \
template <typename T, typename S, int serializable>                   \
struct NAME##_attribute                                                 \
{                                                                       \
  enum { is_serializable = serializable };                              \
  typedef S symbol_type;                                                \
  typedef iod_internals::transform_type_t<typename std::remove_reference<T>::type> value_type; \
  inline NAME##_attribute() {}                                          \
  inline NAME##_attribute(T&& t) : NAME(t) {}        \
  inline const char* attribute_name() const { return #NAME; }           \
  inline value_type& value() { return NAME; }                           \
  inline const value_type& value() const { return NAME; }               \
  value_type NAME;                                                      \
};                                                                      \
                                                                        \
template <int serializable>                                             \
 struct NAME##_attribute_name;                                          \
                                                                        \
template <int s>                                                        \
using self_##NAME = NAME##_attribute_name<s>;                           \
                                                                        \
template <int serializable = 0>                                         \
 struct NAME##_attribute_name : iod_internals::attribute_name<self_##NAME, NAME##_attribute, serializable> \
{                                                                       \
  enum { is_iod_attribute = 1 };                                            \
  template <int s>                                                      \
  using self = NAME##_attribute_name<s>;                                \
  typedef iod_internals::attribute_name<self_##NAME, NAME##_attribute, serializable> super; \
  using super::super;                                                   \
  using super::operator=;                                               \
    template <typename T>                                               \
  static auto accessor(T t) -> decltype(t.NAME) { return t.NAME; }      \
  inline static const char* attribute_name() { return #NAME; }          \
};                                                                      \
namespace {NAME##_attribute_name<0> NAME; }

template <typename T, typename U = void>
struct is_iod_attribute
{
  enum { value = 0 };
};

template <typename T>
struct is_iod_attribute<T, typename std::enable_if<T::is_iod_attribute, void>::type>
{
  enum { value = 1 };
};

#endif
