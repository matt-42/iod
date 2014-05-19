#ifndef IOD_API_HH_
# define IOD_API_HH_

#include <string>
#include <vector>
#include <tuple>
#include <functional>

template <typename ...T>
struct iod_object;

template <typename ...T>
inline iod_object<T...> iod(T... args);

template <typename T, typename ...Tail>
inline std::vector<T> iod_array(const T& t, Tail... args);

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
  };

  template <typename T, typename ...Tail>
  inline void array_fill(const T& a, const Tail&... args, std::vector<T>& array)
  {
    array.push_back(a);
    array_fill(args..., array);
  };

  template <template <int> class N, template <class, class> class A, int serializable>
  struct attribute_name
  {
    typedef N<serializable> name;

    enum { is_serializable = serializable };

    template <typename T>
    inline A<T, name> operator=(T t)
    {
      return A<T, name>(t);
    }

    template <typename T>
    inline A<std::initializer_list<T>, name> operator=(const std::initializer_list<T>& t)
    {
      return A<std::initializer_list<T>, name>(t);
    }

    N<1> operator*() { return N<1>(); }

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

  inline iod_object()
    : T(),
      iod_object<Tail...>()
  {
  }

  inline iod_object(const T& attr, const Tail... tail)
    : T(attr),
      iod_object<Tail...>(tail...)
  {
  }

  template <typename S>
  typename std::enable_if<std::is_same<S, typename T::symbol_type>::value, self&>::type
  find_symbol()
  {
    return *this;
  }

  template <typename S>
  auto find_symbol()
    -> typename std::enable_if<!std::is_same<S, typename T::symbol_type>::value,
                               decltype(super().template find_symbol<S>())>::type
  {
    super* o = this;
    return o->template find_symbol<S>();
  }

  template <typename S, typename ...Args>
  void operator()(S, const Args... args)
  {
    auto o = this->template find_symbol<S>();
    S::accessor(o)(*this, args...);
  }

};

template <typename ...T>
inline iod_object<T...> iod(T... args)
{
  return iod_object<T...>(args...);
};

template <typename T, typename ...Tail>
inline std::vector<T> iod_array(const T& t, Tail... args)
{
  std::vector<T> res;
  res.reserve(1 + sizeof...(args));
  iod_internals::array_fill<T, Tail...>(t, args..., res);
  return res;
};

#define iod_define_attribute(NAME)                                      \
  template <typename T, typename S>                                     \
struct NAME##_attribute                                                 \
{                                                                       \
  typedef S symbol_type;                                                \
  typedef iod_internals::transform_type_t<T> value_type;                \
  inline NAME##_attribute() {}                                          \
  inline NAME##_attribute(T t) : NAME(t) {}                             \
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
template <int serializable>                                             \
 struct NAME##_attribute_name : iod_internals::attribute_name<self_##NAME, NAME##_attribute, serializable> \
{                                                                       \
  template <int s>                                                      \
  using self = NAME##_attribute_name<s>;                                \
  typedef iod_internals::attribute_name<self_##NAME, NAME##_attribute, serializable> super; \
  using super::super;                                                   \
  using super::operator=;                                               \
    template <typename T>                                               \
  static auto accessor(T t) -> decltype(t.NAME) { return t.NAME; }      \
  inline static const char* attribute_name() { return #NAME; }          \
};                                                                      \
namespace { NAME##_attribute_name<0> NAME; }

#endif
