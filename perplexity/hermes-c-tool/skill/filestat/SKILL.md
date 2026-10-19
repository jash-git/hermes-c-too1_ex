---
name: filestat
description: Inspect local files with a small native C CLI. Use it to get file size, modification time, readability, and a fast FNV-1a content hash.
---

# Filestat

Use the `filestat` command-line tool to inspect a local file before reading,
copying, comparing, or processing it.

## When to use this skill

- Before reading a potentially large file, check its size.
- Before comparing two files, calculate a fast content hash.
- Before automating a file-based workflow, verify that the target file exists,
  is readable, and is a regular file.

## Tool location

The executable is installed at:

```bash
~/.hermes/skills/filestat/filestat
```

## Commands

### Get file summary

```bash
~/.hermes/skills/filestat/filestat summary <absolute-or-relative-file-path>
```

Returns:

- `path`
- `size_bytes`
- `modified`
- `readable`
- `is_regular_file`

Example:

```bash
~/.hermes/skills/filestat/filestat summary ./report.csv
```

### Calculate FNV-1a 64-bit hash

```bash
~/.hermes/skills/filestat/filestat hash <absolute-or-relative-file-path>
```

Returns:

- `path`
- `fnv1a64`

Example:

```bash
~/.hermes/skills/filestat/filestat hash ./report.csv
```

## Workflow

1. Always use the actual file path supplied by the user or discovered in context.
2. Run `summary` first when file existence, readability, or size matters.
3. Use `hash` when comparing files or detecting whether a file has changed.
4. If the command exits with status `2`, report the error clearly and do not
   attempt to read or process the file.
5. Do not infer exact file equality from the hash alone; FNV-1a is fast but not
   cryptographically secure. For high-confidence comparison, also compare file
   sizes and, if necessary, use a stronger hash tool.

## Notes

- This tool performs no network access.
- It is intended for local file inspection only.
- Prefer absolute paths when the working directory may change.