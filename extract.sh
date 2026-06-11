#!/usr/bin/env bash
set -euo pipefail

MONOREPO=$(git rev-parse --show-toplevel)
DEST="${MONOREPO}/public/agent-messaging-repo"

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

echo "Done. $(git -C "$DEST" log --oneline | wc -l) commits extracted to $DEST"
echo "Files:"
cd "$DEST" && find . -not -path './.git/*' -type f | sort
