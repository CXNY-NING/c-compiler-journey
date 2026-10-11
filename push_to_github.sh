#!/usr/bin/env bash
# ============================================================
# 把本地仓库推送到 GitHub（仓库需要提前在网页上建好）
#
# 用法：
#   ./push_to_github.sh <你的 GitHub 用户名>
#   ./push_to_github.sh <你的 GitHub 用户名> <仓库名>
#
# 认证方式：Git Credential Manager，走浏览器授权，
#           不需要手动创建和粘贴 Personal Access Token。
# ============================================================
set -e

USERNAME="$1"
REPO="${2:-c-compiler-journey}"

if [ -z "$USERNAME" ]; then
    echo "用法：./push_to_github.sh <你的 GitHub 用户名> [仓库名]"
    echo "示例：./push_to_github.sh CXNY-NING"
    echo "      ./push_to_github.sh CXNY-NING c-compiler-journey"
    exit 1
fi

cd "$(dirname "$0")"

echo "==> 1/3 配置凭据管理器（本仓库级，不影响其他项目）"
git config credential.helper manager

echo "==> 2/3 绑定远程仓库 https://github.com/$USERNAME/$REPO.git"
git remote remove origin 2>/dev/null || true
git remote add origin "https://github.com/$USERNAME/$REPO.git"

echo "==> 3/3 推送分支 $(git branch --show-current)"
echo
echo "    接下来若弹出登录窗口："
echo "      · 选「Sign in with your browser」，浏览器里确认即可"
echo "      · 不要输入 GitHub 登录密码（GitHub 已不支持密码推送）"
echo
git push -u origin main

echo
echo "==> 完成。打开查看：https://github.com/$USERNAME/$REPO"
