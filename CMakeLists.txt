cmake_minimum_required(VERSION 2.8)
project(Iod)

find_package(Boost REQUIRED) # For lexical_cast

install(DIRECTORY iod DESTINATION include
  FILES_MATCHING PATTERN "*.hh")
install(DIRECTORY iod DESTINATION include
  FILES_MATCHING PATTERN "*.hpp")

add_subdirectory(tools)
add_subdirectory(tests)
