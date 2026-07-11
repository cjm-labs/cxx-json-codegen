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
endfunction()
