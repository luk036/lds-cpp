# AGENTS.md - Agent Guidelines for lds-cpp

> Low Discrepancy Sequence Generation in Modern C++ (C++20)

This file provides guidelines for agentic coding agents operating in this repository.

---

## Build System

### CMake (Primary - 3.14+)

There is a single top-level `CMakeLists.txt`; `test/` and `standalone/` have no build files of their own.

```bash
# Configure once, then build (library + tests + standalone)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run all tests
ctest --test-dir build --output-on-failure

# Run the test executable directly
./build/LdsTests

# Standalone demo (its main() is currently an empty stub)
./build/Lds

# Code formatting (view or apply)
cmake --build build --target format        # view changes
cmake --build build --target fix-format    # apply changes

# Optional targets
cmake -S . -B build -DLDS_BUILD_BENCHMARKS=ON   # Bench_<name> targets
cmake -S . -B build -DLDS_BUILD_DOCS=ON         # GenerateDocs target
cmake -S . -B build -DLDS_ENABLE_CLANG_TIDY=ON  # clang-tidy target
```

### xmake (Alternative)

```bash
xmake                 # build default targets (Lds, test_lds, verify_basic)
xmake test            # build and run the doctest suite
xmake build Lds       # build the static library only
xmake clean           # clean build artifacts (do not delete build/ by hand)
```

### Running Single Tests (Doctest)

```bash
./build/LdsTests --test-case="VdCorput"
./build/LdsTests --test-case="Halton"
./build/LdsTests --test-case="Circle"
./build/LdsTests --test-case="Sphere"
# Wildcards supported
./build/LdsTests --test-case="*thread*"
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

The template generators (`VdCorput<Base>`, `Halton<Base0, Base1>`, etc.) are **not thread-safe internally**. Their mutating methods (`pop()`, `reseed()`, `skip()`) are `constexpr` and use plain `unsigned long` counters. To use these generators across threads, wrap them with `std::mutex` externally.

The runtime polymorphic classes (`Sphere3`, `SphereN`, `SphereWrapper`) use `std::mutex` internally for thread safety.

See [Design Decisions](#design-decisions) for the rationale.

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
include/lds/        # Public headers (*.hpp)
source/             # Implementation files (*.cpp)
test/source/        # Test suite (doctest)
standalone/source/  # Executable demo
bench/              # nanobench micro-benchmarks (opt-in)
examples/           # Verification programs (xmake only)
documentation/      # Doxyfile + pages
.github/workflows/  # CI/CD workflows
```

---

## Dependencies

The project uses:
- **CPM.cmake** for dependency management
- **doctest** for testing
- **cxxopts** for the standalone CLI (CMake only)
- **nanobench** for benchmarks (opt-in, CMake downloads the header)
- **fmt** for the `examples/` programs (xmake only)
- **Doxygen** for documentation (opt-in)

---

## Important Notes

- **NO `.h` files** - use `.hpp` for C++ headers
- **NO type error suppression** (`as any`, `@ts-ignore`, `@ts-expect-error`) - fix type issues properly
- **NO empty catch blocks** (`catch(e) {}`)
- Always run format target before committing
- Tests must pass before merging
- **DO NOT delete the build/ directory directly** - use CMake targets or `xmake clean` instead
- C++ standard: **C++20 minimum**
- Compiler warnings as errors enabled (`-Werror`, `/WX`)

---

## Additional Tools

### Static Analyzers
```bash
# clang-tidy over the public headers (uses the checks in .clang-tidy)
cmake -S . -B build -DLDS_ENABLE_CLANG_TIDY=ON
cmake --build build --target clang-tidy
```

### Code Coverage
```bash
cmake -S . -B build -DLDS_ENABLE_COVERAGE=ON
cmake --build build --target coverage   # requires gcovr
```

---

## Design Decisions

### constexpr vs Thread Safety

This project prioritizes `constexpr` on mutating generator methods (`pop()`, `peek()`, `reseed()`, `skip()`, `get_index()`), enabling compile-time sequence evaluation. This is incompatible with `std::atomic` because atomic operations (`fetch_add`, `load`, `store`) are not `constexpr`.

The sibling project [`lds-gen-cpp`](https://github.com/luk036/lds-gen-cpp) makes the opposite choice: runtime `VdCorput` with `std::atomic<unsigned long> count` and `std::memory_order_relaxed` for built-in thread safety, at the cost of `constexpr` on mutating methods.

| Aspect | `lds-cpp` (this project) | `lds-gen-cpp` |
|--------|--------------------------|---------------|
| `constexpr pop()` | ✅ Yes | ❌ No |
| `std::atomic count` | ❌ No (`unsigned long`) | ✅ Yes |
| Thread-safe internally | ❌ No (use external `std::mutex`) | ✅ Yes |
| Compile-time evaluation | ✅ Supported | ❌ Not supported |

**Implication**: Template generators in this project (`VdCorput<Base>`, `Halton<B0, B1>`, etc.) are not safe for concurrent use without external synchronization. The runtime polymorphic classes (`Sphere3`, `SphereN`) use `std::mutex` internally and are thread-safe.