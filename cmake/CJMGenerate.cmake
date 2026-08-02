function(cjm_generate) 
  set(options
    GENERATE_SCHEMAS 
  )
  set(one_value_args
    TARGET
    GENERATED_TARGET
    GENERATED_HEADERS_VAR
    GENERATED_INCLUDE_DIR_VAR
    GENERATED_SCHEMAS_VAR
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
  set(generated_schemas_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/schemas")
  set(generated_headers)
  set(generated_schemas)
  file(MAKE_DIRECTORY "${generated_dir}")
  file(MAKE_DIRECTORY "${generated_schemas_dir}")

  foreach(header IN LISTS CJM_GENERATE_HEADERS)
    get_filename_component(header_path "${header}" ABSOLUTE)
    get_filename_component(header_name "${header_path}" NAME_WE)
    set(generated_header "${generated_dir}/${header_name}.cjm.hpp")
    set(generated_schema "${generated_schemas_dir}/${header_name}.schema.json") 
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

    if (CJM_GENERATE_GENERATE_SCHEMAS)
      add_custom_command(
        OUTPUT "${generated_schema}"
        COMMAND cjm generate-schema
                --input "${header_path}"
                --output "${generated_schema}"
        DEPENDS "${header_path}" cjm
        COMMENT "Generating ${generated_schema}" 
        VERBATIM
      )
      list(APPEND generated_schemas "${generated_schema}") 
    endif()
  endforeach()

  # Expose a stable build target for downstream targets that consume CJM output.
  if (CJM_GENERATE_GENERATED_TARGET)
    add_custom_target(
      ${CJM_GENERATE_GENERATED_TARGET}
      DEPENDS ${generated_headers} ${generated_schemas}
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
  
  target_link_libraries(
    ${CJM_GENERATE_TARGET}
    PRIVATE cjm_contract
  )

  if (CJM_GENERATE_GENERATED_HEADERS_VAR)
    set(${CJM_GENERATE_GENERATED_HEADERS_VAR} ${generated_headers} PARENT_SCOPE)
  endif()

  if (CJM_GENERATE_GENERATED_INCLUDE_DIR_VAR)
    set(${CJM_GENERATE_GENERATED_INCLUDE_DIR_VAR} "${generated_dir}" PARENT_SCOPE)
  endif()
  if (CJM_GENERATE_GENERATED_SCHEMAS_VAR)
    set(${CJM_GENERATE_GENERATED_SCHEMAS_VAR} "${generated_schemas}" PARENT_SCOPE)
  endif()

endfunction()
