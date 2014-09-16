
template <typename ...T>
using last_element = typename std::tuple_element<sizeof...(T) - 1, std::tuple<T...> >::type;

template <int N, typename ...T>
using n_element = typename std::tuple_element<N, std::tuple<T...> >::type;

template <unsigned N, typename ...T>
auto iod_get(iod_object<T...>& o) -> decltype(((n_element<N, T...>*)0)->value())
{
  n_element<N, T...>* attr = &o;
  return attr->value();
}

template <int N = 0, typename T, typename ...TS>
struct other_elements
{
  typedef std::tuple_concat
}

template <typename ...A>
inline iod_object<T, TS..., A...> iod_extend(const iod_object<>& o, A&&... args)
{
  T* attr = &o;
  iod_object<T...>* tail = &o;
  return iod_extend(attr->value(), *tail, args);
};

template <unsigned N, typename A, typename... AT, typename ...BT>
inline iod_object<T, TS..., A...> iod_extend(const iod_object<A, AT...>& a,
                                             const iod_object<BT...>& b,
                                             ARGS... args)
{
  return iod_extend(a, b, *tail, args);
};
