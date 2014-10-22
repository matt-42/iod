#pragma once

namespace iod
{
  template <typename T, typename U>
  struct tuple_embeds : public std::false_type {};

  template <typename U>
  struct tuple_embeds<std::tuple<>, U> : public std::false_type {};

  template <typename T1, typename... T>
  struct tuple_embeds<std::tuple<T1, T...>, T1> : public std::true_type {};

  template <typename T1, typename... T, typename U>
  struct tuple_embeds<std::tuple<T1, T...>, U> : public tuple_embeds<std::tuple<T...>, U> {};


}
