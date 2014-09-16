cmake_minimum_required(VERSION 2.8)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_definitions(-std=c++11)

add_executable(iod_query_stl iod_query_stl.cc)
add_test(iod_query_stl iod_query_stl)

add_executable(iod_query_sql iod_query_sql.cc)
add_test(iod_query_sql iod_query_sql)
