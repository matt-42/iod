cmake_minimum_required(VERSION 2.8)
project(Iod)

set(CMAKE_CXX_STANDARD 14)

add_executable(iod_generate_symbols iod_generate_symbols.cc)

install(TARGETS iod_generate_symbols
        DESTINATION bin)
