# AGENTS.md - Agent Guidelines for lds-cpp

> Low Discrepancy Sequence Generation in Modern C++ (C++20)

This file provides guidelines for agentic coding agents operating in this repository.

---

## Build System

### CMake (Primary - 3.14+)

```bash
# Build and run all tests
cmake -S test -B build/test
cmake --build build/test
CTEST_OUTPUT_ON_FAILURE=1 cmake --build build/test --target test

# Run executable directly
./build/test/LdsTests

# Build standalone executable
cmake -S standalone -B build/standalone
cmake --build build/standalone
./build/standalone/Lds --help

# Build everything (library, tests, standalone, docs)
cmake -S all -B build
cmake --build build

# Code formatting (view or apply)
cmake --build build/test --target format        # view changes
cmake --build build/test --target fix-format   # apply changes

# Clean build artifacts
cmake --build build/test --target clean    # use CMake clean
```

### xmake (Alternative)

```bash
# Build project
xmake

# Build and run tests
xmake test

# Build standalone executable
xmake build Lds

# Run standalone
xmake run Lds

# Clean build artifacts (DO NOT delete build/ directory directly)
xmake clean
```

### Running Single Tests (Doctest)

```bash
./build/test/LdsTests --test-case="VdCorput"
./build/test/LdsTests --test-case="Halton"
./build/test/LdsTests --test-case="Circle"
./build/test/LdsTests --test-case="Sphere"
# Wildcards supported
./build/test/LdsTests --test-case="*thread*"
```

---

## Code Style Guidelines

### Formatting
- **Tool**: clang-format (Google-based style)
- **Column limit**: 100 characters
- **Indentation**: 4 spaces
- **Braces**: Attach style (opening brace on same line)
- **Namespace indentation**: Indent all namespace contents (`NamespaceIndentation: All`)

Always format before committing:
```bash
cmake --build build/test --target fix-format
```

### Naming Conventions

| Type | Convention | Examples |
|------|------------|----------|
| Classes | PascalCase | `VdCorput`, `Halton`, `Circle`, `Sphere`, `SphereN` |
| Functions | snake_case | `vdc()`, `pop()`, `reseed()`, `simple_interp()` |
| Constants | UPPER_SNAKE_CASE | `TWO_PI`, `MAX_REVERSE_BITS`, `PRIME_TABLE` |
| Member variables | snake_case_ (trailing underscore) | `count`, `base`, `vdc_`, `s_gen_` |
| Private methods | snake_case | (same as public) |
| Parameters | snake_case | `base`, `seed`, `x_value`, `n` |

### Modern C++ Practices (C++20)

- Use `auto` for type inference when type is obvious
- Prefer `std::span` for passing arrays/views
- Use `constexpr` for compile-time constants and functions
- Use `std::array` for fixed-size arrays, `std::vector` for dynamic
- Use `std::unique_ptr` for single ownership
- Prefer range-based for loops: `for (auto x : container)`
- Use `override` keyword when overriding virtual functions
- Use `[[nodiscard]]` attribute for functions that should not ignore return values

### Headers & Includes

- Use `#pragma once` for header guards (no traditional #ifndef)
- Include order: 1) Standard library, 2) Third-party (doctest, fmt), 3) Project headers
- Project headers use angle brackets: `#include <lds/lds.hpp>`
- Headers in `include/lds/*.hpp`, implementations in `source/*.cpp`

### Member Functions

```cpp
// Use trailing return types for clarity
auto pop() -> double;
auto reseed(unsigned long seed) -> void;

// Mark constructors explicit unless implicit conversion intended
explicit VdCorput(unsigned long base);

// Delete move operations if not thread-safe/reasonable
VdCorput(VdCorput&&) noexcept = delete;
VdCorput& operator=(VdCorput&&) noexcept = delete;
```

### Thread Safety

- Use `std::atomic<T>` for thread-safe counters
- Use `std::mutex` + `std::lock_guard` for protecting sections
- Use `std::memory_order_relaxed` for atomic operations when synchronization not needed
- Always test thread safety (see `test_lds.cpp` for examples)

### Error Handling

- Throw `std::invalid_argument` for invalid input arguments
- Throw `std::runtime_error` or similar for runtime errors
- Check preconditions and throw early
- NO error suppression: never catch exceptions silently

### Testing Patterns (Doctest)

```cpp
#include <doctest/doctest.h>

TEST_CASE("ClassName::method_name") {
    // Setup
    auto obj = lds::ClassName(params);

    // Act & Assert
    CHECK_EQ(obj.method(), expected_value);
    CHECK_GE(result, 0.0);
    CHECK_EQ(obj.pop(), doctest::Approx(0.5));
}

// Multiple assertions in one test
TEST_CASE("ClassName::pop multiple") {
    auto obj = lds::ClassName<2>();
    obj.reseed(0);
    CHECK_EQ(res[0], expected1);
    CHECK_EQ(res[1], expected2);
}
```

---

## Project Structure

```
include/lds/    # Public headers (*.hpp)
source/             # Implementation files (*.cpp)
test/               # Test suite (doctest)
standalone/         # Executable demo
all/                # Build all targets together
cmake/              # CMake modules (CPM.cmake, Format.cmake, etc.)
.github/workflows/  # CI/CD workflows
```

---

## Dependencies

The project uses:
- **CPM.cmake** for dependency management
- **doctest** for testing
- **fmt** for string formatting
- **spdlog** for logging (optional)
- **rapidcheck** for property-based testing (optional, see test/CMakeLists.txt)

---

## Important Notes

- **NO `.h` files** - use `.hpp` for C++ headers
- **NO type error suppression** (`as any`, `@ts-ignore`, `@ts-expect-error`) - fix type issues properly
- **NO empty catch blocks** (`catch(e) {}`)
- Always run format target before committing
- Tests must pass before merging
- Thread safety is a requirement for generators
- **DO NOT delete the build/ directory directly** - use CMake targets or `xmake clean` instead
- C++ standard: **C++20 minimum**
- Compiler warnings as errors enabled (`-Werror`, `/WX`)

---

## Additional Tools

### Sanitizers
```bash
cmake -S test -B build/test -DUSE_SANITIZER=Address
cmake -S test -B build/test -DUSE_SANITIZER=Thread
```

### Static Analyzers
```bash
cmake -S test -B build/test -DUSE_STATIC_ANALYZER=clang-tidy
```

### Code Coverage
```bash
cmake -S test -B build/test -DENABLE_TEST_COVERAGE=1
cmake --build build/test
```