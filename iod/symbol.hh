#ifndef IOD_SYMBOL_HH_
# define IOD_SYMBOL_HH_

#include <iod/tags.hh>
#include <iod/grammar.hh>

#include <iod/sio.hh>

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

  template <typename E>
  struct variable
  {
  };

#define iod_define_symbol(SYMBOL, NAME)                                 \
  namespace s {                                                         \
  struct NAME##_t : iod::symbol<NAME##_t>                               \
  {                                                                     \
    constexpr NAME##_t() {}                                            \
    typedef iod::symbol<NAME##_t> super;                               \
    using super::operator=;                                             \
                                                                        \
    inline const char* name() const { return #SYMBOL; }                 \
                                                                        \
    template <typename T>                                               \
    inline auto attribute_access(const T& o) const { return o.SYMBOL; } \
                                                                        \
    template <typename T, typename... A>                                \
    inline auto method_call(const T& o, A... args) const { return o.SYMBOL(args...); } \
                                                                        \
    template <typename T, typename INFO = iod::sio<>>                   \
      struct variable_type : public iod::variable<variable_type<T, INFO>> { \
                                                                        \
      typedef T value_type;                                             \
      typedef INFO attributes_type;                                     \
      typedef NAME##_t symbol_type;                                    \
                                                                        \
      variable_type() {}                                                \
      template <typename V>                                             \
      variable_type(V v) : SYMBOL(v) {}                                 \
      inline value_type& value() { return SYMBOL; }                     \
      inline const value_type& value() const { return SYMBOL; }         \
      auto symbol() const { return NAME##_t(); }                       \
      auto symbol_name() const { return #SYMBOL; }                      \
      auto attributes() const { return INFO(); }                        \
                                                                        \
      value_type SYMBOL;                                                \
    };                                                                  \
  };                                                                    \
  constexpr NAME##_t NAME;                                              \
  }
}

#endif
