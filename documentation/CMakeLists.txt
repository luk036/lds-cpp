cmake_minimum_required(VERSION 3.14...3.22)

project(LdsDocs)

# ---- Doxygen variables ----

set(DOXYGEN_PROJECT_NAME Lds)
set(DOXYGEN_PROJECT_VERSION "1.2.1")
set(DOXYGEN_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

find_package(Doxygen REQUIRED)

add_custom_target(
  GenerateDocs
  COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
  COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}/html"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
