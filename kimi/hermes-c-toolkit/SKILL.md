---
name: hermes-c-toolkit
description: "Use the ctool pure-C CLI utility to compute stats, FNV-1a hashes, Base64 encode/decode, arithmetic expressions, and text transforms (upper/lower/reverse). Use when you need fast deterministic offline computations, file statistics, hashing, base64 conversion, or a calculator without relying on Python or external tools."
metadata:
  author: Hermes Toolkit
  version: "1.0.0"
  keywords: "c,cli,calculator,base64,hash,stats,tool"
compatibility: Requires a C99 compiler (cc/gcc/clang) to build from source. Prebuilt binary at bin/ctool works on Linux x86_64. No runtime dependencies beyond libc.
---

# hermes-c-toolkit

Call the `ctool` binary — a single-file pure C (C99) CLI with no external dependencies — for fast, deterministic offline computations.

## When to Use

- Need arithmetic evaluated quickly and deterministically (`calc`)
- Need file line/word/byte statistics (`stats`)
- Need a content fingerprint / dedup key (`hash`, FNV-1a 64-bit)
- Need Base64 encoding/decoding without Python (`b64e` / `b64d`)
- Need simple text transforms (`upper` / `lower` / `rev`)

## Setup

Binary is prebuilt at `bin/ctool`. If it does not run on your platform, rebuild:

```bash
sh scripts/build.sh        # uses $CC (default cc), -std=c99 -O2
```

Verify: `bin/ctool calc "2+2"` → `4`

## Command Reference

All commands print result to stdout, errors to stderr, and exit 0 on success / non-zero on failure.

| Command | Arguments | Output | Example |
|---|---|---|---|
| `stats` | `<file>` | lines / words / bytes of file | `file: a.txt  lines: 3  words: 13  bytes: 71` |
| `hash` | `<file>` | `fnv1a64: <16 hex chars>` | `fnv1a64: 87c07501da139737` |
| `b64e` | `<string>` | base64-encoded string | `ctool b64e "hi"` → `aGk=` |
| `b64d` | `<string>` | decoded string | `ctool b64d "aGk="` → `hi` |
| `calc` | `"<expr>"` | result as `%g` | `ctool calc "(3+4)*5/2"` → `17.5` |
| `upper` | `<string>` | uppercase | `ctool upper "abc"` → `ABC` |
| `lower` | `<string>` | lowercase | `ctool lower "AbC"` → `abc` |
| `rev` | `<string>` | reversed | `ctool rev "abc"` → `cba` |

### calc expression syntax

- Operators: `+` `-` `*` `/` `%` (modulo on integers) and parentheses `( )`
- Unary `+`/`-` supported; standard precedence applies
- Divide by zero → stderr `ctool: 除以零`, exit 1
- Malformed expression → stderr with parse position, exit 1

## Usage Patterns

### Batch file fingerprinting
```bash
for f in data/*.log; do bin/ctool hash "$f"; done
```

### Quick sanity check of a base64 payload
```bash
bin/ctool b64d "$PAYLOAD" | head -c 200
```

### Compute inside shell pipelines
```bash
bin/ctool calc "1024 * 1024 * 7 / 3"   # → 2449472
```

## Error Handling Contract

- **exit 0**: success, result on stdout (single line)
- **exit 1**: runtime error (missing file, divide by zero, invalid base64) — message on stderr
- **exit 2**: usage error (wrong argument count) — usage printed to stderr

Always check the exit code when scripting; never parse stderr as output.

## Notes

- `hash` is FNV-1a 64-bit — fast, non-cryptographic; suitable for dedup keys, NOT for security.
- `b64d` requires input length multiple of 4 and rejects invalid characters.
- Source: `src/ctool.c` (~200 lines, single file, C99 only).
