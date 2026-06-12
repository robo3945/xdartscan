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

# Static release build for Windows/MinGW (links libgcc/libpthread statically)
# Uncomment CMAKE_EXE_LINKER_FLAGS in CMakeLists.txt first
cmake -B cmake-build-release-static-windows -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release-static-windows

# Quick compile without CMake (links pthread on Linux; omit -lpthread on macOS)
cc -std=c99 -Wall -Wextra -o /tmp/xdartscan main.c logic/scan_engine.c logic/random_test.c logic/config_manager.c logic/report_manager.c misc/utils.c -lm -lpthread

# Run all tests
cd cmake-build-debug && ctest -V

# Run a single test by name
cd cmake-build-debug && ctest -V -R test_signatures

# Run the scanner (with optional JSON report)
./cmake-build-debug/xdartscan -i <dir_to_scan> -c config.ini -v -t 8 -j report.json

# Delete all report*.tsv and stats*.txt files in the current directory
./cmake-build-debug/xdartscan -clean
```

### Tests

- `test_signatures` — smoke-tests `sort_signatures()` on `g_well_known_mn[]` (verifies qsort comparator doesn't crash/corrupt)
- `test_entropy` — unit-tests `calc_rand_idx()` entropy calculation with known inputs

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

**Hot path**: `p_scan_files()` → `wq_push()` → worker `p_scan_file()` → read 4 bytes for magic → `p_binary_search()` → if unrecognized: read up to 64KB for entropy → `append_line_to_report()`.

## Key Design Decisions

- **Two-phase read**: phase 1 reads only 4 bytes for magic number extraction (bit-shift, no string parsing). If a known signature matches, the file is closed immediately — no entropy I/O. Phase 2 (only for unrecognized files) `malloc`s a 64KB buffer (`SCAN_READ_BUF_SIZE`) and reads up to `MAX_FILE_SIZE` bytes for entropy. This avoids heap allocation and I/O for the majority of recognized files.
- **Stack-allocated paths**: `normalize_path` and path construction use stack buffers (`MAX_PATH_BUFFER = 2048`) to avoid malloc/free in the recursive scan loop.
- **`d_type` fast-path**: on systems with `_DIRENT_HAVE_D_TYPE`, file vs directory is resolved without `stat()`.
- **Signatures**: 397 entries in `g_well_known_mn[]` (file_signatures.h), sorted at startup by `qsort`, searched by `p_binary_search()`. Matching tries 4-byte then 3-byte prefixes.
- **Entropy cap**: files larger than `MAX_FILE_SIZE` (config.ini, default 10MB) are only partially read for entropy.
- **Multithreaded scanning**: producer-consumer pattern with a bounded ring-buffer (`WORK_QUEUE_CAP=512`). `p_scan_files()` is the sole producer; `NUM_THREADS` worker threads call `p_scan_file()`. Stats updates and report writes are each protected by a dedicated mutex (`g_stats_mutex`, `g_report_mutex`). `-t <n>` CLI flag overrides `NUM_THREADS` from config.ini.
- **Thread safety**: `ctime_r()` with separate buffers for mtime/ctime/atime timestamps.

## Configuration (config.ini)

`ENTROPY_TH` (default 7.99), `MIN_FILE_SIZE` (4000), `MAX_FILE_SIZE` (10000000), `DEBUG_PRINT`, `THROUGHPUT_TEST`, `NUM_THREADS` (default 4) — all parsed as key=value pairs.

Note: `ENTROPY_TH` uses a comma as decimal separator in config.ini (`7,99`) — locale-dependent. `main.c` calls `setlocale(LC_NUMERIC, ".OCP")` (Windows OCP) so `strtod`/`sscanf` parse the comma correctly. The CLI `-t` flag takes precedence over `NUM_THREADS` from config.ini; this is enforced via `set_num_threads_from_cli()` which sets a flag that `read_config_file()` checks before overwriting.

## Worklog

Before starting modifications, read `WORKLOG.md` for context on past changes. After completing modifications, append a new dated entry to `WORKLOG.md` documenting what was changed and why.
