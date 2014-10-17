cmake_minimum_required(VERSION 2.8)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_definitions(-std=c++14 -g)

#add_executable(iod_query_stl iod_query_stl.cc)
#add_test(iod_query_stl iod_query_stl)

#add_executable(iod_query_sql iod_query_sql.cc)
#add_test(iod_query_sql iod_query_sql)

add_executable(json json.cc)
add_test(json json)

add_executable(core core.cc)
add_test(core core)

add_executable(linq linq.cc)
add_test(linq linq)

add_executable(callable_traits callable_traits.cc)
add_test(callable_traits callable_traits)
