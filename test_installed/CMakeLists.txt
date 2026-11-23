cmake_minimum_required(VERSION 3.14)
project(LdsInstalledTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Lds REQUIRED)

add_executable(lds_installed_test main.cpp)
target_link_libraries(lds_installed_test PRIVATE Lds::Lds)

enable_testing()
add_test(NAME lds_installed_test COMMAND lds_installed_test)
