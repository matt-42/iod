#ifndef IOD_SYMBOL_HH_
# define IOD_SYMBOL_HH_

#include <iod/tags.hh>
#include <iod/grammar.hh>

#include <iod/iod.hh>

namespace iod
{

  template <typename E>
  struct symbol : public member_accessible<E>, public callable<E>, public Exp<E>
  {
    typedef E symbol_type;

    constexpr symbol() {}

    // Create a variable from value.
    template <typename T>
    constexpr inline auto operator=(T t) const
    {
      return typename E::template variable_type<T>(t);
    }

    // Create a variable from a initializer_list.
    template <typename T>
    constexpr inline auto operator=(std::initializer_list<T> t) const
    {
      return typename E::template variable_type<std::initializer_list<T>>(t);
    }

  };

  template <typename E, typename ATTRS = iod_object<>>
  struct named_variable
  {
    typedef ATTRS attributes_type;

    auto attributes() const { return ATTRS(); }

    // Allows to attach attributes to variables.
    // For example, a json object definition could be:
    // iod(name = std::string() | optional | json_symbol(the_json_symbol))
    template <typename A>
    auto& operator|(A)
    {
      auto new_attrs = iod_cat(ATTRS(), A());
      return *(named_variable<E, decltype(new_attrs)>*) this;
    }
  };

  template <typename T>
  struct get_attribute_type { typedef T type; };
  template <>
  struct get_attribute_type<const char*> { typedef std::string type; };
  template <int N>
  struct get_attribute_type<const char[N]> { typedef std::string type; };
  template <typename T>
  struct get_attribute_type<std::initializer_list<T> > { typedef std::vector<T> type; };
  template <typename T>
  using get_attribute_type_t =
    typename get_attribute_type<typename std::remove_reference<T>::type>::type;

#define iod_define_symbol(NAME)                                       \
  namespace s                                                           \
  {                                                                     \
  struct _##NAME;                               \
  template <typename T>                                                 \
  struct NAME##_variable : public iod::named_variable<T>              \
  {                                                                     \
    typedef _##NAME symbol_type;                             \
    typedef iod::get_attribute_type_t<T> value_type;                    \
                                                                        \
    NAME##_variable() {}                                                \
    NAME##_variable(const value_type& v) : NAME(v) {}                   \
    inline value_type& value() { return NAME; }                         \
    inline const value_type& value() const { return NAME; }             \
    inline auto symbol() const;                                         \
    inline const char* symbol_name() const;                             \
    value_type NAME;                                                    \
  };                                                                    \
  struct _##NAME : iod::symbol<_##NAME>                     \
  {                                                                     \
    constexpr _##NAME() {}                                                  \
    typedef iod::symbol<_##NAME> super;                           \
    using super::operator=;                                             \
    template <typename U>                                               \
    using variable_type = NAME##_variable<iod::get_attribute_type_t<U>>; \
    inline const char* name() const { return #NAME; }                   \
    template <typename T>                                               \
    inline auto attribute_access(const T& o) const { return o.NAME; }         \
    template <typename T, typename... A>                                \
    inline auto method_call(const T& o, A... args) const { return o.NAME(args...); } \
  };                                                                    \
  template <typename T>                                                 \
  inline const char* NAME##_variable<T>::symbol_name() const { return symbol_type().name(); } \
  template <typename T>                                                 \
  inline auto NAME##_variable<T>::symbol() const { return symbol_type(); } \
  constexpr _##NAME NAME;                                                 \
  } \


//  #define iod_define_symbol(NAME) iod_define_symbol_(NAME, s)
}

#endif
