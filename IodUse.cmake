set(IOD_TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/iod_tmp_dir")
file(MAKE_DIRECTORY ${IOD_TMP_DIR})

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14")

function(add_iod_executable arg1)

  set(sources "")
  foreach(iod_compile_rules ${ARGN})
    set(input_file ${CMAKE_CURRENT_SOURCE_DIR}/${iod_compile_rules})
    set(output_file ${IOD_TMP_DIR}/${iod_compile_rules})
    set(sources ${sources} ${output_file})

    add_custom_command(OUTPUT ${IOD_TMP_DIR}/${iod_compile_rules}
                       COMMAND iodc ${input_file} ${output_file}
                       DEPENDS ${input_file})

  endforeach(iod_compile_rules)
  add_executable(${arg1} ${sources})

endfunction(add_iod_executable)
