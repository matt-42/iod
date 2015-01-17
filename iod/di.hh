#pragma once

#include <type_traits>
#include <iod/sio.hh>
#include <iod/callable_traits.hh>
#include <iod/tuple_utils.hh>
#include <iod/utils.hh>

namespace iod
{

  namespace di
  {

    template <typename M, typename C, typename... T>
    auto call_factory_make(M& factory, C&& ctx, std::tuple<T...>*)
    {
      return factory.make(tuple_get_by_type<T>(ctx)...);
    }

    template <typename E, typename C, typename... T>
    auto call_factory_make_static(C&& ctx, std::tuple<T...>*)
    {
      return E::make(tuple_get_by_type<T>(ctx)...);
    }

    // dependencies_of allows to forward dependencies of a function to another.
    template <typename B>
    struct dependencies_of_
    {
      static auto make()
      {
        return dependencies_of_<B>();
      }
     
      std::tuple<> deps;
    };
  
    template <typename... D>
    struct dependencies_of_<std::tuple<D...>>
    {
      dependencies_of_(D... d) : deps(d...) {}

      static auto make(D&&... deps)
      {
        return dependencies_of_<std::tuple<D...>>(deps...);
      };

      std::tuple<std::remove_reference_t<D>...> deps;
    };
  
    template <typename F>
    using dependencies_of = dependencies_of_<callable_arguments_tuple_t<F>>;

    // tuple_iterate provides an iteration on a tuple with knowledge of the
    // return value of the previous loop step.
    template<unsigned N, unsigned SIZE, typename F, typename A, typename P>
    inline
    auto
    tuple_iterate_loop(std::enable_if_t<N == SIZE>*, F, A&&, P&& prev)
    {
      return prev;
    }
  
    template<unsigned N, unsigned SIZE, typename F, typename A, typename P>
    inline
    auto
    tuple_iterate_loop(std::enable_if_t<N < SIZE>*, F f, A&& t, P&& prev)
    {
      return tuple_iterate_loop<N + 1, SIZE>(0, f, t, f(std::get<N>(t), prev));
    }

    template <typename T, typename P>
    struct tuple_iterate_caller
    {
      tuple_iterate_caller(T&& t, P&& prev_init) : t_(t), prev_init_(prev_init) {}

      template <typename F>
      auto operator|(F f)
      {
        const int size = std::tuple_size<std::remove_reference_t<T>>::value;
        return tuple_iterate_loop<0, size>(0, f, t_, prev_init_);
      }

      const T t_;
      P prev_init_;
    };
  
    template <typename T, typename C>
    auto tuple_iterate(T&& t, C&& init)
    {
      return tuple_iterate_caller<T, C>(t, init);
    }

    template <typename M>
    struct has_make_static_method
    {
      template <typename X>
      static char test(decltype(&X::make));

      template <typename X>
      static int test(...);

      static const int value = (sizeof(test<std::remove_reference_t<M>>(0)) == sizeof(char));
    };

    template <typename T>
    typename T::factory_type return_factory_type(int x, typename T::factory_type* = 0);
    template <typename T>
    not_found return_factory_type(...);

    template <typename T, typename I, typename X = void>
    struct find_di_factory_iterator
    {
      typedef not_found type;
    };

    template <typename T, typename... T2, typename I>
    struct find_di_factory_iterator<std::tuple<T, T2...>, I,
                                    std::enable_if_t<!std::is_same<std::remove_reference_t<callable_return_type_t<decltype(&std::remove_reference_t<T>::make)> >,
                                                                   I>::value> >
      : public find_di_factory_iterator<std::tuple<T2...>, I>
    {};
    
    template <typename T, typename... T2, typename I>
    struct find_di_factory_iterator<std::tuple<T, T2...>, I,
                                    std::enable_if_t<std::is_same<std::remove_reference_t<callable_return_type_t<decltype(&std::remove_reference_t<T>::make)> >,
                                                                  I>::value> >
    {
      typedef T type;
    };

    template <typename T, typename I>
    using find_di_factory_t = std::remove_reference_t<typename find_di_factory_iterator<T, I>::type>;
  
