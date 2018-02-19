#pragma once

#include <type_traits>
#include <iod/sio.hh>
#include <iod/callable_traits.hh>
#include <iod/tuple_utils.hh>
#include <iod/utils.hh>
#include <iod/bind_method.hh>

namespace iod
{

  namespace di
  {

    template <typename M, typename C, typename... T>
    decltype(auto) call_factory_instantiate(M& factory, C&& ctx, typelist<T...>*)
    {
      return factory.instantiate(std::forward<T>(tuple_get_by_type<T>(ctx))...);
    }

    template <typename E, typename C, typename... T>
    decltype(auto) call_factory_instantiate_static(C&& ctx, typelist<T...>*)
    {
      return std::decay_t<E>::instantiate(std::forward<T>(tuple_get_by_type<T>(ctx))...);
    }

    // dependencies_of allows to forward dependencies of a function to another.
    template <typename B>
    struct dependencies_of_
    {
      static auto instantiate()
      {
        return dependencies_of_<B>();
      }

      std::tuple<> deps;
    };

    template <typename... D>
    struct dependencies_of_<std::tuple<D...>>
    {
      dependencies_of_(D... d) : deps(d...) {}

      static auto instantiate(D&&... deps)
      {
        return dependencies_of_<std::tuple<D...>>(deps...);
      };

      std::tuple<std::remove_reference_t<D>...> deps;
    };

    template <typename F>
    using dependencies_of = dependencies_of_<callable_arguments_tuple_t<F>>;

    template <typename M>
    struct has_instantiate_static_method
    {
      template <typename X>
      static char test(decltype(&X::instantiate));

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

    template <typename T>
    using di_factory_return_t = std::remove_reference_t<iod::callable_return_type_t<decltype(&std::remove_reference_t<T>::instantiate)>>;

    template <typename T, typename... T2, typename I>
    struct find_di_factory_iterator<std::tuple<T, T2...>, I,
                                    std::enable_if_t<!std::is_same<di_factory_return_t<T>, I>::value> >
      : public find_di_factory_iterator<std::tuple<T2...>, I>
    {};

    template <typename T, typename... T2, typename I>
    struct find_di_factory_iterator<std::tuple<T, T2...>, I,
                                    std::enable_if_t<std::is_same<di_factory_return_t<T>, I>::value> >
    {
      typedef T type;
    };

    template <typename T, typename... T2, typename I>
    struct find_di_factory_iterator<std::tuple<T, T2...>, I,
                                    std::enable_if_t<!std::is_class<std::remove_reference_t<T>>::value> >
      : public find_di_factory_iterator<std::tuple<T2...>, I>
    {};

    template <typename T, typename I>
    using find_di_factory_t = std::remove_reference_t<typename find_di_factory_iterator<T, std::remove_reference_t<I>>::type>;

    // Provide an element of type E:
    //   If \to_inject contains an element of type E, return it.
    //   If \to_inject contains an element f and f.instantiate return type is E,
    //       return f.instantiate(...). Note: \to_inject must contains the arguments of f::instantiate.
    //   Otherwise, call the static method E::instantiate.
    template <typename E, typename T, typename F>
    decltype(auto) di_meta_instantiate(T&& to_inject, F f)
    {

      typedef std::decay_t<T> T2;
      typedef std::remove_const_t<std::remove_reference_t<E>> E2;

      typedef find_di_factory_t<T2, E2> FT;
      return iod::static_if<!std::is_same<FT, not_found>::value>(
        // If to_inject embed a factory, call it.
        [f] (auto&& /*deps*/, auto* /*e*/, auto* ft_) -> decltype(auto) {
          typedef std::remove_pointer_t<decltype(ft_)> FT_;
          typedef iod::callable_arguments_list_t<decltype(&FT_::instantiate)> ARGS;
          auto instantiate = [&] (auto&& ctx) -> decltype(auto)
            {
              return call_factory_instantiate(tuple_get_by_type<FT>(ctx), ctx, (ARGS*)0);
            };
          return f(instantiate, (ARGS*)0);
        },
        // If the argument type provide a static instantiate method, call it.
        [f] (auto&& /*deps*/, auto* e, auto* /*ft_*/) -> decltype(auto) {
          typedef std::remove_pointer_t<std::remove_pointer_t<decltype(e)>> E2;
          static_assert(has_instantiate_static_method<E2>::value,
                        "Dependency injection failed. Cannot resolve.");
          typedef iod::callable_arguments_list_t<decltype(&E2::instantiate)> ARGS;
          auto instantiate = [&] (auto&& ctx) -> decltype(auto) { return call_factory_instantiate_static<std::remove_pointer_t<E>>(ctx, (ARGS*)0); };
          return f(instantiate, (ARGS*)0);
        },
        to_inject, (E2*)0, (FT*)0);
    }

