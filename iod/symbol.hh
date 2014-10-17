#ifndef IOD_SYMBOL_HH_
# define IOD_SYMBOL_HH_

#include <iod/tags.hh>
#include <iod/grammar.hh>

#include <iod/iod.hh>

namespace iod
{

  template <typename E>
  struct symbol : public member_accessible<E>, public callable<E>,
                  public assignable<E>,
                  public Exp<E>
  {
    typedef E symbol_type;
    constexpr symbol() {}
    using assignable<E>::operator=;
  };

#define iod_define_symbol(NAME)                                         \
  namespace s                                                           \
  {                                                                     \
  struct _##NAME : iod::symbol<_##NAME>                                 \
  {                                                                     \
    constexpr _##NAME() {}                                              \
    typedef iod::symbol<_##NAME> super;                                 \
    using super::operator=;                                             \
                                                                        \
    inline const char* name() const { return #NAME; }                   \
                                                                        \
    template <typename T>                                               \
    inline auto attribute_access(const T& o) const { return o.NAME; }   \
                                                                        \
    template <typename T, typename... A>                                \
    inline auto method_call(const T& o, A... args) const { return o.NAME(args...); } \
                                                                        \
    template <typename T, typename INFO = iod::iod_object<>> struct variable_type { \
                                                                        \
      typedef T value_type;                                             \
      typedef INFO attributes_type;                                     \
      typedef _##NAME symbol_type;                                      \
                                                                        \
      variable_type() {}                                                \
      template <typename V>                                             \
      variable_type(V v) : NAME(v) {}                                   \
      inline value_type& value() { return NAME; }                       \
      inline const value_type& value() const { return NAME; }           \
      auto symbol() const { return _##NAME(); }                         \
      auto symbol_name() const { return #NAME; }                        \
      auto attributes() const { return INFO(); }                        \
                                                                        \
      value_type NAME;                                                  \
    };                                                                  \
  };                                                                    \
  constexpr _##NAME NAME;                                               \
  }


//  #define iod_define_symbol(NAME) iod_define_symbol_(NAME, s)
}

#endif
