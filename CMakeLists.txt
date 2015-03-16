cmake_minimum_required(VERSION 2.8)
project(Iod)


install(DIRECTORY iod DESTINATION include
  FILES_MATCHING PATTERN "*.hh")
install(DIRECTORY iod DESTINATION include
  FILES_MATCHING PATTERN "*.hpp")

add_subdirectory(tools)

set(IOD_INCLUDE_DIR "include")
set(CMAKE_CONFIG_DEST "share/iod")

include(CMakePackageConfigHelpers)
configure_package_config_file (                         
        ${CMAKE_CURRENT_LIST_DIR}/IodConfig.cmake.in
        ${CMAKE_BINARY_DIR}/IodConfig.cmake
        INSTALL_DESTINATION ${CMAKE_CONFIG_DEST}
        PATH_VARS IOD_INCLUDE_DIR CMAKE_CONFIG_DEST)

export(PACKAGE Iod)

configure_file(IodConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/IodConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_SOURCE_DIR}/IodUse.cmake"
  "${PROJECT_BINARY_DIR}/IodConfig.cmake"
  "${PROJECT_BINARY_DIR}/IodConfigVersion.cmake"
  DESTINATION share/iod)
 
# Install the export set for use with the install-tree
install(EXPORT IodTargets DESTINATION ${CMAKE_CONFIG_DEST})
