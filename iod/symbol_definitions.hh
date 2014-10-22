//  Convert the symbol file into a C++ header using sed, or another text processing tool
//  with the equivalent command:
//  sed -e 's/^\([a-zA-Z1-9_]\)\([a-zA-Z1-9_]*\)/#ifndef IOD_SYMBOL__\U\1\L\2\n\#define IOD_SYMBOL__\U\1\L\2\n    iod_define_symbol(\1\2, _\U\1\L\2)\n#endif/' symbols.sb > symbol_definitions.hh

#ifndef IOD_SYMBOL__Select
#define iod_symbol__Select
    iod_define_symbol(select, _Select)
#endif
#ifndef IOD_SYMBOL__Update
#define iod_symbol__Update
    iod_define_symbol(update, _Update)
#endif
#ifndef IOD_SYMBOL__Insert
#define iod_symbol__Insert
    iod_define_symbol(insert, _Insert)
#endif
#ifndef IOD_SYMBOL__Delete_
#define iod_symbol__Delete_
    iod_define_symbol(delete_, _Delete_)
#endif
#ifndef IOD_SYMBOL__Where
#define iod_symbol__Where
    iod_define_symbol(where, _Where)
#endif
#ifndef IOD_SYMBOL__Order_by
#define iod_symbol__Order_by
    iod_define_symbol(order_by, _Order_by)
#endif
#ifndef IOD_SYMBOL__From
#define iod_symbol__From
    iod_define_symbol(from, _From)
#endif
#ifndef IOD_SYMBOL__Table
#define iod_symbol__Table
    iod_define_symbol(table, _Table)
#endif
#ifndef IOD_SYMBOL__Order
#define iod_symbol__Order
    iod_define_symbol(order, _Order)
#endif
#ifndef IOD_SYMBOL__Set
#define iod_symbol__Set
    iod_define_symbol(set, _Set)
#endif
#ifndef IOD_SYMBOL__Into
#define iod_symbol__Into
    iod_define_symbol(into, _Into)
#endif
#ifndef IOD_SYMBOL__Values
#define iod_symbol__Values
    iod_define_symbol(values, _Values)
#endif
#ifndef IOD_SYMBOL__Join
#define iod_symbol__Join
    iod_define_symbol(join, _Join)
#endif
#ifndef IOD_SYMBOL__Inner_join
#define iod_symbol__Inner_join
    iod_define_symbol(inner_join, _Inner_join)
#endif
#ifndef IOD_SYMBOL__Group_by
#define iod_symbol__Group_by
    iod_define_symbol(group_by, _Group_by)
#endif
#ifndef IOD_SYMBOL__On
#define iod_symbol__On
    iod_define_symbol(on, _On)
#endif
#ifndef IOD_SYMBOL__Condition
#define iod_symbol__Condition
    iod_define_symbol(condition, _Condition)
#endif
#ifndef IOD_SYMBOL__Else_
#define iod_symbol__Else_
    iod_define_symbol(else_, _Else_)
#endif
#ifndef IOD_SYMBOL__Elt
#define iod_symbol__Elt
    iod_define_symbol(elt, _Elt)
#endif
#ifndef IOD_SYMBOL__Cpt
#define iod_symbol__Cpt
    iod_define_symbol(cpt, _Cpt)
#endif
#ifndef IOD_SYMBOL__As
#define iod_symbol__As
    iod_define_symbol(as, _As)
#endif
#ifndef IOD_SYMBOL__Criteria
#define iod_symbol__Criteria
    iod_define_symbol(criteria, _Criteria)
#endif
#ifndef IOD_SYMBOL__Sum
#define iod_symbol__Sum
    iod_define_symbol(sum, _Sum)
#endif
#ifndef IOD_SYMBOL__Avg
#define iod_symbol__Avg
    iod_define_symbol(avg, _Avg)
#endif
#ifndef IOD_SYMBOL__Stddev
#define iod_symbol__Stddev
    iod_define_symbol(stddev, _Stddev)
#endif

#ifndef IOD_SYMBOL__1
#define iod_symbol__1
    iod_define_symbol(_1, _1)
#endif
#ifndef IOD_SYMBOL__2
#define iod_symbol__2
    iod_define_symbol(_2, _2)
#endif

#ifndef IOD_SYMBOL__Optional
#define iod_symbol__Optional
    iod_define_symbol(optional, _Optional)
#endif

#ifndef IOD_SYMBOL__Json_symbol
#define iod_symbol__Json_symbol
    iod_define_symbol(json_symbol, _Json_symbol)
#endif
