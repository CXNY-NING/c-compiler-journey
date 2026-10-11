#!/usr/bin/env bash
# ============================================================
# ACWJ-C 构建脚本（MinGW / GCC 路径）
#
# 用法：
#   ./build.sh            # 构建到 build/cwj.exe
#   ./build.sh clean      # 清理
#
# 说明：
#   本脚本只负责构建「编译器本体」。编译器生成的是 x86-64 GNU as 汇编，
#   汇编与链接需要 64 位 GNU 工具链（MinGW-w64 / MSYS2 / Linux），
#   32 位 MinGW 无法完成该步骤（见 docs/执行路线图.md 环境说明）。
# ============================================================
set -e
cd "$(dirname "$0")"

SRC=Sub_C
OUT=build/cwj.exe
CXX=${CXX:-g++}

if [ "$1" = "clean" ]; then
    rm -rf build
    echo "已清理 build/"
    exit 0
fi

mkdir -p build

# 源文件
# 注：Parser.cpp / Parser1.cpp / Parser2.cpp 的划分沿袭教程阶段，并非按职责切分，
#     属于待重构项（见 FR-I-3.1）。当前三者均参与构建，缺一即链接失败。
SOURCES=(
    "$SRC/main.cpp"
    "$SRC/scan.cpp"
    "$SRC/ast.cpp"
    "$SRC/cg.cpp"
    "$SRC/gen.cpp"
    "$SRC/stmt.cpp"
    "$SRC/symbol.cpp"
    "$SRC/Parser.cpp"
    "$SRC/Parser1.cpp"
    "$SRC/Parser2.cpp"
)

echo "=== 构建 ACWJ-C 编译器 ==="
echo "编译器: $CXX  ($($CXX --version | head -1))"
echo "输出  : $OUT"
echo

"$CXX" -std=c++14 -O0 -g -Wall \
    -I"$SRC/include" \
    -finput-charset=UTF-8 \
    -fexec-charset=GBK \
    "${SOURCES[@]}" \
    -o "$OUT"

echo "构建成功：$OUT"
