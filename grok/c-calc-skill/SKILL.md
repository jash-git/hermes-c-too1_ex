---
name: c-calc
description: Pure C arithmetic expression calculator. Evaluate math expressions with + - * / % ^ and parentheses.
version: 1.0.0
author: Grok
license: MIT
platforms: [linux, macos]
metadata:
  hermes:
    tags: [math, calculator, pure-c, cli, expression]
    category: utilities
---

# c-calc Skill

A zero-dependency pure C arithmetic expression evaluator. Useful when you need reliable floating-point math without spawning Python or relying on `bc`/`awk` quirks.

Supports operators (high → low precedence):

| Operator | Meaning              | Associativity |
|----------|----------------------|---------------|
| `^`      | Power                | Right         |
| `* / %`  | Multiply / Divide / Modulo | Left   |
| `+ -`    | Add / Subtract       | Left          |
| `( )`    | Grouping             | —             |
| unary `+ -` | Signs             | —             |

## When to Use

- User asks to evaluate a mathematical expression.
- You need precise arithmetic results inside a larger workflow.
- Prefer a tiny native binary over Python one-liners or `bc`.
- Running on Linux or macOS (requires `gcc` only for first compile).

## Prerequisites

- `gcc` (or compatible C compiler) available in PATH for the first run.
- After first compilation the binary is cached next to the source.

## How to Run

Always prefer the helper script (it auto-compiles if needed):

```bash
bash ${HERMES_SKILL_DIR}/scripts/run-c-calc.sh "EXPRESSION"