#!/usr/bin/env bash
# ============================================================
# ACWJ-C 回归测试
#
# 用法：
#   ./tests/run.sh            # 跑全部用例
#   ./tests/run.sh --build    # 先构建再测试
#
# 测试策略（M0 阶段）：
#   采用「汇编文本快照比对」——把编译器生成的 .s 与 tests/expected/ 下的
#   golden 文件逐字节比对。基线快照取自重构前的输出，用于防止回归。
#
#   注意：当前 MinGW 为 32 位，无法汇编 x86-64 目标代码，因此暂不能做
#   「编译 -> 运行 -> 比对程序输出」的端到端测试。待引入 64 位工具链
#   （MinGW-w64 / MSYS2 / Linux）后，应升级为端到端比对，那才是真正
#   验证语义正确性的方式。见 docs/执行路线图.md。
# ============================================================
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)

if [ "$1" = "--build" ]; then
    ./build.sh || exit 1
fi

CWJ="$ROOT/build/cwj.exe"
if [ ! -x "$CWJ" ]; then
    echo "找不到编译器 $CWJ，请先运行 ./build.sh"
    exit 1
fi

WORK="$ROOT/build/testout"
mkdir -p "$WORK"
rm -f "$WORK"/*.s

pass=0
fail=0
failed_list=""

for case_file in "$ROOT"/tests/cases/input*; do
    [ -e "$case_file" ] || continue
    name=$(basename "$case_file")
    golden="$ROOT/tests/expected/$name.s"

    # 编译器写死输出 out.s，故在独立工作目录中运行
    ( cd "$WORK" && rm -f out.s && "$CWJ" "$case_file" >/dev/null 2>&1 )
    rc=$?

    if [ $rc -ne 0 ]; then
        echo "FAIL  $name  （编译器以退出码 $rc 结束）"
        fail=$((fail + 1))
        failed_list="$failed_list $name"
        continue
    fi

    if [ ! -f "$golden" ]; then
        echo "SKIP  $name  （缺少 golden 快照：$golden）"
        continue
    fi

    if diff -q "$WORK/out.s" "$golden" >/dev/null 2>&1; then
        echo "PASS  $name"
        pass=$((pass + 1))
    else
        echo "FAIL  $name  （汇编输出与 golden 不一致）"
        echo "        diff: diff $WORK/out.s $golden"
        fail=$((fail + 1))
        failed_list="$failed_list $name"
    fi
done

echo
echo "=============================="
echo "通过 $pass   失败 $fail"
if [ $fail -eq 0 ]; then
    echo "全部用例通过"
    exit 0
else
    echo "失败用例：$failed_list"
    exit 1
fi
