cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(LdsTests LANGUAGES CXX)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage("gh:onqtam/doctest#2.4.5")
CPMAddPackage("gh:TheLartians/Format.cmake@1.7.0")

include(../specific.cmake)

if(TEST_INSTALLED_VERSION)
  find_package(Lds REQUIRED)
else()
  CPMAddPackage(NAME Lds SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
add_executable(LdsTests ${sources})
target_link_libraries(LdsTests doctest::doctest Lds::Lds GSL xtensor)
set_target_properties(LdsTests PROPERTIES CXX_STANDARD 17)

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # target_compile_options(Lds PUBLIC -Wall -Wpedantic -Wextra -Werror)
    target_compile_options(Lds PUBLIC -Wall -Wextra -Werror)
  elseif(MSVC)
    target_compile_options(Lds PUBLIC /W4 /WX /wd4819 /wd4146 /wd4127)
    target_compile_definitions(LdsTests PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
  endif()
endif()

# ---- Add LdsTests ----

enable_testing()

# Note: doctest and similar testing frameworks can automatically configure CMake tests. For other
# testing frameworks add the tests target instead: add_test(NAME ldsTests COMMAND LdsTests)

add_test(NAME ldsTests COMMAND LdsTests)

# ---- code coverage ----

if(ENABLE_TEST_COVERAGE)
  target_compile_options(Lds PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(Lds PUBLIC -fprofile-arcs -ftest-coverage)
endif()
