//  Convert the symbol file into a C++ header using sed, or another text processing tool
//  with the equivalent command:
//  sed -e 's/^\([a-zA-Z1-9_]\)\([a-zA-Z1-9_]*\)/#ifndef IOD_SYMBOL__\U\1\L\2\n\#define IOD_SYMBOL__\U\1\L\2\n    iod_define_symbol(\1\2, _\U\1\L\2)\n#endif/' symbols.sb > symbol_definitions.hh

#ifndef IOD_SYMBOL__select
#define IOD_SYMBOL__select
    iod_define_symbol(select, _select)
#endif
#ifndef IOD_SYMBOL__update
#define IOD_SYMBOL__update
    iod_define_symbol(update, _update)
#endif
#ifndef IOD_SYMBOL__insert
#define IOD_SYMBOL__insert
    iod_define_symbol(insert, _insert)
#endif
#ifndef IOD_SYMBOL__delete_
#define IOD_SYMBOL__delete_
    iod_define_symbol(delete_, _delete_)
#endif
#ifndef IOD_SYMBOL__where
#define IOD_SYMBOL__where
    iod_define_symbol(where, _where)
#endif
#ifndef IOD_SYMBOL__order_by
#define IOD_SYMBOL__order_by
    iod_define_symbol(order_by, _order_by)
#endif
#ifndef IOD_SYMBOL__from
#define IOD_SYMBOL__from
    iod_define_symbol(from, _from)
#endif
#ifndef IOD_SYMBOL__table
#define IOD_SYMBOL__table
    iod_define_symbol(table, _table)
#endif
#ifndef IOD_SYMBOL__order
#define IOD_SYMBOL__order
    iod_define_symbol(order, _order)
#endif
#ifndef IOD_SYMBOL__set
#define IOD_SYMBOL__set
    iod_define_symbol(set, _set)
#endif
#ifndef IOD_SYMBOL__into
#define IOD_SYMBOL__into
    iod_define_symbol(into, _into)
#endif
#ifndef IOD_SYMBOL__values
#define IOD_SYMBOL__values
    iod_define_symbol(values, _values)
#endif
#ifndef IOD_SYMBOL__join
#define IOD_SYMBOL__join
    iod_define_symbol(join, _join)
#endif
#ifndef IOD_SYMBOL__inner_join
#define IOD_SYMBOL__inner_join
    iod_define_symbol(inner_join, _inner_join)
#endif
#ifndef IOD_SYMBOL__group_by
#define IOD_SYMBOL__group_by
    iod_define_symbol(group_by, _group_by)
#endif
#ifndef IOD_SYMBOL__on
#define IOD_SYMBOL__on
    iod_define_symbol(on, _on)
#endif
#ifndef IOD_SYMBOL__condition
#define IOD_SYMBOL__condition
    iod_define_symbol(condition, _condition)
#endif
#ifndef IOD_SYMBOL__else_
#define IOD_SYMBOL__else_
    iod_define_symbol(else_, _else_)
#endif
#ifndef IOD_SYMBOL__elt
#define IOD_SYMBOL__elt
    iod_define_symbol(elt, _elt)
#endif
#ifndef IOD_SYMBOL__cpt
#define IOD_SYMBOL__cpt
    iod_define_symbol(cpt, _cpt)
#endif
#ifndef IOD_SYMBOL__as
#define IOD_SYMBOL__as
    iod_define_symbol(as, _as)
#endif
#ifndef IOD_SYMBOL__criteria
#define IOD_SYMBOL__criteria
    iod_define_symbol(criteria, _criteria)
#endif
#ifndef IOD_SYMBOL__sum
#define IOD_SYMBOL__sum
    iod_define_symbol(sum, _sum)
#endif
#ifndef IOD_SYMBOL__avg
#define IOD_SYMBOL__avg
    iod_define_symbol(avg, _avg)
#endif
#ifndef IOD_SYMBOL__stddev
#define IOD_SYMBOL__stddev
    iod_define_symbol(stddev, _stddev)
#endif

#ifndef IOD_SYMBOL__optional
#define IOD_SYMBOL__optional
    iod_define_symbol(optional, _optional)
#endif

#ifndef IOD_SYMBOL__json_key
#define IOD_SYMBOL__json_key
    iod_define_symbol(json_key, _json_key)
#endif
#ifndef IOD_SYMBOL__json_skip
#define IOD_SYMBOL__json_skip
    iod_define_symbol(json_skip, _json_skip)
#endif
