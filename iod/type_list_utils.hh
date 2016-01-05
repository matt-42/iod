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
      decltype(auto) get_nth_impl(seq<Is...> seq, T&&... l)
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

    //find_first.
    template <typename F, typename T1, typename... T>
    auto find_first(F pred, T1 e, T&&... l)
    {
      if (pred())
        return e;
      else find_first(pred, l...);
    }

    template <typename F>
    auto find_first(F pred)
    {
      return not_found();
    }
    
  }

  using tl::detail::gen_seq_t;

}
