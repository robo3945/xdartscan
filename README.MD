## XDartScan ##
[![License](https://img.shields.io/badge/license-GPL3-green)](https://github.com/robo3945/xdartscanner/blob/master/LICENSE)
[![Build](https://img.shields.io/badge/build-1.2-yellowgreen)]()
[![Language](https://img.shields.io/badge/language-C99-blue)]()

XDartScan is a fast, multithreaded scanner that detects encrypted or suspicious
files by combining Shannon entropy analysis with magic number recognition.
It can find private keys (PGP, RSA), encrypted volumes, and files potentially
altered by ransomware activity.

Every run produces a timestamped TSV report and a stats summary. An optional
JSON report with the list of suspect files can also be generated.

---

## Features

- **Entropy-based detection** — flags files with Shannon entropy above a
  configurable threshold (default 7.99 bits/byte)
- **Magic number recognition** — 397 known file signatures searched with
  binary search; files matching a known signature are excluded from the
  suspect list even if their entropy is high
- **Multithreaded scanning** — producer/consumer thread pool (configurable
  via CLI or config file)
- **Recursive directory traversal** with `d_type` fast-path (no extra
  `stat()` call per entry on supported filesystems)
- **Three output formats** — TSV report, plain-text stats, optional JSON
- **Report cleanup** — single command to remove all generated report files

---

## Algorithms

### Shannon Entropy

Each file whose magic number is not recognised is read into memory (up to
`MAX_FILE_SIZE` bytes) and its entropy is computed:

```
H = -Σ p(i) · log₂(p(i))    for i in 0..255
```

The implementation keeps a 256-bucket frequency table, iterates the byte
buffer once, then applies the formula. The maximum possible value is 8 bits
(perfectly random byte stream). Files with `H > ENTROPY_TH` are flagged as
suspect.

### Magic Number Matching

At startup the signature array (`g_well_known_mn`, 397 entries) is sorted in
place by `qsort` on the numeric value of the first 4 bytes. Per-file lookup
runs two passes of binary search:

1. Full 4-byte prefix (`magic_number >> 0`)
2. 3-byte prefix (`magic_number >> 8`) as fallback for shorter signatures

A match on either pass marks the file as a known format and skips entropy
analysis.

### Two-Phase File Read

Only files that survive magic-number matching incur a full read. The first
`fread` of 4 bytes feeds both magic extraction and the start of the entropy
buffer, so no byte is read twice. Files larger than `MAX_FILE_SIZE` are
partially read (entropy is computed on the first `MAX_FILE_SIZE` bytes).

---

## Data Structures

| Structure | Purpose |
|-----------|---------|
| `MagicNumber` | One entry in the signature table. Fields: `number8_s` (hex string), `number8_ul` (numeric key used for binary search), description string. |
| `GlobStat` | Global counters: total files, suspect files, files by category (high entropy, low entropy, known magic, size errors, I/O errors, etc.) and total bytes scanned. |
| `WorkItem` | Unit of work passed between producer and workers: stack-allocated path (`MAX_PATH_BUFFER = 2048`) and file size. |
| `WorkQueue` | Bounded ring buffer (`WORK_QUEUE_CAP = 512 slots`) with head/tail/count indices. Synchronised with one mutex and two condition variables (`not_empty`, `not_full`). A `producer_done` flag signals end-of-input to all workers. |

Two additional mutexes protect shared state across worker threads:

- `g_stats_mutex` — guards all writes to `GlobStat`
- `g_report_mutex` — serialises TSV/JSON appends

---

## Command-Line Options

```
Usage: xdartscan -i <dir_to_scan> -c <config_file_path> [-v] [-x] [-t <n>] [-j <file.json>]
```

| Option | Description |
|--------|-------------|
| `-i <dir>` | Directory to scan (required) |
| `-c <config_file_path>` | Path to `config.ini` (optional; falls back to `./config.ini` then `../config.ini`) |
| `-v` | Verbose mode — prints each file name and entropy/magic result |
| `-x` | Keep terminal open after execution (waits for RETURN) |
| `-t <n>` | Number of worker threads (overrides `NUM_THREADS` in config.ini) |
| `-j <file.json>` | Write a JSON report of suspect files to the specified path |
| `-clean` | Delete all `report*.tsv` and `stats*.txt` files in the current directory |
| `-h` | Print help and exit |

---

## Configuration (`config.ini`)

| Key | Default | Description |
|-----|---------|-------------|
| `ENTROPY_TH` | `7,99` | Entropy threshold (comma as decimal separator) |
| `MIN_FILE_SIZE` | `4000` | Files smaller than this (bytes) are skipped |
| `MAX_FILE_SIZE` | `10000000` | Entropy is computed on at most this many bytes |
| `NUM_THREADS` | `4` | Worker thread count (overridden by `-t`) |
| `DEBUG_PRINT` | `0` | Print per-file entropy debug info |
| `THROUGHPUT_TEST` | `0` | Skip entropy computation (benchmark mode) |

---

## Output

| File | Format | Contents |
|------|--------|---------|
| `report<N>.tsv` | TSV | Full path, filename, extension, suspect flag, entropy (H), magic found flag, magic hex, error flags, file size, ctime, atime, mtime |
| `stats<N>.txt` | Plain text | Scan summary: totals by category, elapsed time, throughput |
| `<file>.json` | JSON | List of suspect files with path, entropy, magic hex, size; plus scan metadata |

---

## Examples

```bash
# Scan a directory with 8 threads, verbose, write JSON report
xdartscan -i /mnt/data -c config.ini -v -t 8 -j /tmp/report.json

# Scan using default config locations (./config.ini or ../config.ini)
xdartscan -i /home/user/docs

# Remove all previously generated report and stats files
xdartscan -clean

# Keep the terminal window open (useful when launched from a GUI)
xdartscan -i C:\Users\user\Documents -c C:\xdartscan\config.ini -x
```

---

## Build

```bash
# Debug build
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug && cmake --build cmake-build-debug

# Release build
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release

# Static release build for Windows/MinGW (uncomment CMAKE_EXE_LINKER_FLAGS in CMakeLists.txt first)
cmake -B cmake-build-release-static-windows -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release-static-windows

# Quick single-command compile (macOS — omit -lpthread; Linux — keep it)
cc -std=c99 -Wall -Wextra -o xdartscan \
   main.c logic/scan_engine.c logic/random_test.c \
   logic/config_manager.c logic/report_manager.c misc/utils.c \
   -lm
```
