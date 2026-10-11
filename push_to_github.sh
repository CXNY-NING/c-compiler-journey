#!/usr/bin/env bash
# ============================================================
#  把「代码部分」推送到 GitHub
#
#  只推：Sub_C/  tests/  build.sh  CMakeLists.txt  LICENSE
#        README.md  .gitignore  .gitattributes  本脚本
#  不推：docs/（课程文档，含个人学号等信息）
#        legacy/（章节学习快照）
#        大三上第二三周周报.pdf（课程周报）
#
#  实现方式：先把本地仓库克隆到临时目录，在「克隆副本」上过滤，
#            原仓库完全不动。提交历史会被保留（只含代码相关的提交）。
#
#  用法：
#    ./push_to_github.sh <你的 GitHub 用户名> [仓库名]
#    ./push_to_github.sh --dry-run          # 只演练过滤，不推送
#
#  认证：Git Credential Manager，浏览器授权，无需手动创建 token。
# ============================================================
set -e

# ---------- 参数 ----------
DRY_RUN=0
SQUASH=0
SKIP_SECRET_SCAN=0
POSITIONAL=()
for arg in "$@"; do
    case "$arg" in
        --dry-run)          DRY_RUN=1 ;;
        --squash)           SQUASH=1  ;;   # 丢掉历史，只保留一次提交（最快）
        --skip-secret-scan) SKIP_SECRET_SCAN=1 ;;
        *)                  POSITIONAL+=("$arg") ;;
    esac
done
USERNAME="${POSITIONAL[0]:-}"
REPO="${POSITIONAL[1]:-c-compiler-journey}"

if [ "$DRY_RUN" -eq 0 ] && [ -z "$USERNAME" ]; then
    echo "用法：./push_to_github.sh <你的 GitHub 用户名> [仓库名]"
    echo "      ./push_to_github.sh --dry-run        # 只演练，不推送"
    echo "      ./push_to_github.sh <用户名> --squash # 不保留提交历史"
    echo
    echo "示例：./push_to_github.sh CXNY-NING"
    echo "      ./push_to_github.sh CXNY-NING c-compiler-journey"
    exit 1
fi

cd "$(dirname "$0")"
PROJECT_ROOT="$(pwd)"

# ---------- 不推送的内容 ----------
EXCLUDE_DIRS=(docs legacy)
EXCLUDE_FILES=("大三上第二三周周报.pdf")

echo "==> 项目根目录：$PROJECT_ROOT"
echo "==> 目标仓库 ：${USERNAME:-<dry-run>}/$REPO"
echo "==> 排除目录 ：${EXCLUDE_DIRS[*]}"
echo "==> 排除文件 ：${EXCLUDE_FILES[*]}"
echo

# ---------- 1. 克隆到临时目录 ----------
TMPDIR_PUB="$(mktemp -d)"
WORK="$TMPDIR_PUB/repo"
echo "==> [1/5] 克隆到临时目录：$WORK"
git clone --quiet --no-hardlinks "$PROJECT_ROOT" "$WORK"
cd "$WORK"
# 断开与本地仓库的联系：否则 origin/main 远程跟踪引用会指向含 docs 的旧对象，
# 导致后续 gc 回收不掉，推送体积不降。
git remote remove origin 2>/dev/null || true
TOTAL_COMMITS="$(git rev-list --count HEAD)"
echo "    克隆完成，提交数：$TOTAL_COMMITS"
echo

# ---------- 2. 过滤 ----------
FB_LOG="$TMPDIR_PUB/filter.log"

if [ "$SQUASH" -eq 1 ]; then
    echo "==> [2/5] --squash 模式：丢弃历史，生成单个提交"
    git checkout --orphan pub-main --quiet
    # 只清空索引，工作区文件保留（不能用 git clean：此刻所有文件都已变成
    # 未跟踪状态，clean 会把 Sub_C/ tests/ 一起删掉）
    git rm -r -q --cached . 2>/dev/null || true
    for d in "${EXCLUDE_DIRS[@]}"; do rm -rf "${d:?}"; done
    for f in "${EXCLUDE_FILES[@]}"; do rm -f "$f"; done
    git add -A
    git commit --quiet -m "feat: 手写 C 语言子集编译器（基于 acwj 教程复现 + 自主扩展）" \
                       -m "跟随 DoctorWkt/acwj 教程完成第 01~11 章，累计约 2000 行 C++，输出 x86-64 汇编。" \
                       -m "自主扩展 do-while；完成目录重构、UTF-8 统一、跨工具链构建与回归测试。" \
                       -m "基础编译器逻辑版权归原作者，详见 LICENSE 与 README 的原创性说明。"
    # 旧 tag 指向被丢弃的历史，不删的话 gc 回收不掉旧对象，体积白大
    for t in $(git tag); do git tag -d "$t" >/dev/null 2>&1; done
    git branch -M main
