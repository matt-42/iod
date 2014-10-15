#pragma once

#include <iod/symbol.hh>

//  Convert the symbol file into a C++ header using sed, or another text processing tool
//  with the equivalent command:
//  sed -e 's/^\([a-zA-Z1-9_]\+\)/#ifndef IOD_SYMBOL_\1\n\#define IOD_SYMBOL_\1\n    iod_define_symbol(\1)\n#endif/' symbols.sb > symbols.hh

#ifndef IOD_SYMBOL_select
#define IOD_SYMBOL_select
    iod_define_symbol(select)
#endif
#ifndef IOD_SYMBOL_update
#define IOD_SYMBOL_update
    iod_define_symbol(update)
#endif
#ifndef IOD_SYMBOL_insert
#define IOD_SYMBOL_insert
    iod_define_symbol(insert)
#endif
#ifndef IOD_SYMBOL_delete_
#define IOD_SYMBOL_delete_
    iod_define_symbol(delete_)
#endif
#ifndef IOD_SYMBOL_where
#define IOD_SYMBOL_where
    iod_define_symbol(where)
#endif
#ifndef IOD_SYMBOL_order_by
#define IOD_SYMBOL_order_by
    iod_define_symbol(order_by)
#endif
#ifndef IOD_SYMBOL_from
#define IOD_SYMBOL_from
    iod_define_symbol(from)
#endif
#ifndef IOD_SYMBOL_table
#define IOD_SYMBOL_table
    iod_define_symbol(table)
#endif
#ifndef IOD_SYMBOL_order
#define IOD_SYMBOL_order
    iod_define_symbol(order)
#endif
#ifndef IOD_SYMBOL_set
#define IOD_SYMBOL_set
    iod_define_symbol(set)
#endif
#ifndef IOD_SYMBOL_into
#define IOD_SYMBOL_into
    iod_define_symbol(into)
#endif
#ifndef IOD_SYMBOL_values
#define IOD_SYMBOL_values
    iod_define_symbol(values)
#endif
#ifndef IOD_SYMBOL_join
#define IOD_SYMBOL_join
    iod_define_symbol(join)
#endif
#ifndef IOD_SYMBOL_inner_join
#define IOD_SYMBOL_inner_join
    iod_define_symbol(inner_join)
#endif
#ifndef IOD_SYMBOL_group_by
#define IOD_SYMBOL_group_by
    iod_define_symbol(group_by)
#endif
#ifndef IOD_SYMBOL_on
#define IOD_SYMBOL_on
    iod_define_symbol(on)
#endif
#ifndef IOD_SYMBOL_condition
#define IOD_SYMBOL_condition
    iod_define_symbol(condition)
#endif
#ifndef IOD_SYMBOL_else_
#define IOD_SYMBOL_else_
    iod_define_symbol(else_)
#endif
#ifndef IOD_SYMBOL_elt
#define IOD_SYMBOL_elt
    iod_define_symbol(elt)
#endif
#ifndef IOD_SYMBOL_cpt
#define IOD_SYMBOL_cpt
    iod_define_symbol(cpt)
#endif
#ifndef IOD_SYMBOL_as
#define IOD_SYMBOL_as
    iod_define_symbol(as)
#endif
#ifndef IOD_SYMBOL_criteria
#define IOD_SYMBOL_criteria
    iod_define_symbol(criteria)
#endif
#ifndef IOD_SYMBOL_sum
#define IOD_SYMBOL_sum
    iod_define_symbol(sum)
#endif
#ifndef IOD_SYMBOL_avg
#define IOD_SYMBOL_avg
    iod_define_symbol(avg)
#endif
#ifndef IOD_SYMBOL_stddev
#define IOD_SYMBOL_stddev
    iod_define_symbol(stddev)
#endif

#ifndef IOD_SYMBOL__1
#define IOD_SYMBOL__1
    iod_define_symbol(_1)
#endif
#ifndef IOD_SYMBOL__2
#define IOD_SYMBOL__2
    iod_define_symbol(_2)
#endif

#ifndef IOD_SYMBOL_optional
#define IOD_SYMBOL_optional
    iod_define_symbol(optional)
#endif

#ifndef IOD_SYMBOL_json_symbol
#define IOD_SYMBOL_json_symbol
    iod_define_symbol(json_symbol)
#endif
