#!/usr/bin/env bash
set -euo pipefail

MONOREPO=$(git rev-parse --show-toplevel)
DEST="${MONOREPO}/public/agent-messaging-repo"
REMOTE="https://github.com/beefeater7/agent-messaging.git"

WORKDIR=$(mktemp -d)
trap 'rm -rf "$WORKDIR"' EXIT

echo "Cloning monorepo..."
git clone "$MONOREPO" "$WORKDIR" >/dev/null 2>&1

cd "$WORKDIR"
echo "Extracting projects/agent-messaging/..."
git-filter-repo --subdirectory-filter projects/agent-messaging/ --force >/dev/null 2>&1

echo "Moving to $DEST..."
rm -rf "$DEST"
mv "$WORKDIR" "$DEST"
trap '' EXIT

git -C "$DEST" config user.name  "$(git config user.name)"
git -C "$DEST" config user.email "$(git config user.email)"
git -C "$DEST" remote add origin "$REMOTE" 2>/dev/null || git -C "$DEST" remote set-url origin "$REMOTE"

echo "$(git -C "$DEST" log --oneline | wc -l) commits extracted to $DEST"
