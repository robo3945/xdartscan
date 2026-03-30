# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

XDartScan is a C99 scanner that detects encrypted/suspicious files by analyzing Shannon entropy and matching magic numbers (file signatures). It recursively scans directories, compares the first 4 bytes of each file against 397 known signatures via binary search, computes entropy, and outputs TSV + text reports.

## Build & Test

```bash
# Configure and build (Debug)
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug && cmake --build cmake-build-debug

# Configure and build (Release)
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release

# Quick compile without CMake (links pthread on Linux; omit -lpthread on macOS)
cc -std=c99 -Wall -Wextra -o /tmp/xdartscan main.c logic/scan_engine.c logic/random_test.c logic/config_manager.c logic/report_manager.c misc/utils.c -lm -lpthread

# Run all tests
cd cmake-build-debug && ctest -V

# Run a single test by name
cd cmake-build-debug && ctest -V -R test_signatures

# Run the scanner
./cmake-build-debug/xdartscan -i <dir_to_scan> -c config.ini -v -t 8

# Delete all report*.tsv and stats*.txt files in the current directory
./cmake-build-debug/xdartscan -clean
```

## Architecture

```
main.c                  → CLI parsing, orchestration
logic/scan_engine.c     → Recursive directory traversal, file analysis (magic number + entropy), report writing
logic/config_manager.c  → INI config parsing, signature array sorting (qsort) and preprocessing
logic/report_manager.c  → TSV/text report file creation with 64KB buffered I/O
logic/random_test.c     → Shannon entropy calculation (byte frequency → -Σ p·log₂p)
misc/utils.c            → File I/O helpers, string utils, formatting (size, time), stats generation
headers/                → All .h files; key types: MagicNumber (file_signatures.h), GlobStat (config.h)
```

**Hot path**: `p_scan_files()` → `p_scan_file()` → single `fread` for magic bytes + entropy buffer → `p_binary_search()` on sorted signatures → `append_line_to_report()`.

## Key Design Decisions

- **Single-read optimization**: one `fread` per file serves both magic number extraction (first 4 bytes via bit-shift) and entropy calculation — no double I/O.
- **Stack-allocated paths**: `normalize_path` and path construction use stack buffers (`MAX_PATH_BUFFER = 2048`) to avoid malloc/free in the recursive scan loop.
- **`d_type` fast-path**: on systems with `_DIRENT_HAVE_D_TYPE`, file vs directory is resolved without `stat()`.
- **Signatures**: 397 entries in `g_well_known_mn[]` (file_signatures.h), sorted at startup by `qsort`, searched by `p_binary_search()`. Matching tries 4-byte then 3-byte prefixes.
- **Entropy cap**: files larger than `MAX_FILE_SIZE` (config.ini, default 10MB) are only partially read for entropy.
- **Multithreaded scanning**: producer-consumer pattern with a bounded ring-buffer (`WORK_QUEUE_CAP=512`). `p_scan_files()` is the sole producer; `NUM_THREADS` worker threads call `p_scan_file()`. Stats updates and report writes are each protected by a dedicated mutex (`g_stats_mutex`, `g_report_mutex`). `-t <n>` CLI flag overrides `NUM_THREADS` from config.ini.
- **Thread safety**: `ctime_r()` with separate buffers for mtime/ctime/atime timestamps.

## Configuration (config.ini)

`ENTROPY_TH` (default 7.99), `MIN_FILE_SIZE` (4000), `MAX_FILE_SIZE` (10000000), `DEBUG_PRINT`, `THROUGHPUT_TEST`, `NUM_THREADS` (default 4) — all parsed as key=value pairs.

Note: `ENTROPY_TH` uses a comma as decimal separator in config.ini (`7,99`) — this is locale-dependent. The CLI `-t` flag takes precedence over `NUM_THREADS` from config.ini.

## Worklog

Before starting modifications, read `WORKLOG.md` for context on past changes. After completing modifications, append a new dated entry to `WORKLOG.md` documenting what was changed and why.
