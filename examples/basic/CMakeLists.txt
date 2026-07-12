add_executable(cjm_basic_example
  main.cpp
)

target_include_directories(
  cjm_basic_example
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

cjm_generate(
  TARGET cjm_basic_example 
  HEADERS user.hpp
)
