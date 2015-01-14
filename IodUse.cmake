set(IOD_TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/iod_tmp_dir")
file(MAKE_DIRECTORY ${IOD_TMP_DIR})
set(SYMBOL_INCLUDE_FILE ${IOD_TMP_DIR}/symbol_include.hh)
file(WRITE ${SYMBOL_INCLUDE_FILE} "#include <iod/symbol.hh>\n")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14")

function(add_iod_executable arg1)

  set(sources "")
  foreach(iod_compile_rules ${ARGN})
    set(input_file ${CMAKE_CURRENT_SOURCE_DIR}/${iod_compile_rules})
    set(inputp_file ${IOD_TMP_DIR}/${iod_compile_rules}.i)
    set(output_file ${IOD_TMP_DIR}/${iod_compile_rules})
    set(sources ${sources} ${output_file})

    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    set(INCLUDE_FLAGS)
    foreach(dir ${dirs})
      set(INCLUDE_FLAGS ${INCLUDE_FLAGS} -I ${dir}  )
    endforeach()

    add_custom_command(OUTPUT ${inputp_file}
                       COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_DEFINES}  ${CMAKE_CXX_FLAGS} ${INCLUDE_FLAGS} -E ${SYMBOL_INCLUDE_FILE} ${input_file} > ${inputp_file}
                       IMPLICIT_DEPENDS CXX ${input_file})

    add_custom_command(OUTPUT ${output_file}
                       COMMAND iodc ${inputp_file} ${output_file}
                       DEPENDS ${inputp_file})

#    add_custom_command(OUTPUT ${output_file}
#                       COMMAND iodc ${input_file}.i ${output_file}
#                       #DEPENDS ${input_file}.i
#                       IMPLICIT_DEPENDS  ${input_file}.i
#                       )

  endforeach(iod_compile_rules)
  #add_executable(${arg1}_just_for_preprocessor_rules ${ARGN})
  add_executable(${arg1} ${sources})

endfunction(add_iod_executable)
