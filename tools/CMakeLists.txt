cmake_minimum_required(VERSION 2.8)
project(Iod)

find_package(Boost)
add_definitions(-std=c++14)

add_executable(iodc iod_compiler.cc)
target_link_libraries(iodc boost_regex)

install(TARGETS iodc
        EXPORT IodTargets
        DESTINATION bin)
