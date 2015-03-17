IF(BIICODE)

  ADD_BIICODE_TARGETS()

IF(APPLE)
   TARGET_COMPILE_OPTIONS(${BII_BLOCK_TARGET} INTERFACE "-std=c++14 -stdlib=libc++")
ELSEIF (WIN32 OR UNIX)
   TARGET_COMPILE_OPTIONS(${BII_BLOCK_TARGET} INTERFACE "-std=c++14")
ENDIF(APPLE)

ELSE()


cmake_minimum_required(VERSION 2.8)
project(Iod)

find_package(Boost REQUIRED) # For lexical_cast

install(DIRECTORY iod DESTINATION include
  FILES_MATCHING PATTERN "*.hh")
install(DIRECTORY iod DESTINATION include
  FILES_MATCHING PATTERN "*.hpp")

add_subdirectory(tools)
add_subdirectory(tests)

#set(IOD_INCLUDE_DIR "include")
#set(CMAKE_CONFIG_DEST "share/iod")

#include(CMakePackageConfigHelpers)
#configure_package_config_file (                         
#        ${CMAKE_SOURCE_DIR}/IodConfig.cmake.in
#        ${CMAKE_BINARY_DIR}/IodConfig.cmake
#        INSTALL_DESTINATION ${CMAKE_CONFIG_DEST}
#        PATH_VARS IOD_INCLUDE_DIR CMAKE_CONFIG_DEST)

#export(PACKAGE Iod)

#configure_file(IodConfigVersion.cmake.in
#  "${PROJECT_BINARY_DIR}/IodConfigVersion.cmake" @ONLY)

#install(FILES
#  "${PROJECT_BINARY_DIR}/IodConfig.cmake"
#  "${PROJECT_BINARY_DIR}/IodConfigVersion.cmake"
#  DESTINATION share/iod)
 
# Install the export set for use with the install-tree
#install(EXPORT IodTargets DESTINATION ${CMAKE_CONFIG_DEST})

ENDIF() # BIICODE
