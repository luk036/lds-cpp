cmake_minimum_required(VERSION 3.14...3.22)

project(LdsTests LANGUAGES CXX)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools and dependencies ----

include(../cmake/CPM.cmake)
include(../cmake/specific.cmake)
include(../cmake/tools.cmake)

# Try system-installed doctest first (Ubuntu: doctest-dev, macOS: brew install doctest, Termux:
# doctest)
find_package(doctest CONFIG QUIET)

if(NOT doctest_FOUND)
  CPMAddPackage("gh:doctest/doctest@2.5.2")
endif()
CPMAddPackage("gh:TheLartians/Format.cmake@1.7.3")

if(TEST_INSTALLED_VERSION)
  find_package(Lds REQUIRED)
else()
  CPMAddPackage(NAME Lds SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} doctest::doctest Lds::Lds ${SPECIFIC_LIBS})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(Lds PUBLIC -Wall -Wpedantic -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(Lds PUBLIC /utf-8 /W4 /WX)
    # target_compile_definitions(${PROJECT_NAME} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add LdsTests ----

enable_testing()

# Add a single test that runs all tests (both doctest and RapidCheck)
add_test(NAME LdsAllTests COMMAND ${PROJECT_NAME})

# ---- code coverage ----

if(ENABLE_TEST_COVERAGE)
  target_compile_options(Lds PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(Lds PUBLIC -fprofile-arcs -ftest-coverage)
endif()
