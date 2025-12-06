#!/usr/bin/env bash
# Just a small helper script for myself to rebase correctly in the future
set -e

# Make sure we're in the git repo
if [ ! -d .git ]; then
    echo "Error: Not in a git repository!"
    exit 1
fi

# Switch to master
git checkout master

# Fetch latest from upstream
git fetch upstream

# Rebase master onto upstream/master
git rebase upstream/master

echo "Master branch successfully rebased on top of upstream/master."
echo "If there are conflicts, resolve them and run 'git rebase --continue'."
