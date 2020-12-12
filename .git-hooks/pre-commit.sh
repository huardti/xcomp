#!/bin/sh
# Stop commit if files are not comforming to clang-format

ret=0

# Stash unstaged changes to only check committed files
STASH_NAME="pre-commit-$(date +%s)"
git stash save -q --keep-index "$STASH_NAME"

output=$(git clang-format --style=file)
case "$output" in
    *"changed files:"* )
        echo "clang-format:"
        echo "$output"
        ret=1
        ;;
esac

# Restore unstaged changes
git stash pop -q
exit $ret
