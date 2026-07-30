# Specific dependencies for Lds project

# Add spdlog for logging functionality
find_package(spdlog CONFIG QUIET)
if(spdlog_FOUND)
  message(STATUS "Found system spdlog: ${spdlog_DIR}")
else()
  CPMAddPackage(
    NAME spdlog
    GIT_TAG v1.17.0
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS "SPDLOG_INSTALL YES"        # create an installable target
            "SPDLOG_FMT_EXTERNAL YES"   # use external fmt (avoid linker conflict with system fmt)
  )
endif()

# Define specific libs to be linked to library and targets
set(SPECIFIC_LIBS fmt::fmt spdlog::spdlog)
