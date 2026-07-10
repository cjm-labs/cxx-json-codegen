cmake_minimum_required(VERSION 3.20)

project(CJM VERSION 0.1.0 LANGUAGES CXX)

option(CJM_BUILD_TESTS "Build CJM tests" ON)

add_library(cjm_metadata INTERFACE)

target_include_directories(cjm_metadata 
  INTERFACE 
      ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(cjm_metadata INTERFACE cxx_std_17)

if (CJM_BUILD_TESTS)
  enable_testing()

  add_executable(cjm_metadata_tests
    tests/metadata/model_tests.cpp
  )

  target_link_libraries(cjm_metadata_tests PRIVATE cjm_metadata)

  add_test(NAME cjm_metadata_tests COMMAND cjm_metadata_tests)

endif()

