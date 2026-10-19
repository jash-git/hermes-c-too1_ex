#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CC="${CC:-cc}"
mkdir -p "$ROOT/build"
"$CC" -std=c11 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wall -Wextra -Wpedantic -Werror -O2 -I"$ROOT/include" "$ROOT/src/main.c" "$ROOT/src/filetool.c" -o "$ROOT/build/c-file-tool"
printf 'Built: %s\n' "$ROOT/build/c-file-tool"
