cmake_minimum_required(VERSION 3.0.2)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

cmake_policy(SET CMP0048 NEW)
project(iod VERSION 1.0.0 LANGUAGES CXX)
cmake_policy(SET CMP0048 NEW)

find_package(Boost REQUIRED) # For lexical_cast

set(CMAKE_CXX_STANDARD 14)

option(IOD_BUILD_TOOLS "Build command line iod_generate_symbols tool" ON)
option(IOD_BUILD_TESTS "Build ctest tests" ON)

if (IOD_BUILD_TOOLS)
  add_subdirectory(tools)
endif()

if (IOD_BUILD_TESTS)
  add_subdirectory(tests)
endif()

add_library(iod INTERFACE)
target_sources(
    iod 
    INTERFACE 
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/aos_view.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/callable_traits.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/json.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/linq_evaluate.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/parse_command_line.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/stringview.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/type_list_utils.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/apply.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/di.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/json_unicode.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/number_symbol_definitions.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/pow_10.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/symbol.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/tags.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/typelist.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/array_view.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/foreach.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/linq.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/sio.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/symbol_definitions.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/timer.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/utils.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/bind_method.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/grammar.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/linq_aggregators.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/options.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/sio_utils.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/symbols.hh>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod/tuple_utils.hh>)

target_include_directories(
    iod 
    INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/iod>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>    
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_14)

set(IOD_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/iod")

# Manage version comprarison.
#
# iod is header only so it doesn't care about architecture so
# temporarily clear CMAKE_SIZEOF_VOIDP during the write_basic_package_version_file()
# call to make an architecture-independent version file.
set(IOD_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOIDP})
set(CMAKE_SIZEOF_VOIDP "")
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/iod-version.cmake"
    COMPATIBILITY SameMajorVersion
)
set(CMAKE_SIZEOF_VOIDP ${IOD_CMAKE_SIZEOF_VOIDP})

# Generate configuration file which cmake uses for using an installed package.
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/CMake/iod-config.cmake.in
    ${PROJECT_BINARY_DIR}/iod-config.cmake
    INSTALL_DESTINATION ${IOD_CMAKE_CONFIG_DESTINATION}
)

# Set folders for installation artifacts - INCLUDE is omitted because 
# target_include_directories() has the INSTALL_INTERFACE line.
install(
    TARGETS iod
    EXPORT iod-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install iod-targets.cmake defined in share-ring-buffer-config.cmake
install(
    EXPORT iod-targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${IOD_CMAKE_CONFIG_DESTINATION}
)

# Install iod-config.cmake and iod-version.cmake
install(
    FILES 
        "${PROJECT_BINARY_DIR}/iod-config.cmake"
        "${PROJECT_BINARY_DIR}/iod-version.cmake"
	DESTINATION "${IOD_CMAKE_CONFIG_DESTINATION}"
)

# Finally, install the actual header files in this header-only library.
install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/iod"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

