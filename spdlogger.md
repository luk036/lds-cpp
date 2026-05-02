# Spdlog Integration Guide for csd-cpp Project

## Overview

This document provides a comprehensive guide on how spdlog is integrated into the csd-cpp project using both CMake and xmake build systems. spdlog (version 1.17.0) is a fast C++ logging library that provides a simple and efficient logging interface.

## Configuration Summary

| Aspect | CMake | xmake |
|--------|-------|-------|
| **Version** | v1.12.0 | Latest (via xmake package manager) |
| **Package Manager** | CPM.cmake | xmake package manager |
| **Install Target** | YES (SPDLOG_INSTALL YES) | N/A |
| **Target Name** | `spdlog::spdlog` | `spdlog` (package name) |
| **Usage Scope** | Library and standalone targets | Multiple targets (Csd, test_csd, csd_cli_spdlog) |

---

## CMake Integration

### 1. Dependency Declaration

Location: `cmake/specific.cmake:11-16`

```cmake
CPMAddPackage(
  NAME spdlog
  GIT_TAG v1.17.0
  GITHUB_REPOSITORY gabime/spdlog
  OPTIONS "SPDLOG_INSTALL YES" # create an installable target
)
```

**Key Points:**
- Uses CPM.cmake for dependency management
- Pins to specific version v1.12.0
- Enables install target for potential downstream consumption
- Creates the `spdlog::spdlog` target

### 2. Library Integration

Location: `CMakeLists.txt:59`

```cmake
target_link_libraries(${PROJECT_NAME} PRIVATE ${SPECIFIC_LIBS})
```

Where `SPECIFIC_LIBS` is defined in `cmake/specific.cmake:18`:
```cmake
set(SPECIFIC_LIBS fmt::fmt spdlog::spdlog)
```

**Integration Pattern:**
- spdlog is linked as a PRIVATE dependency to the Csd library
- Uses the modern CMake target `spdlog::spdlog`
- Private linking means spdlog is only needed for building Csd, not for consumers of Csd

### 3. Standalone Executable Integration

Location: `standalone/CMakeLists.txt:39`

```cmake
target_link_libraries(${PROJECT_NAME} Csd::Csd cxxopts::cxxopts ${SPECIFIC_LIBS})
```

The standalone executable (`CsdStandalone`) inherits spdlog from:
1. Direct inclusion via `${SPECIFIC_LIBS}`
2. Transitive dependency from `Csd::Csd`

### 4. Test Integration

Location: `test/CMakeLists.txt:53`

```cmake
target_link_libraries(${PROJECT_NAME} doctest::doctest Csd::Csd ${SPECIFIC_LIBS})
```

Tests also link spdlog through `${SPECIFIC_LIBS}`.

---

## xmake Integration

### 1. Dependency Declaration

Location: `xmake.lua:5-8`

```lua
add_requires("spdlog", {alias = "spdlog"})
```

**Key Points:**
- Uses xmake's built-in package manager
- Fetches the latest compatible version
- Sets an alias for easier reference in targets

### 2. Library Target Integration

Location: `xmake.lua:38-44`

```lua
target("Csd")
    set_languages("c++14")
    set_kind("static")
    add_includedirs("include", {public = true})
    add_files("source/*.cpp")
    add_packages("fmt", "spdlog")
```

**Integration Pattern:**
- The Csd static library directly includes spdlog
- Public include directories are exported to dependent targets
- spdlog is linked at build time via `add_packages()`

### 3. Test Target Integration

Location: `xmake.lua:47-73`

```lua
target("test_csd")
    set_languages("c++14")
    set_kind("binary")
    add_deps("Csd")
    add_files("test/source/*.cpp")
    add_packages("doctest", "fmt")
    add_packages("spdlog")
    add_tests("default")
```

Tests receive spdlog through:
1. Direct `add_packages("spdlog")`
2. Transitive dependency from `Csd` library

### 4. Benchmark Target Integration

Location: `xmake.lua:88-92`

```lua
target("test_switch")
    set_languages("c++14")
    set_kind("binary")
    add_deps("Csd")
    add_files("bench/BM_switch.cpp")
    add_packages("benchmark", "fmt")
    add_packages("spdlog")
```

### 5. CLI Application with Spdlog

Location: `xmake.lua:99-105`

```lua
target("csd_cli_spdlog")
    set_languages("c++17")
    set_kind("binary")
    add_deps("Csd")
    add_files("standalone/source/main_with_spdlog.cpp")
    add_packages("cxxopts", "fmt", "spdlog")
    add_packages("spdlog")  -- Note: redundant, spdlog listed twice
```

**Observation:** This target includes a redundant `add_packages("spdlog")` call on line 105.

---

## Code Implementation

### 1. Logger Header

Location: `include/csd/logger.hpp`

```cpp
#pragma once
#include <string>

namespace csd {
    void log_with_spdlog(const std::string& message);
}
```

**Design Approach:**
- Simple, wrapper-style interface
- Hides spdlog complexity from library consumers
- Provides a clean API within the `csd` namespace

### 2. Logger Implementation

Location: `source/logger.cpp`

```cpp
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <csd/logger.hpp>

namespace csd {
    void log_with_spdlog(const std::string& message) {
        // Create a file logger
        auto logger = spdlog::basic_logger_mt("file_logger", "csd.log");
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::info);
        
        spdlog::info("Csd message: {}", message);
        spdlog::flush_on(spdlog::level::info);
    }
}
```

