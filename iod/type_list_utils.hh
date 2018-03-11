#pragma once

namespace iod
{
  namespace tl
  {

    namespace detail
    {

      template<std::size_t...> struct seq{ using type = seq; };

      template<class S1, class S2> struct concat;

      template<std::size_t... I1, std::size_t... I2>
      struct concat<seq<I1...>, seq<I2...>>
        : seq<I1..., (sizeof...(I1)+I2)...>{};

      template<class S1, class S2>
      using concat_t = typename concat<S1, S2>::type;

      template<std::size_t N> struct gen_seq;
      template<std::size_t N> using gen_seq_t = typename gen_seq<N>::type;

      template<std::size_t N>
      struct gen_seq : concat_t<gen_seq_t<N/2>, gen_seq_t<N - N/2>>{};

      template<> struct gen_seq<0> : seq<>{};
      template<> struct gen_seq<1> : seq<0>{};
  
      template<std::size_t>
      struct eat
      {
        template <typename T>
        eat(T) {}
      };

      template<std::size_t... Is>
      struct get_nth_helper
      {
        template <typename T, typename... Tail>
        static decltype(auto) run(detail::eat<Is>..., T e, Tail&&...)
        {
          return e;
        }
      };

      template <std::size_t... Is, typename... T>
      decltype(auto) get_nth_impl(seq<Is...>, T&&... l)
      {
        return get_nth_helper<Is...>::run(std::forward<T>(l)...);
      }
  
    }
  
    template <std::size_t N, typename... T>
    decltype(auto) get_nth(T&&... l)
    {
      return detail::get_nth_impl(detail::gen_seq_t<N>{}, std::forward<T>(l)...);
    }

    template <std::size_t N, typename... T>
    using get_nth_type = decltype(get_nth<N>(std::declval<T>()...));

    // Get the position of a type in a type list.
    template <typename T, int N, typename... L>
    struct get_type_position_;

    template <typename T, int N, typename L1, typename... L>
    struct get_type_position_<T, N, L1, L...>
    {
      enum { value = get_type_position_<T, N + 1, L...>::value };
    };

    template <typename T, int N, typename... L>
    struct get_type_position_<T, N, T, L...>
    {
      enum { value = N };
    };


    template <typename T, int N>
    struct get_type_position_<T, N>
    {
      static_assert(N == -1, "get type position error: type not found.");
      enum { value = N };
    };

    template <typename T, typename... L>
    struct get_type_position
    {
      static const std::size_t value = get_type_position_<T, 0, L...>::value;
    };

    // Check if a list contains a type.
    template <typename T, typename... L>
    struct type_list_has;

    template <typename T, typename L1, typename... L>
    struct type_list_has<T, L1, L...>
    {
      enum { value = type_list_has<T, L...>::value };
    };

    template <typename T, typename... L>
    struct type_list_has<T, T, L...> { enum { value = true }; };
    template <typename T>
    struct type_list_has<T>          { enum { value = false }; };
    
    //find_first.
    template <typename F, typename T1, typename... T>
    auto find_first(F pred, T1 e, T&&... l)
    {
      if (pred())
        return e;
      else find_first(pred, l...);
    }

    template <typename F>
    auto find_first(F /*pred*/)
    {
      return not_found();
    }
    
  }

  using tl::detail::gen_seq_t;

}
