#pragma once

#include <type_traits>
#include <algorithm>
#include <iod/symbol.hh>
#include <iod/grammar.hh>
#include <iod/utils.hh>
#include <iod/symbols.hh>

namespace iod
{
  namespace linq_internals
  {

    // Terminals.
    template <typename A, typename T>
    inline const A&
    evaluate(const A& v, const T&, std::enable_if_t<
             not std::is_base_of<Exp<A>, A>::value and
             not std::is_base_of<symbol<A>, A>::value
             >* = nullptr)
    {
      return v;
    }

    template <int N, int S, typename ATTR>
    struct find_first_predicate_runner;

    template <int S, typename ATTR>
    struct find_first_predicate_runner<S, S, ATTR>
    {
      template <typename T>
      static inline not_found run(const T& t)
      {
        return not_found();
      }
    };

    template <int N, int S, typename ATTR>
    struct find_first_predicate_runner
    {
      template <typename T>
      static inline auto run(const T& t)
      {
        typedef typename std::remove_reference_t<decltype(t.template get_nth<N>())> found_type;
        
        return static_if<has_symbol<found_type, ATTR>::value>
          ([] (auto t) { return t.template get_nth<N>(); },
           [] (auto t) { return find_first_predicate_runner<N + 1, S, ATTR>::run(t); },
           t);
      }
    };

    template <typename ATTR, typename T>
    auto find_first_attribute_with_predicate(const T& t)
    {
      return find_first_predicate_runner<0, T::_size, ATTR>::run(t);
    }

    // Access to attributes.
    // If ctx has the symbol \s return ctx.s.
    // Otherwise, find the first attribute x of ctx containing s and return x.s.
    template <typename S, typename T>
    inline auto
    evaluate(const symbol<S>& s, const T& ctx)
    {
      return static_if<has_symbol<T, S>::value>
        ([] (auto s, const auto& ctx) { return S().member_access(ctx); },
         [] (auto s, const auto& ctx) { return S().member_access(find_first_attribute_with_predicate<S>(ctx)); },
         s, ctx);
    }

    // Access to attributes of a named variable: variable[attribute]
    template <typename O, typename M, typename T>
    inline auto
    evaluate(const array_subscript_exp<O, M>& s, const T& ctx)
    {
      return M().member_access(evaluate(s.object, ctx));
    }

#define iod_stl_binary_op(OP, NAME)                             \
    template <typename A, typename B, typename T>               \
    inline auto                                                 \
    evaluate(const NAME##_exp<A, B>& e, const T& ctx)           \
    {                                                           \
      return evaluate(e.lhs, ctx) OP evaluate(e.rhs, ctx);      \
    }

    iod_stl_binary_op(+, plus);
    iod_stl_binary_op(-, minus);
    iod_stl_binary_op(*, mult);
    iod_stl_binary_op(<<, shiftl);
    iod_stl_binary_op(>>, shiftr);
    iod_stl_binary_op(<, inf);
    iod_stl_binary_op(<=, inf_eq);
    iod_stl_binary_op(>, sup);
    iod_stl_binary_op(>=, sup_eq);
    iod_stl_binary_op(==, eq);
    iod_stl_binary_op(!=, neq);
    iod_stl_binary_op(&, logical_and);
    iod_stl_binary_op(^, logical_xor);
    iod_stl_binary_op(|, logical_or);
    iod_stl_binary_op(&&, and);
    iod_stl_binary_op(||, or);

#undef iod_stl_binary_op

  }

}
