include_guard(GLOBAL)

include(FetchContent)

# Provide the pinned Catch2 dependency for CJM test targets.
function(cjm_add_catch2_dependency)
  if (TARGET Catch2::Catch2WithMain)
    return()
  endif()

  message(STATUS "CJM: fetching Catch2 v3.15.3")
  FetchContent_Declare(
    Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.15.3.tar.gz
    URL_HASH SHA256=b0299ae552918220a7a6e21e7de5b714777f4e8c883fb70c4bb23fe01df8c6e3
    DOWNLOAD_NO_PROGRESS FALSE
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(Catch2)

  if (NOT TARGET Catch2::Catch2WithMain)
    message(
      FATAL_ERROR
      "CJM_BUILD_TESTS=ON requires the Catch2::Catch2WithMain Cmake target"
    )
  endif()
endfunction()

