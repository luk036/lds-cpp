# Setting Up clang-tidy on Windows Without CMake

## 1. Install LLVM/clang-tidy

Two options:

### Option A: Official LLVM installer (recommended)
- Download from https://github.com/llvm/llvm-project/releases — grab `LLVM-<version>-win64.exe`
- Run installer, **check "Add LLVM to the system PATH"**
- After install, verify:
  ```powershell
  clang-tidy --version
  ```

### Option B: Chocolatey
```powershell
choco install llvm
```

**LLVM 22.1.2** is what's used in this project. clang-tidy ships as part of the LLVM distribution — you get it for free with the C++ toolchain.

---

## 2. Create `.clang-tidy` Config

This is the **central configuration file**. Put it in your project root — clang-tidy auto-discovers it by walking up from each source file's directory.

The project's current `.clang-tidy`:
```yaml
---
Checks: >
  -*,
  bugprone-*,
  clang-analyzer-*,
  performance-*,
  modernize-*,
  readability-*,
  cppcoreguidelines-*,
  -modernize-use-trailing-return-type,
  -readability-magic-numbers,
  -cppcoreguidelines-avoid-magic-numbers,
  -readability-identifier-length,
  -cppcoreguidelines-pro-bounds-avoid-unchecked-container-access,
  -readability-math-missing-parentheses,
  ...

CheckOptions:
  - { key: readability-braces-around-statements.ShortStatementLines, value: 3 }
  - { key: modernize-loop-convert.MinConfidence, value: reasonable }
  - { key: cppcoreguidelines-narrowing-conversions.IgnoreConversionFromTypes, value: "double;float;__int64;long long;unsigned long long" }
...
```

**Key points:**
- Start with `*-,` (disable everything) then opt-in with bare `group-*` (e.g., `bugprone-*`, `modernize-*`)
- Use `-prefix` to exclude individual checks (e.g., `-readability-magic-numbers`)
- `CheckOptions` fine-tune per-check behavior (e.g., ignore narrowing on known-safe types)
- Use `--explain-config` to see where every check comes from

---

## 3. Generate `compile_commands.json` (The Hard Part)

clang-tidy needs to know your compiler flags to work. This is the **only real CMake dependency** — CMake has `CMAKE_EXPORT_COMPILE_COMMANDS=ON`. Without CMake, you need alternatives:

### Option A: xmake (what this project uses)
```powershell
xmake project -k compile_commands
```
This generates `compile_commands.json` at the project root. clang-tidy auto-finds it.

### Option B: Ninja + compiledb
```powershell
pip install compiledb
compiledb make
# or
compiledb ninja
```

### Option C: Bear (Linux/WSL only, not native Windows)
```bash
bear -- make
```

### Option D: Build yourself (sledgehammer approach)
Create `compile_commands.json` manually — a JSON array of per-file command objects:
```json
[
  {
    "directory": "D:/github/cpp/lds-cpp",
    "command": "cl.exe /std:c++20 /Iinclude /EHsc /W4 /c source/lds.cpp /Folbuild/obj/lds.obj",
    "file": "source/lds.cpp"
  }
]
```
Impractical beyond trivial projects, but works if you're desperate.

### Option E: Other build systems
Many modern build systems (FASTBuild, Meson, Bazel) can export compile commands — check their docs.

---

## 4. Running clang-tidy

### Basic invocation (single file)
```powershell
clang-tidy source/lds.cpp -- -Iinclude
```

### Run on all source files
```powershell
# Using PowerShell:
Get-ChildItem -Recurse -Filter *.cpp | ForEach-Object { clang-tidy $_.FullName }

# With xmake's compile_commands, clang-tidy auto-resolves includes:
clang-tidy source/lds.cpp
```

### With filtering (suppress third-party noise)
The project's headers sit alongside MSVC STL, doctest, fmt, spdlog. To focus on project code:
```powershell
clang-tidy --quiet source/lds.cpp --extra-arg=-Wno-unused-command-line-argument
```
`--quiet` suppresses stats. The `--extra-arg` flag suppresses MSVC `/c` unused flag warnings.

