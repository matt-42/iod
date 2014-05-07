#ifndef IOD_API_HH_
# define IOD_API_HH_

#include <string>
#include <vector>
#include <tuple>


template <typename ...T>
struct iod_object;

template <typename ...T>
iod_object<T...> iod(T... args);

template <typename T, typename ...Tail>
std::vector<T> iod_array(const T& t, Tail... args);

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
    //typedef std::vector<typename transform_type<T>::type> type;
    typedef std::vector<T> type;
  };

  template <typename T>
  using transform_type_t = typename transform_type<T>::type;

  template <typename T>
  void array_fill(std::vector<T>& array)
  {
  };

  template <typename T, typename ...Tail>
  void array_fill(const T& a, const Tail&... args, std::vector<T>& array)
  {
    array.push_back(a);
    array_fill(args..., array);
  };

}



template <>
struct iod_object<>
{
};

template <typename T, typename ...Tail>
struct iod_object<T, Tail...> : public T, public iod_object<Tail...>
{
  iod_object()
    : T(),
      iod_object<Tail...>()
  {
  }

  iod_object(const T& attr, const Tail... tail)
    : T(attr),
      iod_object<Tail...>(tail...)
  {
  }

};


template <typename ...T>
iod_object<T...> iod(T... args)
{
  return iod_object<T...>(args...);
};

template <typename T, typename ...Tail>
std::vector<T> iod_array(const T& t, Tail... args)
{
  std::vector<T> res;
  res.reserve(1 + sizeof...(args));
  iod_internals::array_fill<T, Tail...>(t, args..., res);
  return res;
};

#define iod_define_attribute(NAME)                                      \
template <typename T>                                                   \
struct NAME##_attr                                                      \
{                                                                       \
  typedef iod_internals::transform_type_t<T> value_type;                               \
  NAME##_attr() {}                                                      \
  NAME##_attr(T t) : NAME(t) {}                                         \
  const char* attribute_name() const { return #NAME; }                  \
  value_type& value() { return NAME; }                                  \
  const value_type& value() const { return NAME; }                      \
  value_type NAME;                                                      \
};                                                                      \
                                                                        \
struct NAME##_attr_builder                                              \
{                                                                       \
  template <typename T>                                                 \
    NAME##_attr<T> operator=(T t) { return NAME##_attr<T>(t); } \
  template <typename T>                                                 \
    NAME##_attr<std::initializer_list<T>> operator=(const std::initializer_list<T>& t) { return NAME##_attr<std::initializer_list<T>>(t); } \
} NAME##_;

#endif
