add_executable(cjm_basic_example
  main.cpp
)

target_include_directories(
  cjm_basic_example
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (nlohmann_json_FOUND)
  target_link_libraries(
    cjm_basic_example 
    PRIVATE 
      nlohmann_json::nlohmann_json 
  )
endif()

cjm_generate(
  TARGET cjm_basic_example 
  HEADERS user.hpp
)