    // Provide an element of type E:
    //   If \to_inject contains an element of type E, return it.
    //   If \to_inject contains an element f and f.make return type is E,
    //       return f.make(...). Note: \to_inject must contains the arguments of f::make.
    //   Otherwise, call the static method E::make.
    struct X {};
    template <typename E, typename T, typename F>
    auto di_meta_instantiate(T&& to_inject, F f)
    {

      typedef std::remove_reference_t<T> T2;
      typedef std::remove_const_t<std::remove_reference_t<E>> E2;

      return static_if<tuple_embeds<T2, E2>::value or
                       tuple_embeds<T2, E2&>::value
                       >(
                         [&] (auto& to_inject) -> auto {
                           // If to_inject already embeds an element of type E, return it.
                           auto make = [&] (auto) { return tuple_get_by_type<E>(to_inject); };
                           return f(make, (std::tuple<>*)0);
                         },
                         [&] (auto& to_inject) -> auto {

                           typedef find_di_factory_t<T2, E2> FT;

                           return iod::static_if<!std::is_same<FT, not_found>::value>(
                             // If to_inject embed a factory, call it.
                             [&] (auto& deps, auto* e, auto* ft_) -> auto {
                               typedef std::remove_pointer_t<decltype(ft_)> FT_;
                               typedef iod::callable_arguments_tuple_t<decltype(&FT_::make)> ARGS;
                               auto make = [&] (auto& ctx)
                                 {
                                   return call_factory_make(tuple_get_by_type<FT>(deps), ctx, (ARGS*)0);
                                 };
                               return f(make, (ARGS*)0);
                             },
                             // If the argument type provide a static instantiate method, call it.
                             [&] (auto& deps, auto* e, auto* ft_) -> auto {
                               typedef std::remove_pointer_t<decltype(e)> E2;
                               static_assert(has_make_static_method<E2>::value,
                                             "Dependency injection failed. Cannot resolve.");
                               typedef iod::callable_arguments_tuple_t<decltype(&E2::make)> ARGS;
                               auto make = [&] (auto& ctx) { return call_factory_make_static<E>(ctx, (ARGS*)0); };
                               return f(make, (ARGS*)0);
                               //return 1;
                             },
                             to_inject, (E*)0, (FT*)0);
                         },
                         to_inject);
    }

    template <typename E,typename T>
    auto instantiate(T&& to_inject)
    {
      return di_meta_instantiate<E>(to_inject,
                                    [&] (auto make, auto* args)
                                    {
                                      return make(to_inject);
                                    });
    }  

    template <typename E, typename T>
    auto create_di_ctx_rec(T& ctx);

    // Instantiate the injection list from the types A...
    // returns the concatenation of ctx, elements of type A... and
    // the elements required to build them.
    template <typename C, typename... A>
    auto create_di_ctx_list_rec(C&& ctx, std::tuple<A...>*)
    {
      std::tuple<std::remove_reference_t<A>*...> tmp;
      return tuple_iterate(tmp, ctx) | [&] (auto* e, auto& prev) { // Foreach instantiate argument e.
        typedef std::remove_reference_t<std::remove_pointer_t<decltype(e)>> E;
        return create_di_ctx_rec<E>(prev);
      };
    }
  
    template <typename E, typename T>
    auto create_di_ctx_rec(T& ctx)
    {
      return di_meta_instantiate<E>(ctx,
                                    [&] (auto make, auto args)
                                    {
                                      typedef std::remove_pointer_t<decltype(args)> ARGS;
                                      auto deps = create_di_ctx_list_rec(ctx, (ARGS*)0);
                                      auto n = make(deps);
                                      return std::tuple_cat(deps, std::make_tuple(n));
                                    });
    }

    template <typename... T, typename... U>
    decltype(auto) create_di_ctx(std::enable_if_t<sizeof...(T) == sizeof...(U)>*,
                                 std::tuple<T...>*, U&&... args)
    {
      return std::tuple<T...>(std::forward<U>(args)...);
    };
  
    template <typename... T, typename... U>
    decltype(auto) create_di_ctx(std::enable_if_t<(sizeof...(T) > sizeof...(U))>*,
                                 std::tuple<T...>* ctx, U&&... args)
    {
      return create_di_ctx(0, ctx, args...,
                           instantiate<std::tuple_element_t<sizeof...(U), std::tuple<T...>>>
                           (std::forward_as_tuple(args...)));
    };

    // Call fun with its required argument A...
    // by picking in args... or calling A::make()
    template <typename F, typename... A, typename... B>
    auto call_with_di2(F fun, std::tuple<A...>*, B&&... to_inject)
    {
      // function arguments type.
      callable_arguments_tuple_t<F>* arguments;

      // Compute the context type containing the arguments plus the
      // dependencies of the possible A::make(...) methods.
      typedef decltype(create_di_ctx_list_rec(std::make_tuple(to_inject...), arguments)) ctx_type;

      // Instantiate it.
      ctx_type ctx = create_di_ctx(0, (ctx_type*)0, to_inject...);

      // Call the function.
      return fun(tuple_get_by_type<A>(ctx)...);
    }

  }

  // Dependency injection entry point.
  //
  // di_call(fun, args_to_inject_or_factories...)
  //
  static const struct di_call_t
  {
    di_call_t() {}

    template <typename F, typename... A>
    auto operator()(F fun, A&&... to_inject) const
    {
      return di::call_with_di2(fun, (callable_arguments_tuple_t<F>*)0, std::forward<A>(to_inject)...);
    }
  } di_call;

}
