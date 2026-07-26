function(cjm_generate) 
  set(options)
  set(one_value_args
    TARGET
    GENERATED_TARGET
    GENERATED_HEADERS_VAR
    GENERATED_INCLUDE_DIR_VAR
  )
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

  if (CJM_GENERATE_GENERATED_TARGET AND TARGET ${CJM_GENERATE_GENERATED_TARGET})
    message(FATAL_ERROR "cjm_generate GENERATED_TARGET already exists: ${CJM_GENERATE_GENERATED_TARGET}")
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

  # Expose a stable build target for downstream targets that consume CJM output.
  if (CJM_GENERATE_GENERATED_TARGET)
    add_custom_target(
      ${CJM_GENERATE_GENERATED_TARGET}
      DEPENDS ${generated_headers}
    )
    add_dependencies(
      ${CJM_GENERATE_TARGET}
      ${CJM_GENERATE_GENERATED_TARGET}
    )
  endif()

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

  if (CJM_GENERATE_GENERATED_HEADERS_VAR)
    set(${CJM_GENERATE_GENERATED_HEADERS_VAR} ${generated_headers} PARENT_SCOPE)
  endif()

  if (CJM_GENERATE_GENERATED_INCLUDE_DIR_VAR)
    set(${CJM_GENERATE_GENERATED_INCLUDE_DIR_VAR} "${generated_dir}" PARENT_SCOPE)
  endif()

endfunction()
