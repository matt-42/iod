#pragma once

#include <type_traits>
#include <algorithm>
#include <iod/symbol.hh>
#include <iod/grammar.hh>
#include <iod/utils.hh>
#include <iod/symbols.hh>
#include <iod/foreach.hh>
#include <iod/linq_aggregators.hh>
#include <iod/linq_evaluate.hh>

namespace iod
{
  using namespace s;

  namespace linq_internals
  {

    template <typename R, typename S>
    auto format_record(R& req, const S& r, std::enable_if_t<decltype(req.select)::_size != 0>* = nullptr)
    {
      // Format with a non empty select statement: select(f1 = t1[x], f2 = t2[y])
      return foreach(req.select) |
        [&r] (auto m) { return m.symbol() = evaluate(m.value(), r); };
    }

    template <typename R, typename S>
    auto format_record(R& req, const S& r,
                       std::enable_if_t<decltype(req.select)::_size == 0 and
                       !decltype(req.from)::template _has<_as_t>::value>* = nullptr)
    {
      // Format with an empty select statement and no table alias.
      //return r.template get_nth<0>();
      return r;
    }

    template <typename R, typename S>
    auto format_record(R& req, const S& r,
                       std::enable_if_t<decltype(req.select)::_size == 0 and
                       decltype(req.from)::template _has<_as_t>::value>* = nullptr)
    {
      // Format with an empty select statement and a table alias.
      return r;
    }

    template <typename R>
    auto compute_request_record_type(const R& req)
    {
      const auto& from_table = *req.from.table;
      typedef std::remove_reference_t<decltype(*req.from.table)> from_table_type;

      return static_if<R::template _has<_join_t>::value>
        ([](const auto& req){
          const auto& from_table = *req.from.table;
          const auto& join_table = *req.join.table;

          auto from_name = req.from.get(_as, s::_1);
          auto join_name = req.join.get(_as, s::_2);
          
          typedef decltype(select_format(req, D(from_name = typename decltype(from_table)::value_type(),
                                                join_name = typename decltype(join_table)::value_type())))
            record_type;
          record_type sample;
          return format_record(req, sample);
        },
        [](const auto& req){
          auto from_name = req.from.get(_as, _1);
          typedef decltype(D(from_name = typename from_table_type::value_type())) record_type1;
          record_type1 sample;
          return format_record(req, sample);
        }, req);
    }

    template <typename R, typename T, typename F>
    void exec_iteration(R& req, T& table, F f)
    {
      static_if<has_symbol<R, _group_by_t>::value>
        ([] (auto& req, auto& table, auto f) {
          auto gb = req.get(_group_by, D(_criteria = 42));
          std::sort(table.begin(), table.end(), [&gb] (const auto& a, const auto& b)
                    {
                      return evaluate(gb.criteria, a) < evaluate(gb.criteria, b);
                    });

          for (int i = 0; i < int(table.size());)
          {
            int j = i + 1;
            while (j < int(table.size()) and
                   evaluate(gb.criteria, table[i]) == evaluate(gb.criteria, table[i + j]))
              j++;
            T group(table.begin() + i, table.begin() + j);
            f(group);
            i = j;
          }
        },
          [] (auto& req, auto& table, auto f) {
            for (auto& t : table)
              f(t);
          }, req, table, f);
    }
    
    template <typename R, typename T, typename F>
    void exec_simple_iteration(R& req, const T& table, F f)
    {
      for (auto& t : table)
      {
        auto r = D(req.from.get(_as, _1) = t);
        if (evaluate(req.get(_where, D(_condition = true)).condition, r))
          f(format_record(req, r));
      }

    }

    template <typename R, typename T, typename F>
    void exec_order_by(R& req, T& table, F f)
    {
      if (req.has(_order_by))
      {
        auto order = req.get(_order_by, D(_order = 1)).order;
        std::sort(table.begin(), table.end(), [&order] (const auto& a, const auto& b)
                  {
                    return evaluate(order, a) < evaluate(order, b);
                  });
      }
    }

    template <typename R, typename T, typename F>
    auto exec_aggregate(R& req, T& group, F f)
    {
      //auto aggregators = req.select;
      auto aggrs = foreach(req.select) | [&] (auto& m) {
        //return m.symbol() = aggregate_initialize(m.value(), decltype(group[0])());
        return m.symbol() = aggregate_initialize(m.value(), group[0]);
      };
      for(auto e : group)
        foreach_attribute_value([&] (auto& a) { a.take(e); }, aggrs);

      return foreach(aggrs) | [] (auto& m) {
        return m.symbol() = m.value().result();
      };

    }

