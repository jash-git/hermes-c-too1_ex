#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
if [ ! -x "$ROOT/build/hermes-calc" ]; then
    make -C "$ROOT" >/dev/null
fi
exec "$ROOT/build/hermes-calc" "$@"
