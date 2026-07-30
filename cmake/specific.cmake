# Specific dependencies for Lds project

# Add spdlog for logging functionality
find_package(spdlog CONFIG QUIET)
if(spdlog_FOUND)
  message(STATUS "Found system spdlog: ${spdlog_DIR}")
else()
  # Only set SPDLOG_FMT_EXTERNAL when fmt is from system (not CPM).
  # When fmt is also built via CPM, spdlog bundles its own fmt copy.
  set(SPDLOG_OPTS "SPDLOG_INSTALL YES")
  if(FMT_FROM_SYSTEM)
    list(APPEND SPDLOG_OPTS "SPDLOG_FMT_EXTERNAL YES")
  endif()
  CPMAddPackage(
    NAME spdlog
    GIT_TAG v1.17.0
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS ${SPDLOG_OPTS}
  )
endif()

# Define specific libs to be linked to library and targets
set(SPECIFIC_LIBS fmt::fmt spdlog::spdlog)
