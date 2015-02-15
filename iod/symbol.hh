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

#define iod_define_symbol_body(SYMBOL, NAME)                            \
    constexpr NAME##_t() {}                                             \
    typedef iod::symbol<NAME##_t> super;                                \
    using super::operator=;                                             \
                                                                        \
    inline const char* name() const { return #SYMBOL; }                 \
                                                                        \
    template <typename T>                                               \
    inline const auto& member_access(const T& o) const { return o.SYMBOL; } \
    template <typename T>                                               \
    inline auto& member_access(T& o) const { return o.SYMBOL; } \
                                                                        \
    template <typename T, typename... A>                                \
    inline auto method_call(const T& o, A... args) const { return o.SYMBOL(args...); } \
                                                                        \
    template <typename T, typename INFO = iod::sio<>>                   \
      struct variable_type : public iod::variable<variable_type<T, INFO>> { \
                                                                        \
      typedef T value_type;                                             \
      typedef INFO attributes_type;                                     \
      typedef NAME##_t symbol_type;                                     \
                                                                        \
      variable_type() {}                                                \
      template <typename V>                                             \
      variable_type(V v) : SYMBOL(v) {}                                 \
      inline value_type& value() { return SYMBOL; }                     \
      inline const value_type& value() const { return SYMBOL; }         \
      auto symbol() const { return NAME##_t(); }                        \
      auto symbol_name() const { return #SYMBOL; }                      \
      auto attributes() const { return INFO(); }                        \
                                                                        \
      value_type SYMBOL; \
    };

#define iod_define_symbol(SYMBOL)                         \
  namespace s {                                                 \
  struct _##SYMBOL##_t : iod::symbol<_##SYMBOL##_t>                       \
  {                                                             \
  iod_define_symbol_body(SYMBOL, _##SYMBOL)                          \
  };                                                            \
  constexpr _##SYMBOL##_t _##SYMBOL;                                      \
  }

  template <int N>
  struct int_symbol : iod::symbol<int_symbol<N>>
  {
    constexpr int_symbol() {}
    typedef iod::symbol<iod::int_symbol<N>> super;
    using super::operator=;
    static constexpr const int to_int = N;
  };

  template <typename T> struct is_int_symbol               : std::false_type {};
  template <int N>      struct is_int_symbol<int_symbol<N>> : std::true_type {};
  template <typename T> struct is_int_symbol<const T> : is_int_symbol<T> {};
  template <typename T> struct is_int_symbol<T&> : is_int_symbol<T> {};

#define iod_define_number_symbol(NUMBER)                                \
  namespace s {                                                         \
  typedef iod::int_symbol<NUMBER>  _##NUMBER##_t; \
  constexpr _##NUMBER##_t _##NUMBER;                                    \
  }

}

int __compiler_insert_symbols_here__();

#endif
