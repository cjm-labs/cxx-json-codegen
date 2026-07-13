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
  file(MAKE_DIRECTORY "${generated_dir}")

  foreach(header IN LISTS CJM_GENERATE_HEADERS)
    get_filename_component(header_path "${header}" ABSOLUTE)
    get_filename_component(header_name "${header_path}" NAME_WE)
    set(generated_header "${generated_dir}/${header_name}.cjm.hpp")
  
    add_custom_command(
      OUTPUT "${generated_header}"
      COMMAND cjm generate 
              --input "${header_path}"
              --output "${generated_header}"
      DEPENDS "${header_path}" cjm 
      COMMENT "Generating ${generated_header}"
      VERBATIM
    )

    list(APPEND generated_headers "${generated_header}")
  endforeach()

  target_sources(
    ${CJM_GENERATE_TARGET}
    PRIVATE
      ${generated_headers}
  )
  
  target_include_directories(
    ${CJM_GENERATE_TARGET}
    PRIVATE
      "${generated_dir}"
  )

endfunction()
