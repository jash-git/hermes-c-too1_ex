# Hermes C File Tool

A small read-only filesystem inspection CLI written in C for Linux/Ubuntu, with a Hermes Agent `SKILL.md` wrapper. It uses libc/POSIX APIs and requires no third-party C libraries.

## Features

- `info PATH`: metadata (type, size, mode, UID/GID, mtime)
- `list DIRECTORY`: immediate directory entries
- `find DIRECTORY BASENAME`: recursive exact-basename search, no symlink-directory traversal, maximum recursion depth 64
- `search FILE TEXT`: literal substring search with line numbers
- `hash FILE`: FNV-1a 64-bit checksum (non-cryptographic)

The tool is deliberately read-only: it does not alter files.

## Requirements

- Linux / Ubuntu
- C compiler (`cc`, usually GCC)
- Bash for build/install/test scripts

Install compiler if needed: `sudo apt-get update && sudo apt-get install -y build-essential`.

## Build

```bash
./scripts/build.sh
./build/c-file-tool --help
```

## Test

```bash
./tests/test_tool.sh
```

The test script builds no files itself; run the build step first. It uses temporary test data and removes it on exit.

## Install for current user

```bash
./scripts/install.sh
```

Defaults:
- binary: `~/.local/bin/c-file-tool`
- skill: `~/.hermes/skills/c-file-tool/SKILL.md`

Override install paths if needed:

```bash
PREFIX="$HOME/.local" HERMES_SKILLS_DIR="$HOME/.hermes/skills" ./scripts/install.sh
```

Make sure `~/.local/bin` is in `PATH`. Hermes skill discovery locations can vary by version/configuration; if your installation uses a different skills directory, set `HERMES_SKILLS_DIR` accordingly. Restart/reload Hermes if needed, then ask it to use the `c-file-tool` skill.

## Examples

```bash
c-file-tool info "$HOME/.hermes/config.yaml"
c-file-tool list "$HOME/.hermes/skills"
c-file-tool find "$HOME/.hermes" SKILL.md
c-file-tool search "$HOME/.hermes/config.yaml" MCP_Server
c-file-tool hash ./build/c-file-tool
```

## Exit codes

- `0`: success
- `1`: I/O failure
- `2`: invalid command/arguments or search root is not a directory
- `3`: no matches

## Notes / limitations

- `find` skips directories it cannot open and limits recursion to 64 levels; it is not a forensic-grade exhaustive scanner.
- `search` is literal substring matching, not regex, and is intended for text files.
- `hash` uses FNV-1a-64, not SHA-256 or another cryptographic hash.
- The CLI prints plain text for predictable shell/agent consumption; paths with spaces must be quoted.
