include_guard(GLOBAL)

include(FetchContent)

# Provide the pinned simdjson dependency for selected backend targets.
function(cjm_add_simdjson_dependency)
  # 1. Reuse a target already provided by the parent project.
  if (TARGET simdjson::simdjson)
    return()
  endif()

  # 2. Prefer the exact installed package used by this spile.
  find_package(simdjson 4.6.4 EXACT CONFIG QUIET)

  # 3. Fetch the pinned releases when no installed target is available.
  if (NOT TARGET simdjson::simdjson)
    message(STATUS "CJM: fetching simdjson v4.6.4")
    FetchContent_Declare(
      simdjson
      URL https://github.com/simdjson/simdjson/archive/refs/tags/v4.6.4.tar.gz
      URL_HASH SHA256=b091107844fe928158c5c2265c20360fff312889ddf7ebc4528a0f0f8f2ff9cd
      DOWNLOAD_NO_PROGRESS FALSE
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(simdjson)
  endif()

  # 4. Verify the dependency exposes its official consumer target.
  if (NOT TARGET simdjson::simdjson)
    message(
      FATAL_ERROR
      "CJM_ENABLE_SIMDJSON=ON requires the simdjson::simdjson CMake target"
    )
  endif()
endfunction()
