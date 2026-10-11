cmake_minimum_required(VERSION 3.10)
project(acwj-c CXX)

# ============================================================
# ACWJ-C：C 语言子集编译器学习与优化项目
#
# 构建：
#   cmake -B build/cmake -S .
#   cmake --build build/cmake
#
# 测试（需 bash）：
#   ctest --test-dir build/cmake
#
# 说明：本文件与 build.sh 并存。build.sh 用于当前无 CMake 的 MinGW 环境；
#       CMake 用于 CI（ubuntu-latest）与 Visual Studio「打开文件夹」模式。
# ============================================================

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
# 注：Parser.cpp / Parser1.cpp / Parser2.cpp 沿袭教程阶段划分，非按职责切分，
#     属待重构项（FR-I-3.1）。三者均参与构建，缺一即链接失败。
set(CWJ_SOURCES
    Sub_C/main.cpp
    Sub_C/scan.cpp
    Sub_C/ast.cpp
    Sub_C/cg.cpp
    Sub_C/gen.cpp
    Sub_C/stmt.cpp
    Sub_C/symbol.cpp
    Sub_C/Parser.cpp
    Sub_C/Parser1.cpp
    Sub_C/Parser2.cpp
)

add_executable(cwj ${CWJ_SOURCES})
target_include_directories(cwj PRIVATE Sub_C/include)

# 源码统一 UTF-8（带 BOM）。Windows 控制台为 GBK 代码页，故执行字符集转 GBK，
# 避免 printf 的中文诊断信息乱码；Linux 下保持 UTF-8。
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(cwj PRIVATE
        -finput-charset=UTF-8
        -fexec-charset=GBK)
endif()

if(MSVC)
    target_compile_options(cwj PRIVATE /utf-8)
    target_compile_definitions(cwj PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# 回归测试：汇编文本快照比对（详见 tests/run.sh）
if(UNIX OR CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    enable_testing()
    add_test(NAME regression
             COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/run.sh)
    set_tests_properties(regression PROPERTIES
        PASS_REGULAR_EXPRESSION "全部用例通过")
endif()
