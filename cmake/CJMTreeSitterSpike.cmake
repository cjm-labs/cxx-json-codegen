include_guard(GLOBAL)

include(FetchContent)

enable_language(C)

# Add private Tree-sitter spike targets for frontend research only.
function(cjm_add_tree_sitter_spike)
  # Pinned tag v0.26.0.
  FetchContent_Declare(
    cjm_tree_sitter_runtime_source
    GIT_REPOSITORY https://github.com/tree-sitter/tree-sitter.git 
    GIT_TAG v0.26.11
    SOURCE_SUBDIR cjm-no-upstream-cmake
  )

  # Pinned tag v0.23.4.
  FetchContent_Declare(
    cjm_tree_sitter_cpp_source
    GIT_REPOSITORY https://github.com/tree-sitter/tree-sitter-cpp.git
    GIT_TAG v0.23.4
    SOURCE_SUBDIR cjm-no-upstream-cmake
  )

  FetchContent_MakeAvailable(cjm_tree_sitter_runtime_source cjm_tree_sitter_cpp_source)

  file(GLOB CJM_TREE_SITTER_RUNTIME_SOURCES CONFIGURE_DEPENDS 
    "${cjm_tree_sitter_runtime_source_SOURCE_DIR}/lib/src/*.c"
  )
  list(REMOVE_ITEM CJM_TREE_SITTER_RUNTIME_SOURCES 
    "${cjm_tree_sitter_runtime_source_SOURCE_DIR}/lib/src/lib.c"
  )

  add_library(cjm_tree_sitter_runtime STATIC ${CJM_TREE_SITTER_RUNTIME_SOURCES})
  target_include_directories(cjm_tree_sitter_runtime 
    PUBLIC 
      "${cjm_tree_sitter_runtime_source_SOURCE_DIR}/lib/include"
    PRIVATE 
      "${cjm_tree_sitter_runtime_source_SOURCE_DIR}/lib/src"
      "${cjm_tree_sitter_runtime_source_SOURCE_DIR}/lib/src/wasm"
    )
    target_compile_definitions(cjm_tree_sitter_runtime
      PRIVATE 
        _POSIX_C_SOURCE=200112L
        _DEFAULT_SOURCE
        _BSD_SOURCE
        _DARWIN_C_SOURCE
    )
    set_target_properties(cjm_tree_sitter_runtime PROPERTIES 
      C_STANDARD 11
      C_STANDARD_REQUIRED ON
      POSITION_INDEPENDENT_CODE ON
    )
    set(CJM_TREE_SITTER_CPP_SOURCES 
      "${cjm_tree_sitter_cpp_source_SOURCE_DIR}/src/parser.c"
    )
    if (EXISTS "${cjm_tree_sitter_cpp_source_SOURCE_DIR}/src/scanner.c")
      list(APPEND CJM_TREE_SITTER_CPP_SOURCES 
        "${cjm_tree_sitter_cpp_source_SOURCE_DIR}/src/scanner.c"
      )
    endif()

    add_library(cjm_tree_sitter_cpp_grammar STATIC ${CJM_TREE_SITTER_CPP_SOURCES})
    target_include_directories(cjm_tree_sitter_cpp_grammar 
      PUBLIC 
        "${cjm_tree_sitter_cpp_source_SOURCE_DIR}/bindings/c"
      PRIVATE 
        "${cjm_tree_sitter_cpp_source_SOURCE_DIR}/src"
    )
    target_link_libraries(cjm_tree_sitter_cpp_grammar PUBLIC cjm_tree_sitter_runtime)
    set_target_properties(cjm_tree_sitter_cpp_grammar PROPERTIES 
      C_STANDARD 11
      C_STANDARD_REQUIRED ON
      POSITION_INDEPENDENT_CODE ON
    )
endfunction()

