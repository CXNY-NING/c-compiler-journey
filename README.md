# c-compiler-journey

一个用于系统学习编译原理的手写 C 语言子集编译器：词法分析 → 语法分析 → 语义分析 → x86-64 汇编生成。

> **关于原创性（重要）**
> 本项目的核心实现跟随开源教程 [DoctorWkt/acwj](https://github.com/DoctorWkt/acwj) 逐章复现，
> 基础编译器逻辑的版权归原作者所有。本人在此之上的产出为：中文注释、工程化改造（目录重构、
> 编码统一、跨工具链构建、回归测试）、缺陷修复与自主语言扩展。**这不是从零原创的编译器设计。**

## 当前进度

| 项目 | 状态 |
|---|---|
| 教程章节 | 第 01 ~ 11 章 |
| 代码规模 | 约 2000 行 C++ |
| 输出目标 | x86-64 GNU as 汇编（`.s`） |
| 已支持 | 表达式与运算符优先级、变量、赋值、`if/else`、`while`、`for`、`do-while`、函数定义与调用 |
| 自主扩展 | `do-while` 语句 |
| 工程化 | 单库目录结构、UTF-8 源码统一、GCC/Clang/MSVC 三工具链构建、golden 快照回归测试 |

## 目录结构

```
Sub_C/              主工作库：日常改代码全在这里
  include/          头文件
  *.cpp             源文件
  Sub_C.sln         Visual Studio 解决方案
tests/
  cases/            测试用例输入
  expected/         golden 快照（期望输出）
  run.sh            回归测试脚本
build.sh            命令行构建（MinGW / GCC / Clang）
CMakeLists.txt      跨平台构建（CMake / CI / VS「打开文件夹」）
```

历史章节快照未包含在本仓库中，仅在本地归档。

## 构建

```bash
./build.sh          # 编译编译器 → build/cwj.exe
./tests/run.sh      # 跑回归测试
```

使用 Visual Studio：打开 `Sub_C/Sub_C.sln`，工具栏选 `x64` + `Debug`，按 F5。
生成的汇编落在 `Sub_C/out.s`。

使用 CMake：

```bash
cmake -S . -B build-cmake && cmake --build build-cmake
```

## 运行生成的程序

编译器产出的是 x86-64 汇编，需要在 Linux 上汇编并运行：

```bash
gcc -o out out.s && ./out
```

## 回归测试

`tests/run.sh` 采用 golden 快照比对：对每个 `tests/cases/*` 运行编译器，
将生成的汇编与 `tests/expected/*.s` 逐字节比对。

新增语言特性后，确认差异符合预期，用新输出更新对应快照即可。

## 许可

GPL-3.0，与上游教程保持一致，详见 [LICENSE](LICENSE)。

上游项目：<https://github.com/DoctorWkt/acwj>