    template <typename E,typename... T>
    decltype(auto) instantiate(std::enable_if_t<!typelist_embeds_any_ref_of<typelist<T...>, E>::value>*,
                               T&&... to_inject)
    {
      return di_meta_instantiate<E>(std::forward_as_tuple(to_inject...),
                                    [&] (auto instantiate, auto* /*args*/) -> decltype(auto)
        {
          return instantiate(std::forward_as_tuple(to_inject...));
        });
    }

    template <typename E,typename... T>
    decltype(auto) instantiate(std::enable_if_t<typelist_embeds_any_ref_of<typelist<T...>, E>::value>*,
                               T&&... to_inject)
    {
      return arg_get_by_type<E>(std::forward<T>(to_inject)...);
    }

    template <typename E, typename T>
    decltype(auto) create_di_ctx_rec(T&& ctx);


    template <typename... A> 
    struct create_di_ctx_iterator {};

    template <> 
    struct create_di_ctx_iterator<>
    {
      template <typename P>
      static decltype(auto) run(P&& prev)
      {
        return std::forward<P>(prev);
      }
    };

    template <typename A1, typename... A>
    struct create_di_ctx_iterator<A1, A...>
    {
      template <typename P>
      static decltype(auto) run(P&& prev)
      {
        return create_di_ctx_iterator<A...>::template run(create_di_ctx_rec<A1>(prev));
      }
    };

    // Instantiate the injection list from the types A...
    // returns the concatenation of ctx, elements of type A... and
    // the elements required to build them.
    template <typename C, typename... A>
    decltype(auto) create_di_ctx_list_rec(C&& ctx, typelist<A...>*)
    {
      return create_di_ctx_iterator<A...>::template run(ctx);
    }

    template <typename E, typename T>
    decltype(auto) create_di_ctx_rec(T&& ctx)
    {
      return static_if<typelist_embeds_any_ref_of<std::decay_t<T>, E>::value>(
        [] (auto&& ctx) { return ctx; },
        [] (auto&& ctx) {
          return di_meta_instantiate<E>(*(typelist_to_tuple_t<std::decay_t<T>>*)42,
                                        [&] (auto /*instantiate*/, auto args) -> decltype(auto)
            {
              typedef std::remove_pointer_t<decltype(args)> ARGS;
              auto deps = create_di_ctx_list_rec(ctx, (ARGS*)0);
              return typelist_cat(deps, typelist<E>());
            });
        }, ctx);
    }

    template <typename... B, typename... A, typename F>
    decltype(auto) create_stack_and_call(typelist<>*,
                                         typelist<A...>*,
                                         F fun, B&&... to_inject)
    {
      return fun(arg_get_by_type<A>(std::forward<B>(to_inject)...)...);
    }

    template <typename C1, typename... C, typename... A, typename... B, typename F>
    decltype(auto) create_stack_and_call(typelist<C1, C...>*,
                                         typelist<A...>* args,
                                         F fun, B&&... to_inject)
    {
      return create_stack_and_call((typelist<C...>*)0, args, fun,
                                   std::forward<B>(to_inject)...,
                                   instantiate<C1>(0, std::forward<B>(to_inject)...));
    }

    // Call fun with its required argument A...
    // by picking in args... or calling A::instantiate()
    template <typename F, typename... A, typename... B>
    decltype(auto)
    call_with_di2(F fun, std::tuple<A...>* /*arguments*/, B&&... to_inject)
    {
      // Compute the context type containing the arguments plus the
      // dependencies of the possible A::instantiate(...) methods.
      typedef
        std::remove_reference_t<
        decltype(create_di_ctx_list_rec(
                   std::declval<typelist<B...>>(),
                   (typelist<A...>*)0))
        > ctx_type;

      // typedef typename tuple_to_list<ctx_type>::type ctx_typelist;
      return create_stack_and_call((ctx_type*)0, (typelist<A...>*)0, fun, to_inject...);
    }

  }

  // Dependency injection entry point for function.
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

  // Dependency injection entry point for object methods.
  //
  // di_call(object, method, args_to_inject_or_factories...)
  //
  static const struct di_call_method_t
  {
    di_call_method_t() {}

    template <typename O, typename F, typename... A>
    auto operator()(O& o , F fun, A&&... to_inject) const
    {
      return di::call_with_di2(bind_method(o, fun), (callable_arguments_tuple_t<F>*)0, std::forward<A>(to_inject)...);
    }
  } di_call_method;
}