else
    echo "==> [2/5] 过滤掉非代码内容（保留代码相关的提交历史，约需 1 分钟）"

    # 组装 --path 参数（filter-repo 用）
    FR_ARGS=""
    for d in "${EXCLUDE_DIRS[@]}"; do FR_ARGS="$FR_ARGS --path $d/"; done
    for f in "${EXCLUDE_FILES[@]}"; do FR_ARGS="$FR_ARGS --path $f"; done

    if command -v git-filter-repo >/dev/null 2>&1; then
        echo "    使用 git-filter-repo（快）"
        # shellcheck disable=SC2086
        git filter-repo --invert-paths $FR_ARGS --force --quiet
    elif git filter-repo --version >/dev/null 2>&1; then
        echo "    使用 git filter-repo（git 子命令形式）"
        # shellcheck disable=SC2086
        git filter-repo --invert-paths $FR_ARGS --force --quiet
    else
        echo "    未找到 filter-repo，回退到 git filter-branch（较慢）"
        RM_ARGS=""
        for d in "${EXCLUDE_DIRS[@]}"; do RM_ARGS="$RM_ARGS \"$d\""; done
        for f in "${EXCLUDE_FILES[@]}"; do RM_ARGS="$RM_ARGS \"$f\""; done
        if ! FILTER_BRANCH_SQUELCH_WARNING=1 git filter-branch \
                --index-filter "git rm -r -q --cached --ignore-unmatch $RM_ARGS" \
                --tag-name-filter cat --prune-empty -- --all \
                >"$FB_LOG" 2>&1 </dev/null; then
            printf '\n'
            echo "    ✗ filter-branch 失败，日志：$FB_LOG"
            tail -20 "$FB_LOG"
            exit 1
        fi
        # filter-branch 用 \r 写进度，末尾可能没有换行，补一个空行避免
        # 后续输出被拼到同一行上
        printf '\n'
        # 清理 filter-branch 留下的备份引用
        git for-each-ref --format="%(refname)" refs/original/ 2>/dev/null \
            | while read -r ref; do git update-ref -d "$ref"; done
    fi
fi

git reflog expire --expire=now --all 2>/dev/null || true
git gc --prune=now --quiet 2>/dev/null || true
echo "    过滤完成，提交数：$(git rev-list --count HEAD)（原始 $TOTAL_COMMITS）"
echo

# ---------- 3. 安全校验 ----------
echo "==> [3/5] 校验过滤结果"
FAILED=0

for d in "${EXCLUDE_DIRS[@]}"; do
    if git ls-files | grep -q "^${d}/"; then
        echo "    ✗ 仍然包含目录：$d"; FAILED=1
    else
        echo "    ✓ 已移除目录：$d"
    fi
done
for f in "${EXCLUDE_FILES[@]}"; do
    if git ls-files | grep -qxF "$f"; then
        echo "    ✗ 仍然包含文件：$f"; FAILED=1
    else
        echo "    ✓ 已移除文件：$f"
    fi
done

for f in Sub_C tests build.sh CMakeLists.txt LICENSE README.md; do
    if git ls-files | grep -q "^${f}\(/\|$\)"; then
        echo "    ✓ 保留：$f"
    else
        echo "    ✗ 缺失：$f"; FAILED=1
    fi
done

# 敏感信息扫描
# 注意：模式必须带长度量词（如 ghp_ 后跟 20+ 位），否则本脚本自身的
# 这些模式字符串会被扫出来，形成「扫描器扫到自己」的误报。
if [ "$SKIP_SECRET_SCAN" -eq 1 ]; then
    echo "    – 已跳过密钥扫描（--skip-secret-scan）"
else
    SECRET_RE='ghp_[A-Za-z0-9]{20,}|github_pat_[A-Za-z0-9_]{20,}|AKIA[0-9A-Z]{16}|-----BEGIN [A-Z ]*PRIVATE KEY'
    SECRET_HITS="$(git grep -lE "$SECRET_RE" HEAD -- . 2>/dev/null || true)"
    if [ -n "$SECRET_HITS" ]; then
        echo "    ✗ 检出疑似密钥，已中止："
        printf '%s\n' "$SECRET_HITS" | sed 's/^/        /'
        echo "      确认是误报可加 --skip-secret-scan 跳过"
        FAILED=1
    else
        echo "    ✓ 未检出密钥/Token"
    fi
fi

if [ "$FAILED" -ne 0 ]; then
    echo
    echo "校验未通过，未推送。临时目录保留待查：$WORK"
    exit 1
fi

FILE_COUNT="$(git ls-files | wc -l | tr -d ' ')"
REPO_SIZE="$(du -sh .git 2>/dev/null | cut -f1)"
echo "    —— 文件数：$FILE_COUNT   仓库体积：$REPO_SIZE"
echo

# ---------- 4. dry-run 到此为止 ----------
if [ "$DRY_RUN" -eq 1 ]; then
    echo "==> [4/5] dry-run 模式，跳过推送"
    echo
    echo "==> [5/5] 即将推送的文件清单（前 40 条）："
    git ls-files | head -40 | sed 's/^/    /'
    echo
    echo "演练完成。临时目录：$WORK"
    echo "确认无误后执行：./push_to_github.sh <你的 GitHub 用户名>"
    exit 0
fi

# ---------- 5. 推送 ----------
echo "==> [4/5] 配置凭据管理器 + 绑定远程"
git config credential.helper manager
git remote remove origin 2>/dev/null || true
git remote add origin "https://github.com/$USERNAME/$REPO.git"
echo "    origin -> https://github.com/$USERNAME/$REPO.git"
echo

echo "==> [5/5] 推送分支 main"
echo
echo "    若弹出登录窗口："
echo "      · 选「Sign in with your browser」，浏览器里点确认"
echo "      · 不要输入 GitHub 登录密码（GitHub 已不支持密码推送）"
echo "      · 注意：用户名是 GitHub 登录名，不一定等于 git config 里的姓名"
echo
git push -u origin main

echo
echo "==> 完成。打开查看：https://github.com/$USERNAME/$REPO"
rm -rf "$TMPDIR_PUB"
