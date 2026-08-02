add_executable(cjm_basic_example
  main.cpp
)

target_include_directories(
  cjm_basic_example
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  cjm_basic_example 
  PRIVATE 
    nlohmann_json::nlohmann_json 
)

cjm_generate(
  TARGET cjm_basic_example 
  HEADERS user.hpp
  GENERATED_TARGET cjm_basic_example_generated
  GENERATED_HEADERS_VAR cjm_basic_generated_headers
  GENERATED_INCLUDE_DIR_VAR cjm_basic_generated_include_dir

  GENERATE_SCHEMAS
  GENERATED_SCHEMAS_VAR cjm_basic_generated_schemas
)

if (NOT cjm_basic_generated_headers)
  message(FATAL_ERROR "cjm_generate did not expose generated headers")
endif()

if (NOT cjm_basic_generated_include_dir)
  message(FATAL_ERROR "cjm_generate did not expose generated include directory")
endif()

if (NOT cjm_basic_generated_schemas)
  message(FATAL_ERROR "cjm_generate did not expose generated schemas")
endif()

add_executable(cjm_basic_downstream_consumer
  downstream_consumer.cpp
)

add_dependencies(
  cjm_basic_downstream_consumer
  cjm_basic_example_generated
)

target_sources(
  cjm_basic_downstream_consumer
  PRIVATE
    ${cjm_basic_generated_headers}
)

target_include_directories(
  cjm_basic_downstream_consumer
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${cjm_basic_generated_include_dir}
)

target_link_libraries(
  cjm_basic_downstream_consumer
  PRIVATE
    nlohmann_json::nlohmann_json
    cjm_contract
)
