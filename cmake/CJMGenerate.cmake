function(cjm_generate) 
  set(options)
  set(one_value_args TARGET)
  set(multi_value_args HEADERS)

  cmake_parse_arguments(
    CJM_GENERATE
    "${options}"
    "${one_value_args}"
    "${multi_value_args}"
    ${ARGN}
  )
  
  if (NOT CJM_GENERATE_TARGET) 
    message(FATAL_ERROR "cjm_generate requires TARGET <target>")
  endif()
  
  if (NOT TARGET ${CJM_GENERATE_TARGET})
    message(FATAL_ERROR "cjm_generate target does not exist: ${CJM_GENERATE_TARGET}")
  endif()
  
  if (NOT CJM_GENERATE_HEADERS) 
    message(FATAL_ERROR "cjm_generate requires HEADERS <header>...")
  endif()

  set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/cjm")
  set(generated_headers)

  foreach(header IN LISTS CJM_GENERATE_HEADERS)
    get_filename_component(header_name "${header}" NAME_WE)
    set(generated_header "${generated_dir}/${header_name}.cjm.hpp")
  
    add_custom_command(
      OUTPUT "${generated_header}"
      COMMAND cjm generate 
              --input "${header}"
              --output "${generated_header}"
      DEPENDS "${header}" cjm 
      COMMENT "Generating ${generated_header}"
      VERBATIM
    )

    list(APPEND generated_headers "${generated_header}")
  endforeach()
endfunction()
