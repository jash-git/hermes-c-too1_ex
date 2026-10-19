#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/build/c-file-tool"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP/subdir"
printf 'alpha\nbeta needle\ngamma\n' > "$TMP/sample.txt"
printf 'nested file\n' > "$TMP/subdir/nested.txt"
pass=0
check() { local name="$1"; shift; if "$@"; then printf 'PASS: %s\n' "$name"; pass=$((pass+1)); else printf 'FAIL: %s\n' "$name" >&2; exit 1; fi; }
check 'help prints usage' "$BIN" --help
check 'info reports regular file' bash -c '"$1" info "$2" | grep -q "type: file"' _ "$BIN" "$TMP/sample.txt"
check 'list shows entry' bash -c '"$1" list "$2" | grep -qx sample.txt' _ "$BIN" "$TMP"
check 'find locates nested file' bash -c '"$1" find "$2" nested.txt | grep -q nested.txt' _ "$BIN" "$TMP"
check 'search prints matching line number' bash -c '"$1" search "$2" needle | grep -q "^2:beta needle"' _ "$BIN" "$TMP/sample.txt"
check 'hash emits 16 hex digits' bash -c '"$1" hash "$2" | grep -Eq "^hash: [0-9a-f]{16}$"' _ "$BIN" "$TMP/sample.txt"
check 'missing file returns I/O error' bash -c '"$1" info "$2" >/dev/null 2>&1; test $? -eq 1' _ "$BIN" "$TMP/missing"
check 'no search result returns code 3' bash -c '"$1" search "$2" absent >/dev/null 2>&1; test $? -eq 3' _ "$BIN" "$TMP/sample.txt"
check 'invalid command returns code 2' bash -c '"$1" unknown >/dev/null 2>&1; test $? -eq 2' _ "$BIN"
printf '\nAll %d tests passed.\n' "$pass"
