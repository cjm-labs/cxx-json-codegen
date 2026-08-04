cmake_minimum_required(VERSION 3.20)

project(CJM VERSION 0.3.0 LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CJMFetchContent.cmake)

find_package(nlohmann_json QUIET)

if (NOT nlohmann_json_FOUND)
  FetchContent_Declare(
    nlohmann_json 
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
    DOWNLOAD_NO_PROGRESS FALSE
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CJM_BUILD_TESTS "Build CJM tests" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CJMGenerate.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CJMTreeSitter.cmake)
cjm_add_tree_sitter_frontend_dependencies()

add_library(cjm_tree_sitter_adapter
  src/frontends/cxx/tree_sitter/adapter.cpp
)
target_compile_features(cjm_tree_sitter_adapter PUBLIC cxx_std_17)
target_include_directories(cjm_tree_sitter_adapter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cjm_tree_sitter_adapter PRIVATE cjm_tree_sitter_cpp_grammar)

if (CJM_BUILD_TESTS)
  add_executable(cjm_tree_sitter_adapter_tests
    tests/frontends/cxx/tree_sitter/adapter_tests.cpp
  )
  target_compile_features(cjm_tree_sitter_adapter_tests PRIVATE cxx_std_17)
  target_link_libraries(cjm_tree_sitter_adapter_tests PRIVATE cjm_tree_sitter_adapter)
  add_test(NAME cjm_tree_sitter_adapter_tests COMMAND cjm_tree_sitter_adapter_tests)
  set_tests_properties(
    cjm_tree_sitter_adapter_tests
    PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  add_executable(cjm_tree_sitter_dump
    tools/tree_sitter_dump/main.cpp
  )
  target_compile_features(cjm_tree_sitter_dump PRIVATE cxx_std_17)
  target_link_libraries(cjm_tree_sitter_dump PRIVATE cjm_tree_sitter_cpp_grammar)
endif()

# metadata library 
add_library(cjm_contract INTERFACE)
target_include_directories(cjm_contract INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_contract INTERFACE cxx_std_17)

add_library(cjm_contract_generator src/backends/contract/contract_generator.cpp)
target_link_libraries(cjm_contract_generator PUBLIC cjm_metadata cjm_contract)
target_include_directories(cjm_contract_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_contract_generator PUBLIC cxx_std_17)

add_library(cjm_schema_generator src/backends/schema/schema_generator.cpp)
target_link_libraries(cjm_schema_generator PUBLIC cjm_metadata)
target_include_directories(cjm_schema_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_schema_generator PUBLIC cxx_std_17)

# metadata library 
add_library(cjm_metadata INTERFACE)
target_include_directories(cjm_metadata INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_metadata INTERFACE cxx_std_17)

# generator library
add_library(cjm_generator src/backends/nlohmann/cpp_generator.cpp)
target_link_libraries(cjm_generator PUBLIC cjm_metadata)
target_include_directories(cjm_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_generator PUBLIC cxx_std_17)

# parser library
add_library(cjm_parser src/frontends/cxx/parser/parser.cpp)
target_include_directories(cjm_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cjm_parser PRIVATE cjm_tree_sitter_adapter)
target_compile_features(cjm_parser PUBLIC cxx_std_17)

# semantic library 
add_library(cjm_semantic src/frontends/cxx/semantic/analysis.cpp)
target_include_directories(cjm_semantic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cjm_semantic PUBLIC cjm_metadata cjm_parser)
target_compile_features(cjm_semantic PUBLIC cxx_std_17)

# cjm main app
add_executable(cjm tools/cjm/main.cpp)
target_link_libraries(cjm PRIVATE cjm_generator cjm_schema_generator cjm_semantic)

if (CJM_BUILD_TESTS)
  enable_testing()

  # METADATA tests below...
  add_executable(cjm_metadata_tests
    tests/core/ir/model_tests.cpp
  )
  target_link_libraries(cjm_metadata_tests PRIVATE cjm_metadata)
  add_test(NAME cjm_metadata_tests COMMAND cjm_metadata_tests)


  # GENERATOR tests below...
  add_executable(cjm_generator_tests tests/backends/nlohmann/cpp_generator_tests.cpp)
  target_link_libraries(cjm_generator_tests PRIVATE cjm_generator)
  add_test(NAME cjm_generator_tests COMMAND cjm_generator_tests)
  set_tests_properties(cjm_generator_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
  add_executable(cjm_contract_generator_tests
    tests/backends/contract/contract_generator_tests.cpp
  )
  target_link_libraries(cjm_contract_generator_tests PRIVATE cjm_contract_generator)
  add_test(NAME cjm_contract_generator_tests COMMAND cjm_contract_generator_tests)
  set_tests_properties(cjm_contract_generator_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_executable(cjm_contract_generated_compile_tests tests/backends/contract/contract_generated_compile_tests.cpp)
  target_compile_features(cjm_contract_generated_compile_tests PRIVATE cxx_std_17)
  target_include_directories(cjm_contract_generated_compile_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(cjm_contract_generated_compile_tests PRIVATE cjm_contract)
  add_test(NAME cjm_contract_generated_compile_tests COMMAND cjm_contract_generated_compile_tests)

  add_executable(cjm_schema_generator_tests
    tests/backends/schema/schema_generator_tests.cpp
  )
  target_link_libraries(cjm_schema_generator_tests PRIVATE cjm_schema_generator)
  add_test(NAME cjm_schema_generator_tests COMMAND cjm_schema_generator_tests)
  set_tests_properties(cjm_schema_generator_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


  # PIPELINE GOLDEN tests below...
  add_executable(cjm_pipeline_golden_tests tests/backends/nlohmann/pipeline_golden_tests.cpp)
  target_link_libraries(cjm_pipeline_golden_tests PRIVATE cjm_generator cjm_parser cjm_semantic)
  add_test(NAME cjm_pipeline_golden_tests COMMAND cjm_pipeline_golden_tests)
  set_tests_properties(cjm_pipeline_golden_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  # CLI tests below...
  add_test(NAME cjm_cli_help COMMAND cjm --help)
  add_test(NAME cjm_cli_generate
    COMMAND cjm generate
    --input ${CMAKE_CURRENT_SOURCE_DIR}/examples/basic/user.hpp 
    --output ${CMAKE_CURRENT_BINARY_DIR}/user.cjm.hpp
  )
  set_tests_properties(
    cjm_cli_generate
    PROPERTIES 
      PASS_REGULAR_EXPRESSION "cjm: generated"
  )
 
  add_test(NAME cjm_cli_unknown_command 
    COMMAND cjm unknown 
  )
  set_tests_properties(
    cjm_cli_unknown_command
    PROPERTIES 
      WILL_FAIL TRUE 
  )

  add_test(NAME cjm_cli_generate_missing_args 
    COMMAND cjm generate 
  )
  set_tests_properties(
    cjm_cli_generate_missing_args 
    PROPERTIES 
      WILL_FAIL TRUE 
  )

  add_subdirectory(examples/basic)

  add_test(
    NAME cjm_basic_example 
    COMMAND cjm_basic_example 
  )

  add_test(
    NAME cjm_basic_downstream_consumer
    COMMAND cjm_basic_downstream_consumer
  )

  add_test(
    NAME cjm_basic_example_generated_header
    COMMAND ${CMAKE_COMMAND} -E compare_files
            ${CMAKE_CURRENT_BINARY_DIR}/examples/basic/generated/cjm/user.cjm.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/golden/basic_pipeline.expected.cjm.hpp
  )
  # parser tests below...
  add_executable(cjm_parser_tests tests/frontends/cxx/parser/syntax_tests.cpp)
  target_link_libraries(cjm_parser_tests PRIVATE cjm_parser)
  add_test(NAME cjm_parser_tests COMMAND cjm_parser_tests)
  set_tests_properties(cjm_parser_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
  add_executable(cjm_parser_file_tests tests/frontends/cxx/parser/parser_tests.cpp)
  target_link_libraries(cjm_parser_file_tests PRIVATE cjm_parser)
  add_test(NAME cjm_parser_file_tests COMMAND cjm_parser_file_tests)
  set_tests_properties(cjm_parser_file_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_executable(cjm_tree_sitter_smoke_tests
    tests/frontends/cxx/tree_sitter/build_smoke_tests.cpp
  )
  target_compile_features(cjm_tree_sitter_smoke_tests PRIVATE cxx_std_17)
  target_link_libraries(cjm_tree_sitter_smoke_tests PRIVATE cjm_tree_sitter_cpp_grammar)
  add_test(NAME cjm_tree_sitter_smoke_tests COMMAND cjm_tree_sitter_smoke_tests)

  # semantic tests below...
  add_executable(cjm_semantic_tests tests/frontends/cxx/semantic/analysis_tests.cpp)
  target_link_libraries(cjm_semantic_tests PRIVATE cjm_semantic)
  add_test(NAME cjm_semantic_tests COMMAND cjm_semantic_tests)
  set_tests_properties(cjm_semantic_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  # practical generated compile tests below...
  add_executable(cjm_practical_generated_compile_tests tests/backends/nlohmann/practical_generated_compile_tests.cpp)
  target_compile_features(cjm_practical_generated_compile_tests PRIVATE cxx_std_17)
  target_include_directories(cjm_practical_generated_compile_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(cjm_practical_generated_compile_tests PRIVATE nlohmann_json::nlohmann_json cjm_contract)
  add_test(NAME cjm_practical_generated_compile_tests COMMAND cjm_practical_generated_compile_tests)

  add_executable(cjm_map_generated_compile_tests tests/backends/nlohmann/map_generated_compile_tests.cpp)
  target_compile_features(cjm_map_generated_compile_tests PRIVATE cxx_std_17)
  target_include_directories(cjm_map_generated_compile_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(cjm_map_generated_compile_tests PRIVATE nlohmann_json::nlohmann_json cjm_contract)
  add_test(NAME cjm_map_generated_compile_tests COMMAND cjm_map_generated_compile_tests)

  add_executable(cjm_fixed_width_generated_compile_tests tests/backends/nlohmann/fixed_width_generated_compile_tests.cpp)
  target_compile_features(cjm_fixed_width_generated_compile_tests PRIVATE cxx_std_17)
  target_link_libraries(cjm_fixed_width_generated_compile_tests PRIVATE nlohmann_json::nlohmann_json)
  add_test(NAME cjm_fixed_width_generated_compile_tests COMMAND cjm_fixed_width_generated_compile_tests)

  add_test(NAME cjm_cli_generate_multiple_inputs
    COMMAND cjm generate
    --input ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/cli_multi/address.hpp
    --input ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/cli_multi/user.hpp
    --output ${CMAKE_CURRENT_BINARY_DIR}/cli_multi.cjm.hpp 
  )
  set_tests_properties(
    cjm_cli_generate_multiple_inputs
    PROPERTIES 
      PASS_REGULAR_EXPRESSION "cjm: generated"
  )

  add_test(NAME cjm_cli_generate_multiple_inputs_golden 
    COMMAND ${CMAKE_COMMAND} -E compare_files 
    ${CMAKE_CURRENT_BINARY_DIR}/cli_multi.cjm.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/golden/cli_multi.expected.cjm.hpp 
  )
  set_tests_properties(
    cjm_cli_generate_multiple_inputs_golden
    PROPERTIES
      DEPENDS cjm_cli_generate_multiple_inputs
  )

  add_test(NAME cjm_cli_generate_grouped_inputs
    COMMAND cjm generate
    --input
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/cli_multi/address.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/cli_multi/user.hpp
    --output ${CMAKE_CURRENT_BINARY_DIR}/cli_multi_grouped.cjm.hpp
  )
  set_tests_properties(
    cjm_cli_generate_grouped_inputs
    PROPERTIES
      PASS_REGULAR_EXPRESSION "cjm: generated"
  )

  add_test(NAME cjm_cli_generate_grouped_inputs_golden
    COMMAND ${CMAKE_COMMAND} -E compare_files
    ${CMAKE_CURRENT_BINARY_DIR}/cli_multi_grouped.cjm.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/golden/cli_multi.expected.cjm.hpp
  )
  set_tests_properties(
    cjm_cli_generate_grouped_inputs_golden
    PROPERTIES
      DEPENDS cjm_cli_generate_grouped_inputs
  )

  add_test(NAME cjm_cli_generate_v03_practical
    COMMAND cjm generate
    --input ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/v03_practical_models.hpp
    --output ${CMAKE_CURRENT_BINARY_DIR}/v03_practical.cjm.hpp
  )
  set_tests_properties(
    cjm_cli_generate_v03_practical
    PROPERTIES
      PASS_REGULAR_EXPRESSION "cjm: generated"
  )

  add_test(NAME cjm_cli_generate_v03_practical_golden
    COMMAND ${CMAKE_COMMAND} -E compare_files
    ${CMAKE_CURRENT_BINARY_DIR}/v03_practical.cjm.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/golden/v03_practical.expected.cjm.hpp
  )
  set_tests_properties(
    cjm_cli_generate_v03_practical_golden
    PROPERTIES
      DEPENDS cjm_cli_generate_v03_practical
  )

  add_executable(cjm_model_contract_tests tests/core/model_contract_tests.cpp)
  target_link_libraries(cjm_model_contract_tests PRIVATE cjm_contract)
  add_test(NAME cjm_model_contract_tests COMMAND cjm_model_contract_tests)

  add_test(NAME cjm_cli_generate_schema
    COMMAND cjm generate-schema
    --input ${CMAKE_CURRENT_SOURCE_DIR}/examples/basic/user.hpp
    --output ${CMAKE_CURRENT_BINARY_DIR}/user.schema.json
  )
  set_tests_properties(
    cjm_cli_generate_schema
    PROPERTIES
      PASS_REGULAR_EXPRESSION "cjm: generated"
  )

  add_test(
    NAME cjm_basic_example_generated_schemas
    COMMAND ${CMAKE_COMMAND} -E compare_files
            ${CMAKE_CURRENT_BINARY_DIR}/examples/basic/generated/schemas/user.schema.json
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/golden/basic_user.expected.schema.json
  )
endif()
