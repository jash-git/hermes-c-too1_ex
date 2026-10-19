#!/usr/bin/env bash
# Helper: ensure c-calc is compiled (to a writable location), then exec it.
# Hermes can call this via: bash ${HERMES_SKILL_DIR}/scripts/run-c-calc.sh "expr"

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC="${SCRIPT_DIR}/c-calc.c"

# Prefer a user-writable cache location (avoids noexec mounts)
CACHE_DIR="${XDG_CACHE_HOME:-$HOME/.cache}/c-calc"
mkdir -p "$CACHE_DIR"
BIN="${CACHE_DIR}/c-calc"

if [[ ! -x "$BIN" ]] || [[ "$SRC" -nt "$BIN" ]]; then
  echo "[c-calc] compiling..." >&2
  gcc -O2 -Wall -o "$BIN" "$SRC" -lm
  chmod +x "$BIN"
fi

exec "$BIN" "$@"