**Configuration Details:**
- **Sink Type:** `basic_file_sink_mt` (thread-safe file logging)
- **Log File:** `csd.log`
- **Logger Name:** `"file_logger"`
- **Log Level:** `info`
- **Flush Policy:** Flush on info level messages

### 3. Standalone Application Example

Location: `standalone/source/main_with_spdlog.cpp`

```cpp
#include <csd/csd.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <string>

int main() {
    // Create a file logger
    auto logger = spdlog::basic_logger_mt("file_logger", "csd_example.txt");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);

    // Log some information
    spdlog::info("Starting CSD library example with spdlog");

    // Test some CSD conversions and log the results
    double test_value = 28.5;
    auto csd_result = csd::to_csd(test_value, 2);
    spdlog::info("CSD conversion: {} -> {}", test_value, csd_result);

    // ... more logging ...

    spdlog::info("Example completed successfully!");
    return 0;
}
```

**Usage Pattern:**
- Direct spdlog API usage in application code
- File-based logging to `csd_example.txt`
- Demonstrates integration with csd library functions
- Shows formatted logging with spdlog's `fmt`-style syntax

---

## Build Commands

### CMake

```bash
# Build the library with spdlog
cmake -S. -B build
cmake --build build

# Build standalone application
cmake -S standalone -B build/standalone
cmake --build build/standalone

# Run standalone application
./build/standalone/Csd --help

# Build tests
cmake -S test -B build/test
cmake --build build/test

# Run tests
cd build/test
./CsdTests
```

### xmake

```bash
# Build all targets
xmake

# Build specific target
xmake build csd_cli_spdlog

# Run specific target
xmake run csd_cli_spdlog

# Run tests
xmake run test_csd

# Build in release mode
xmake f -m release -y
xmake
```

---

## Comparison: CMake vs xmake

| Aspect | CMake | xmake |
|--------|-------|-------|
| **Dependency Management** | CPM.cmake (manual version pinning) | Built-in package manager (latest) |
| **Version Control** | Explicit (v1.12.0) | Implicit (latest compatible) |
| **Install Target** | Configured via OPTIONS | N/A (xmake handles differently) |
| **Target Specification** | Modern CMake (`spdlog::spdlog`) | Package name (`spdlog`) |
| **Redundancy Handling** | Single declaration per scope | Manual (found redundant line in xmake.lua) |
| **Build Time** | Slightly slower (CPM downloads) | Generally faster (optimized caching) |

---

## Best Practices Observed

1. **Version Pinning (CMake):** Explicit version specification ensures reproducible builds
2. **Clean API Wrapper:** The `logger.hpp`/`logger.cpp` abstraction hides spdlog complexity
3. **Consistent Integration:** Both build systems link spdlog to library and test targets
4. **Thread-Safe Logging:** Uses `basic_logger_mt` for thread-safe file operations
5. **Proper Flush Policy:** Configures immediate flushing for critical log levels
6. **Separation of Concerns:** Library uses wrapper; applications can use direct API

---

## Potential Improvements

1. **xmake Version Pinning:** Consider specifying a version in xmake for consistency
```lua
add_requires("spdlog v1.12.0", {alias = "spdlog"})
```

2. **Remove Redundancy:** Clean up the duplicate `add_packages("spdlog")` in `xmake.lua:105`

3. **Logger Configuration:** Consider making logger configuration (file path, level) configurable
```cpp
void log_with_spdlog(const std::string& message, const std::string& log_file = "csd.log");
```

4. **Multiple Sinks:** Add console logging alongside file logging for better visibility
```cpp
auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("csd.log");
```

5. **Async Logging:** For performance-critical applications, consider async logging
```cpp
spdlog::init_thread_pool(8192, 1);
auto logger = std::make_shared<spdlog::async_logger>(
    "async_logger", file_sink, spdlog::thread_pool()
);
```

---

## Testing

The project includes test files that may use spdlog for logging:
- `test/source/test_csd.cpp` - Unit tests for CSD functionality
- `test/source/test_stress.cpp` - Stress tests that may benefit from logging
- `test/source/test_rapidcheck.cpp` - Property-based tests

To run tests with logging:
```bash
# CMake
cd build/test
./CsdTests

# xmake
xmake run test_csd
```

---

## Troubleshooting

### Common Issues

1. **Multiple Logger Registration Error**
   - **Symptom:** `logger with name 'file_logger' already exists`
   - **Cause:** Calling `basic_logger_mt` multiple times with the same name
   - **Fix:** Use `basic_logger_st` for single-threaded or check existence first

2. **Log File Not Created**
   - **Symptom:** No `csd.log` file appears
   - **Cause:** Directory doesn't exist or permission issues
   - **Fix:** Ensure write permissions in the working directory

3. **Build Errors on Windows**
   - **Symptom:** Linker errors for spdlog
   - **Cause:** Missing dependencies or incorrect library linking
   - **Fix:** Ensure `SPDLOG_INSTALL YES` option is set in CMake

---

## References

- [spdlog GitHub Repository](https://github.com/gabime/spdlog)
- [spdlog Documentation](https://github.com/gabime/spdlog/wiki)
- [CPM.cmake Documentation](https://github.com/cpm-cmake/CPM.cmake)
- [xmake Package Management](https://xmake.io/#/package/package)
- [Project README](README.md)

---

*Document Version: 1.0*  
*Last Updated: 2026-02-16*  
*Project: csd-cpp v1.1*