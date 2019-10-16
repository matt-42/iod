cmake_minimum_required(VERSION 2.8)
project(Iod)

set(CMAKE_CXX_STANDARD 14)

add_executable(iod_generate_symbols iod_generate_symbols.cc)
target_compile_options(
    iod_generate_symbols 
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-std=c++14>
        $<$<CXX_COMPILER_ID:MSVC>:/permissive->
)

install(TARGETS iod_generate_symbols
        DESTINATION bin)
