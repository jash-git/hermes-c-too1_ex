#!/bin/sh
# 編譯 ctool — 需要 C99 編譯器 (cc / gcc / clang)
set -e
cd "$(dirname "$0")/.."
CC="${CC:-cc}"
$CC -std=c99 -O2 -Wall -Wextra -o bin/ctool src/ctool.c
echo "built: bin/ctool"
