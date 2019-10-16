cmake_minimum_required(VERSION 2.8)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

#add_executable(iod_query_stl iod_query_stl.cc)
#add_test(iod_query_stl iod_query_stl)

#add_executable(iod_query_sql iod_query_sql.cc)
#add_test(iod_query_sql iod_query_sql)

add_executable(json json.cc)
target_compile_options(
    json
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(json json)

add_executable(core core.cc)
target_compile_options(
    core
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(core core)


add_executable(options options.cc)
target_compile_options(
    options
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(options options)

#add_executable(linq linq.cc)
#add_test(linq linq)

add_executable(callable_traits callable_traits.cc)
target_compile_options(
    callable_traits
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(callable_traits callable_traits)

add_executable(apply apply.cc)
target_compile_options(
    apply
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(apply apply)

add_executable(foreach foreach.cc)
target_compile_options(
    foreach
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(foreach foreach)

add_executable(readme readme.cc)
target_compile_options(
    readme
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(readme readme)

add_executable(di di.cc)
target_compile_options(
    di
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(di di)

add_executable(bind_method bind_method.cc)
target_compile_options(
    bind_method
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(bind_method bind_method)

add_executable(tuple tuple.cc)
target_compile_options(
    tuple
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(tuple tuple)

add_executable(parse_command_line parse_command_line.cc)
target_compile_options(
    parse_command_line
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(parse_command_line parse_command_line)

add_executable(deep_merge deep_merge.cc)
target_compile_options(
    deep_merge
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(deep_merge deep_merge)


add_executable(array_view array_view.cc)
target_compile_options(
    array_view
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(array_view array_view)

add_executable(aos_view aos_view.cc)
target_compile_options(
    aos_view
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(aos_view aos_view)


add_executable(variable variable.cc)
target_compile_options(
    variable
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14 -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)
add_test(variable variable)
