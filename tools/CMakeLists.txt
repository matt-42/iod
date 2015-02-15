cmake_minimum_required(VERSION 2.8)
project(Iod)

find_package(Boost)
add_definitions(-std=c++14)

add_executable(iodc iod_compiler.cc)
target_link_libraries(iodc boost_regex)

add_executable(iod_generate_symbols iod_generate_symbols.cc)
target_link_libraries(iod_generate_symbols boost_regex)

install(TARGETS iodc
        EXPORT IodTargets
        DESTINATION bin)

install(TARGETS iod_generate_symbols
        EXPORT IodTargets
        DESTINATION bin)
