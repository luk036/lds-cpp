## Summary

After a thorough analysis of the codebase, several performance bottlenecks and optimization opportunities were identified. Below is a prioritized list of findings.

---

## 🔴 High Priority

### 1. `GeneratorIterator::operator*` — destructive read, 3x penalty per dereference

**File:** `include/lds/lds.hpp:50-59`

The iterator dereference modifies the generator state (reseed → pop → reseed back) even for a "read" operation. Each `*it` costs 2 `reseed()` + 1 `pop()` = 3x the cost of a normal `pop()`. It also breaks `const`-correctness by mutating the generator from a const method.

**Suggestion:** Redesign the iterator so that `operator*` simply calls `pop()` (single-pass input iterator semantics), or materialize the range upfront.

---

### 2. `VdCorputDynamic` — no precomputed `rev_lst`, runtime division every iteration

**File:** `include/lds/lds_n.hpp:43-68`

The runtime-polymorphic VdCorput does **not** use the precomputed reverse-lookup table that the compile-time version uses. Every `pop()` divides and multiplies by `base` in a loop, computing `denom` from scratch.

**Suggestion:** Precompute a `rev_lst` vector in the constructor, same pattern as the template version.

---

### 3. `log_with_spdlog()` — recreates logger on every call

**File:** `source/logger.cpp:8-27`

Each call drops and recreates the file logger, acquires filesystem locks, and opens file handles. This is **extremely expensive** for a logging function.

**Suggestion:** Use a function-local `static` logger so it is created once and reused.

---

## 🟡 Medium Priority

### 4. `simple_interp()` — O(n) linear search on every Sphere3/SphereN pop

**File:** `source/sphere_n.tpp:43-49`

Every call to `Sphere3::pop()` and `SphereN::pop()` does a linear scan of a 300-element table. For millions of samples this adds up significantly.

**Suggestion:** Use `std::upper_bound` (binary search, O(log n)) or, since the x-points are uniformly spaced, compute the index directly in O(1):
```cpp
double t = (x_value - x_points.front()) / (x_points.back() - x_points.front());
std::size_t i = static_cast<std::size_t>(t * (x_points.size() - 1));
```

---

### 5. Missing build optimization flags

**Files:** `CMakeLists.txt`, `test/CMakeLists.txt`

The project does not set `-O3` / `-DNDEBUG` for Release builds. For a performance-oriented numerical library, this leaves significant performance on the table.

**Suggestion:** Add per-config compile options:
```cmake
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)
```
Also consider enabling `INTERPROCEDURAL_OPTIMIZATION` (LTO).

---

## 🟢 Low Priority

### 6. `ilds::VdCorput` — `std::pow(Base, scale)` at runtime

**File:** `include/lds/ilds.hpp:40`

`std::pow` with compile-time Base and a runtime scale can be replaced with a `constexpr` multiplication loop.

---

### 7. `get_tp_recursive()` — copies vectors on every recursion

**File:** `source/sphere_n.tpp:92-110`

Each recursion level returns `std::vector<double>` by value (300 elements). Since `get_tp()` caches results, this is primarily a cold-start cost, but making the computation iterative would eliminate it entirely.

---

### 8. `Sphere3`/`SphereN` — mutex in every `pop()`/`reseed()`

**File:** `source/sphere_n.tpp:139-238`

Every call acquires `std::mutex` for thread safety. If single-threaded use is the common case, consider a policy-based approach or a separate single-threaded variant.