### Header-filter to scope analysis to your code
```powershell
clang-tidy source/lds.cpp --header-filter="include/lds/.*"
```

### Fix mode (auto-apply suggestions)
```powershell
clang-tidy --fix source/lds.cpp
```
Review all changes — `--fix` is not always perfect. Use `--fix-errors` to apply fixes even
when there are compilation errors (not recommended — may produce broken code).

### Check config without running
```powershell
clang-tidy --dump-config          # prints effective config for a source file
clang-tidy --verify-config        # validates all check names in .clang-tidy
```

### Full project scan (this project's workflow)
```powershell
foreach ($f in Get-ChildItem -Recurse -Filter *.cpp | Where-Object {
    $_.FullName -match '\\source\\|\\test\\source\\|\\standalone\\source\\'
}) {
    clang-tidy --quiet --extra-arg=-Wno-unused-command-line-argument $f.FullName
}
```

---

## 5. Managing Third-Party Noise

This is **the biggest pain point** without a build system abstraction layer. CMake's `SYSTEM`
include directories suppress warnings from third-party headers. Without CMake, apply one of:

### Strategy 1: Header filters
```powershell
clang-tidy --header-filter="include/lds/.*" source/lds.cpp
```
Still analyzes headers included *through* your source, but suppresses diagnostics from
external headers.

### Strategy 2: .clang-tidy HeaderFilterRegex
```yaml
HeaderFilterRegex: "include/lds/.*"
```

### Strategy 3: System include emulation
Pass `-isystem include/third_party` instead of `-Iinclude/third_party` to mark those paths
as system headers. Requires manual command-line construction.

---

## 6. CI Integration

Without CMake, the simplest approach:

```yaml
# GitHub Actions (powershell)
- name: Run clang-tidy
  run: |
    xmake project -k compile_commands
    Get-ChildItem -Recurse -Filter *.cpp | Where-Object {
      $_.FullName -match '\\source\\'
    } | ForEach-Object {
      clang-tidy --quiet --warnings-as-errors=* $_.FullName
    }
```

`--warnings-as-errors=*` makes any finding a non-zero exit.

---

## 7. Editor Integration

### VS Code
Install the **clang-tidy** extension. Configure in `.vscode/settings.json`:
```json
{
  "clang-tidy.executable": "clang-tidy",
  "clang-tidy.fixOnSave": false
}
```

### Visual Studio
LLVM 22.1+ integrates clang-tidy natively. Go to:
- Project Properties → **Code Analysis** → **Enable Clang-Tidy**
- Or use the `clang-tidy.vsix` extension

### Neovim
`null-ls` or `clangd` with `clang-tidy` checks enabled.

---

## 8. Comparison: CMake vs Non-CMake Approach

| Aspect | With CMake | Without CMake |
|---|---|---|
| compile_commands.json | `CMAKE_EXPORT_COMPILE_COMMANDS=ON` (automatic) | Use xmake: `xmake project -k compile_commands` |
| System header suppression | `SYSTEM` target_include_directories | Header-filter regex, `-isystem` flag |
| Invocation | `cmake --build . --target clang-tidy` (with tools.cmake) | Manual `clang-tidy file.cpp` |
| Per-target flags | Inherited from CMake targets | Must pass via `--extra-arg` |
| Learning curve | Low (abstracted) | Medium (manual flags) |

---

## Summary: Minimal Setup Recipe

```powershell
# 1. Install LLVM (if not done)
choco install llvm

# 2. Verify
clang-tidy --version

# 3. Create .clang-tidy at project root  (already done in this project)

# 4. Generate compile_commands.json
xmake project -k compile_commands

# 5. Run
clang-tidy --quiet --header-filter="include/lds/.*" source/lds.cpp

# 6. Fix
clang-tidy --quiet --fix source/lds.cpp
```

This project already has all of this wired up — `.clang-tidy` at the root,
`compile_commands.json` generated by xmake, and all project-specific issues fixed.
Running `clang-tidy --quiet --extra-arg=-Wno-unused-command-line-argument source/lds.cpp`
(or any other source file) should produce zero project warnings.
