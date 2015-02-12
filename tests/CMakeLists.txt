cmake_minimum_required(VERSION 2.8)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_definitions(-std=c++14 -Wall)

#add_executable(iod_query_stl iod_query_stl.cc)
#add_test(iod_query_stl iod_query_stl)

#add_executable(iod_query_sql iod_query_sql.cc)
#add_test(iod_query_sql iod_query_sql)

add_executable(json json.cc)
add_test(json json)

add_executable(core core.cc)
add_test(core core)

#add_executable(linq linq.cc)
#add_test(linq linq)

add_executable(callable_traits callable_traits.cc)
add_test(callable_traits callable_traits)

add_executable(apply apply.cc)
add_test(apply apply)

add_executable(foreach foreach.cc)
add_test(foreach foreach)

add_executable(readme readme.cc)
add_test(readme readme)

add_executable(di di.cc)
add_test(di di)

add_executable(bind_method bind_method.cc)
add_test(bind_method bind_method)

add_executable(tuple tuple.cc)
add_test(tuple tuple)
