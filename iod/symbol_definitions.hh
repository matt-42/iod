//  Convert the symbol file into a C++ header using sed, or another text processing tool
//  with the equivalent command:
//  sed -e 's/^\([a-zA-Z1-9_]\)\([a-zA-Z1-9_]*\)/#ifndef IOD_SYMBOL__\U\1\L\2\n\#define IOD_SYMBOL__\U\1\L\2\n    iod_define_symbol(\1\2, _\U\1\L\2)\n#endif/' symbols.sb > symbol_definitions.hh

#ifndef IOD_SYMBOL__Select
#define IOD_SYMBOL__Select
    iod_define_symbol(select, _Select)
#endif
#ifndef IOD_SYMBOL__Update
#define IOD_SYMBOL__Update
    iod_define_symbol(update, _Update)
#endif
#ifndef IOD_SYMBOL__Insert
#define IOD_SYMBOL__Insert
    iod_define_symbol(insert, _Insert)
#endif
#ifndef IOD_SYMBOL__Delete_
#define IOD_SYMBOL__Delete_
    iod_define_symbol(delete_, _Delete_)
#endif
#ifndef IOD_SYMBOL__Where
#define IOD_SYMBOL__Where
    iod_define_symbol(where, _Where)
#endif
#ifndef IOD_SYMBOL__Order_by
#define IOD_SYMBOL__Order_by
    iod_define_symbol(order_by, _Order_by)
#endif
#ifndef IOD_SYMBOL__From
#define IOD_SYMBOL__From
    iod_define_symbol(from, _From)
#endif
#ifndef IOD_SYMBOL__Table
#define IOD_SYMBOL__Table
    iod_define_symbol(table, _Table)
#endif
#ifndef IOD_SYMBOL__Order
#define IOD_SYMBOL__Order
    iod_define_symbol(order, _Order)
#endif
#ifndef IOD_SYMBOL__Set
#define IOD_SYMBOL__Set
    iod_define_symbol(set, _Set)
#endif
#ifndef IOD_SYMBOL__Into
#define IOD_SYMBOL__Into
    iod_define_symbol(into, _Into)
#endif
#ifndef IOD_SYMBOL__Values
#define IOD_SYMBOL__Values
    iod_define_symbol(values, _Values)
#endif
#ifndef IOD_SYMBOL__Join
#define IOD_SYMBOL__Join
    iod_define_symbol(join, _Join)
#endif
#ifndef IOD_SYMBOL__Inner_join
#define IOD_SYMBOL__Inner_join
    iod_define_symbol(inner_join, _Inner_join)
#endif
#ifndef IOD_SYMBOL__Group_by
#define IOD_SYMBOL__Group_by
    iod_define_symbol(group_by, _Group_by)
#endif
#ifndef IOD_SYMBOL__On
#define IOD_SYMBOL__On
    iod_define_symbol(on, _On)
#endif
#ifndef IOD_SYMBOL__Condition
#define IOD_SYMBOL__Condition
    iod_define_symbol(condition, _Condition)
#endif
#ifndef IOD_SYMBOL__Else_
#define IOD_SYMBOL__Else_
    iod_define_symbol(else_, _Else_)
#endif
#ifndef IOD_SYMBOL__Elt
#define IOD_SYMBOL__Elt
    iod_define_symbol(elt, _Elt)
#endif
#ifndef IOD_SYMBOL__Cpt
#define IOD_SYMBOL__Cpt
    iod_define_symbol(cpt, _Cpt)
#endif
#ifndef IOD_SYMBOL__As
#define IOD_SYMBOL__As
    iod_define_symbol(as, _As)
#endif
#ifndef IOD_SYMBOL__Criteria
#define IOD_SYMBOL__Criteria
    iod_define_symbol(criteria, _Criteria)
#endif
#ifndef IOD_SYMBOL__Sum
#define IOD_SYMBOL__Sum
    iod_define_symbol(sum, _Sum)
#endif
#ifndef IOD_SYMBOL__Avg
#define IOD_SYMBOL__Avg
    iod_define_symbol(avg, _Avg)
#endif
#ifndef IOD_SYMBOL__Stddev
#define IOD_SYMBOL__Stddev
    iod_define_symbol(stddev, _Stddev)
#endif

#ifndef IOD_SYMBOL__Optional
#define IOD_SYMBOL__Optional
    iod_define_symbol(optional, _Optional)
#endif

#ifndef IOD_SYMBOL__Json_symbol
#define IOD_SYMBOL__Json_symbol
    iod_define_symbol(json_symbol, _Json_symbol)
#endif
