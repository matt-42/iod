#pragma once

#include <iod/symbol.hh>

//  Convert the symbol file into a C++ header using sed, or another text processing tool
//  with the equivalent command:
//  sed -e 's/^\([a-zA-Z1-9_]\)\([a-zA-Z1-9_]\+\)$/iod_define_symbol(\1\2, _\U\1\2)/' symbols.sb > symbols.hh

iod_define_symbol(select, _Select)
iod_define_symbol(update, _Update)
iod_define_symbol(insert, _Insert)
iod_define_symbol(delete_, _Delete_)
iod_define_symbol(where, _Where)
iod_define_symbol(order_by, _Order_by)
iod_define_symbol(from, _From)
iod_define_symbol(table, _Table)
iod_define_symbol(order, _Order)
iod_define_symbol(set, _Set)
iod_define_symbol(into, _Into)
iod_define_symbol(values, _Values)
iod_define_symbol(join, _Join)
iod_define_symbol(inner_join, _Inner_join)
iod_define_symbol(group_by, _Group_by)
iod_define_symbol(on, _On)
iod_define_symbol(condition, _Condition)
iod_define_symbol(else_, _Else_)
iod_define_symbol(elt, _Elt)
iod_define_symbol(cpt, _Cpt)
iod_define_symbol(as, _As)
iod_define_symbol(criteria, _Criteria)
iod_define_symbol(sum, _Sum)
iod_define_symbol(avg, _Avg)
iod_define_symbol(stddev, _Stddev)

iod_define_symbol(_1, _1)
iod_define_symbol(_2, _2)

iod_define_symbol(optional, _Optional)

iod_define_symbol(json_symbol, _Json_symbol)