    template <typename R, typename F>
    void exec_table(R& req, F f)
    {
      static_if<!has_symbol<R, _inner_join_t>::value and
                !has_symbol<R, _order_by_t>::value and
                !has_symbol<R, _group_by_t>::value and
                decltype(req.select)::_empty
                >
        ([] (auto& req, auto f) {
          exec_simple_iteration(req, *req.from.table, f);
        },
        [] (auto& req, auto f) {

          // Compute { table1_name = table1, table2_name = table2, ...}
          auto tables = static_if<R::template _has<_inner_join_t>::value>
            ([] (auto& req) {
              //auto inner_join_name = req.inner_join.get(_as, _2);
              return D(req.from.get(_as, _1) = req.from.table,
                         req.inner_join.get(_as, _1) = req.inner_join.table);
            },
              [] (auto& req) {
                return D(req.from.get(_as, _1) = req.from.table);
              }, req);

          // The actual intermediate record type.
          auto record_sample = foreach(tables) | [&] (auto m) { return m.symbol() = (*tables[m])[0]; };

          // Compute the intermediate table on which we will iterate.
          auto v = static_if<has_symbol<R, _inner_join_t>::value>
            ([] (auto& req, auto& tables) { // if join
              const auto& from_table = *tables.template get_nth<0>();
              const auto& join_table = *tables.template get_nth<1>();

              auto on_condition = req.inner_join.get(_on, true);
              auto where_condition = req.get(_where, D(_condition = true)).condition;

              auto record_sample = foreach(tables) | [&] (auto m) { return m.symbol() = (*tables[m])[0]; };
              typedef decltype(record_sample) record_type;

              // Bruteforce O(n2) inner join. => optimization?
              std::vector<record_type> out;
              for (int i = 0; i < int(from_table.size()); i++)
              {
                for (int j = 0; j < int(join_table.size()); j++)
                {
                  auto r = foreach(tables) | [&] (auto m) {
                    return m.symbol() = (*tables[m])[((void*)tables[m] == (void*)&from_table) ? i : j]; 
                  };
                  if (evaluate(on_condition, r) and evaluate(where_condition, r))
                    out.push_back(r);
                }
              }
              return out;
            },
            [] (auto& req, auto& tables) { // if no join
              auto record_sample = foreach(tables) | [&] (auto m) { return m.symbol() = (*tables[m])[0]; };
              typedef decltype(record_sample) record_type;

              auto table1 = *tables.template get_nth<0>();
              auto where_condition = req.get(_where, D(_condition = true)).condition;
              std::vector<record_type> out;
              for (int i = 0; i < int(table1.size()); i++)
              {
                auto r = foreach(tables) | [&] (auto m) { return m.symbol() = (*tables[m])[i]; };
                if (evaluate(where_condition, r))
                  out.push_back(r);
              }
              return out;
            }, req, tables);

          // Sort if requested.
          if (req.has(_order_by))
            exec_order_by(req, v, f);

          static_if<has_aggregator<decltype(req.select)>::value>
            ([] (auto& req, auto& v, auto f) // If request contains aggregators.
             {
               static_if<has_symbol<R, _group_by_t>::value>
                 ([] (auto& req, auto f, auto& v) { // If group_by
                   exec_iteration(req, v, 
                                  [&] (const auto& group) { return f(exec_aggregate(req, group, f)); });
                 },
                 [] (auto& req, auto f, auto& v) { // If no group by
                   f(exec_aggregate(req, v, f));
                 }, req, f, v);
             },
             [] (auto& req, auto& v, auto f) // If no aggregators.
             {

               static_if<has_symbol<R, _group_by_t>::value>
                 ([] (auto& req, auto f, auto& v) { //If group_by
                   exec_iteration(req, v, 
                                  [&] (const auto& group) { 
                                    return static_if<decltype(req.select)::_empty>
                                      ([] (const auto& group) { return group; },
                                       [&req] (const auto& group) {
                                         std::vector<decltype(format_record(req, group[0]))> v;
                                         for (const auto& t : group) v.push_back(format_record(req, t));
                                         return v;
                                       }, group);
                                  });
                 },
                   [] (auto& req, auto f, auto& v) { // If no group by
                     exec_iteration(req, v,
                                    [&] (const auto& elt) { return f(format_record(req, elt)); });
                   }, req, f, v);

             }, req, v, f);

          }, req, f);
    }

    template <typename T>
    struct query
    {
      query(const T& q) : q(q) {}

      template <typename U>
      auto make_query(U u)
      {
        return query<U>(u);
      }

      inline auto from_exp() { return D(); } 

      // from(vector, _as(_table_name))
      template <typename E, typename... O>
      auto from_exp(const function_call_exp<_as_t, E>& e, O&&... tail)
      { return cat(from_exp(tail...), D(_as = std::get<0>(e.args))); }
      
      template <typename... E>
      auto select(const E&... e) { return make_query(cat(q, _select = D(e...))); }

      template <typename E>
      auto where(E e) { return make_query(cat(q, _where = D(_condition = e))); }
      template <typename E>
      auto group_by(E e) { return make_query(cat(q, _group_by = D(_criteria = e))); }
      template <typename Q, typename... E>
      auto from(Q&& table, const E&... e)
      { return make_query(cat(q, _from = cat(D(_table = &table), from_exp(e...)))); }

      template <typename Q, typename... E>
      auto inner_join(Q&& table, E... e) { return make_query(cat(q,
                                                                 _inner_join = D(_table = &table, e...))); }
      template <typename Q>
      auto order_by(Q&& order) { return make_query(cat(q, _order_by = D(_order = order))); }

      template <typename F>
      void operator|(F f) { return linq_internals::exec_table(q, f); }

      auto to_array() { 
        typedef decltype(linq_internals::compute_request_record_type(q)) record_type;
        std::vector<record_type> v;
        (*this) | [&v] (const auto& r) { v.push_back(r); };
        return v;
      }

      T q;
    };

  }


  auto linq = linq_internals::query<sio<>>(sio<>());

}
