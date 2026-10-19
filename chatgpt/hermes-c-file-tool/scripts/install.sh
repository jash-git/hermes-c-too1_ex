#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PREFIX="${PREFIX:-$HOME/.local}"
SKILL_DIR="${HERMES_SKILLS_DIR:-$HOME/.hermes/skills}"
"$ROOT/scripts/build.sh"
install -d "$PREFIX/bin" "$SKILL_DIR/c-file-tool"
install -m 0755 "$ROOT/build/c-file-tool" "$PREFIX/bin/c-file-tool"
install -m 0644 "$ROOT/skills/c-file-tool/SKILL.md" "$SKILL_DIR/c-file-tool/SKILL.md"
printf 'Installed binary: %s/bin/c-file-tool\nInstalled skill: %s/c-file-tool/SKILL.md\n' "$PREFIX" "$SKILL_DIR"
printf 'Ensure %s/bin is in PATH. Restart/reload Hermes if your version requires it to discover new skills.\n' "$PREFIX"
