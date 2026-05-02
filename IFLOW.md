# IFLOW.md - lds-cpp 项目上下文

## 项目概述

`lds-cpp` 是一个 C++ 库，实现了低差异序列（Low Discrepancy Sequence）生成器。这些序列在计算机图形学、数值积分和蒙特卡罗模拟等领域中具有重要应用，因为它们比随机数具有更好的均匀性。

主要特性包括：

1. van der Corput 序列
2. Halton 序列
3. Circle 序列
4. Sphere 序列
5. 3-Sphere Hopf 序列
6. N 维 Halton 序列

该项目基于现代 CMake 实践构建，支持单头文件库和各种规模的项目，具有干净的库和可执行代码分离、集成测试套件、CI/CD 支持等功能。

## 技术栈和依赖

- **C++11** 或更高版本
- **CMake** (最低版本 3.14) 用于构建系统
- **CPM.cmake** 用于依赖管理
- **fmt** 库 (版本 12.1.0)
- **doctest** 用于测试框架
- **clang-format** 和 **cmake-format** 用于代码格式化

## 主要文件结构

- `include/lds/` - 包含所有头文件
  - `lds.hpp` - 主要的序列生成器类定义
  - `ilds.hpp` - 整数版本的序列生成器
  - `lds_n.hpp` - N 维 Halton 序列生成器和质数表
  - `div_mod.tpp` - 模板实现文件
- `source/lds.cpp` - 源文件，定义了质数表
- `test/source/` - 包含测试文件
- `standalone/source/main.cpp` - 独立示例程序
- `CMakeLists.txt` - 主构建配置文件

## 核心功能

### 序列生成器类

1. **VdCorput** - van der Corput 序列生成器
2. **Halton** - 二维 Halton 序列生成器
3. **Circle** - 单位圆上的序列生成器
4. **Disk** - 单位圆盘上的序列生成器
5. **Sphere** - 单位球面上的序列生成器
6. **Sphere3Hopf** - 使用 Hopf 纤维化的 3-球面序列生成器
7. **HaltonN** - N 维 Halton 序列生成器

### 使用方式

所有生成器类都提供 `pop()` 方法来获取序列中的下一个值，并提供 `reseed()` 方法来重置序列到特定起点。

## 构建和运行

### 构建独立目标

```bash
cmake -S standalone -B build/standalone
cmake --build build/standalone
./build/standalone/Lds --help
```

### 构建和运行测试套件

```bash
cmake -S test -B build/test
cmake --build build/test
CTEST_OUTPUT_ON_FAILURE=1 cmake --build build/test --target test

# 或直接调用可执行文件
./build/test/LdsTests
```

### 构建所有内容

```bash
cmake -S all -B build
cmake --build build
```

## 开发约定

- 遵循现代 C++ 最佳实践
- 使用 C++11 标准
- 遵循 C++ Core Guidelines
- 代码格式化使用 clang-format
- 测试使用 doctest 框架
- 依赖管理使用 CPM.cmake

## 常用命令

- 代码格式化检查: `cmake --build build/test --target format`
- 代码格式化应用: `cmake --build build/test --target fix-format`
- 文档生成: `cmake --build build --target GenerateDocs`