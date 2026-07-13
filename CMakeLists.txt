cmake_minimum_required(VERSION 3.20)

project(CJM VERSION 0.1.0 LANGUAGES CXX)

find_package(nlohmann_json QUIET)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CJM_BUILD_TESTS "Build CJM tests" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CJMGenerate.cmake)

# metadata library 
add_library(cjm_metadata INTERFACE)
target_include_directories(cjm_metadata INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_metadata INTERFACE cxx_std_17)

# generator library
add_library(cjm_generator src/generator/cpp_generator.cpp)
target_link_libraries(cjm_generator PUBLIC cjm_metadata)
target_include_directories(cjm_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_generator PUBLIC cxx_std_17)

# parser library
add_library(cjm_parser src/parser/parser.cpp)
target_include_directories(cjm_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(cjm_parser PUBLIC cxx_std_17)

# semantic library 
add_library(cjm_semantic src/semantic/analysis.cpp)
target_include_directories(cjm_semantic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cjm_semantic PUBLIC cjm_metadata cjm_parser)
target_compile_features(cjm_semantic PUBLIC cxx_std_17)

# cjm main app
add_executable(cjm tools/cjm/main.cpp)
target_link_libraries(cjm PRIVATE cjm_generator cjm_semantic)

if (CJM_BUILD_TESTS)
  enable_testing()

  # METADATA tests below...
  add_executable(cjm_metadata_tests
    tests/metadata/model_tests.cpp
  )
  target_link_libraries(cjm_metadata_tests PRIVATE cjm_metadata)
  add_test(NAME cjm_metadata_tests COMMAND cjm_metadata_tests)


  # GENERATOR tests below...
  add_executable(cjm_generator_tests tests/generator/cpp_generator_tests.cpp)
  target_link_libraries(cjm_generator_tests PRIVATE cjm_generator)
  add_test(NAME cjm_generator_tests COMMAND cjm_generator_tests)
  set_tests_properties(cjm_generator_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

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

  # parser tests below...
  add_executable(cjm_parser_tests tests/parser/syntax_tests.cpp)
  target_link_libraries(cjm_parser_tests PRIVATE cjm_parser)
  add_test(NAME cjm_parser_tests COMMAND cjm_parser_tests)
  set_tests_properties(cjm_parser_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  
  add_executable(cjm_parser_file_tests tests/parser/parser_tests.cpp)
  target_link_libraries(cjm_parser_file_tests PRIVATE cjm_parser)
  add_test(NAME cjm_parser_file_tests COMMAND cjm_parser_file_tests)
  set_tests_properties(cjm_parser_file_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  # semantic tests below...
  add_executable(cjm_semantic_tests tests/semantic/analysis_tests.cpp)
  target_link_libraries(cjm_semantic_tests PRIVATE cjm_semantic)
  add_test(NAME cjm_semantic_tests COMMAND cjm_semantic_tests)

endif()


