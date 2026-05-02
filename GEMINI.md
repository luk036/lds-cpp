# Gemini Code-Aware Context: `lds-cpp`

## Project Overview

This project is a C++ library for generating various low-discrepancy sequences (LDS). These sequences are more uniform than pseudo-random numbers and are useful in applications like numerical integration and computer graphics.

The core of the library is the Van der Corput sequence, which is extended to implement other sequences like the Halton sequence (for n-dimensions), and sequences mapped to circles and spheres.

**Key Technologies:**

*   **Languages:** C++ (C++11/14/17)
*   **Build Systems:** Primarily [CMake](https://cmake.org/), with [XMake](https://xmake.io/) as an alternative.
*   **Dependency Management:** [CPM.cmake](https://github.com/TheLartians/CPM.cmake) is used to fetch and manage dependencies like `fmtlib`.
*   **Testing:** The project uses [doctest](https://github.com/doctest/doctest) for unit testing.
*   **Formatting:** Code style is enforced by `clang-format` for C++ and `cmake-format` for CMake files.
*   **Documentation:** Documentation is generated using [Doxygen](https://www.doxygen.nl/).

**Project Structure:**

The repository is structured as a modular C++ project:

*   `include/lds`: Contains the public headers of the library.
*   `source`: Contains the source code for the library.
*   `test`: Contains the test suite for the library.
*   `standalone`: Contains a sample executable demonstrating the library usage.
*   `all`: A convenience directory to build all targets (library, tests, standalone) at once.
*   `cmake`: Contains helper CMake modules.
*   `.github/workflows`: Contains CI/CD pipelines for building, testing, and style checking on different platforms.

## Building and Running

The project uses a `CMake-out-of-source` build approach. The `all` directory is the most convenient for development as it configures all targets.

### Unified Build (Recommended)

These commands build the library, tests, and the standalone executable.

1.  **Configure:**
    ```bash
    cmake -S all -B build
    ```

2.  **Build:**
    ```bash
    cmake --build build
    ```

### Running Tests

After the unified build, the tests can be run directly:

```bash
./build/test/LdsTests
```

Or, using CTest:

```bash
cd build && ctest
```

### Running the Standalone Executable

The standalone application provides a command-line interface to the library.

```bash
./build/standalone/Lds --help
```

## Development Conventions

*   **Code Style:** C++ code follows the style defined in `.clang-format`. CMake files follow the style in `.cmake-format`. Use the `format` and `fix-format` targets to check and apply formatting.
    ```bash
    # After configuring, from the project root
    cmake --build build --target fix-format
    ```
*   **Dependencies:** External dependencies are added via `CPMAddPackage` in the `CMakeLists.txt` files.
*   **Testing:** Tests are located in `test/source`. The main test runner is `test/source/main.cpp`. New tests should be added in this directory.
*   **Modern CMake:** The project follows modern CMake practices. Avoid using `include_directories` in favor of `target_include_directories`. Dependencies should be linked using `target_link_libraries`.
*   **Modularity:** New applications or tools that use this library should be created in a new directory (like `standalone`) with their own `CMakeLists.txt` that finds and links the `Lds` library.
