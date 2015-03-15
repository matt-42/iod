cmake_minimum_required(VERSION 2.8)
project(Iod)

add_definitions(-std=c++14)

add_executable(iod_generate_symbols iod_generate_symbols.cc)

install(TARGETS iod_generate_symbols
        EXPORT IodTargets
        DESTINATION bin)
