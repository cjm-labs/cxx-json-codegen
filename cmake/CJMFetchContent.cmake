include_guard(GLOBAL)

include(FetchContent)

option(CJM_FETCHCONTENT_SHOW_PROGRESS "Show dependency download and clone progress" ON)
option(
  CJM_FETCHCONTENT_UPDATES_DISCONNECTED
  "Avoid remote update checks for already populated pinned dependencies"
  ON
)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(_CJM_DEFAULT_FETCHCONTENT_BASE_DIR "${PROJECT_SOURCE_DIR}/.cache/fetchcontent")
else()
  set(_CJM_DEFAULT_FETCHCONTENT_BASE_DIR "")
endif()

set(CJM_FETCHCONTENT_BASE_DIR
  "${_CJM_DEFAULT_FETCHCONTENT_BASE_DIR}"
  CACHE PATH
  "Optional shared FetchContent base directory for CJM dependencies"
)

if (CJM_FETCHCONTENT_BASE_DIR)
  set(FETCHCONTENT_BASE_DIR
    "${CJM_FETCHCONTENT_BASE_DIR}"
    CACHE PATH
    "FetchContent base directory"
    FORCE
  )
endif()

if (CJM_FETCHCONTENT_SHOW_PROGRESS)
  set(FETCHCONTENT_QUIET OFF)
endif()

if (CJM_FETCHCONTENT_UPDATES_DISCONNECTED)
  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()
