# XDartScan - Worklog

## 2026-04-07 - Two-phase read optimization

### Goal
Optimize file scanning by avoiding eager reading and memory allocation for files with known magic numbers.

### Changes
#### `logic/scan_engine.c`
- **Two-phase read**: Changed `p_scan_file()` to initially read only the first 4 bytes for magic number extraction. If a known magic number is found, the file is immediately closed, avoiding unnecessary I/O and memory allocation for the rest of the file.
- **Lazy memory allocation**: The buffer for entropy calculation is now dynamically allocated only if the file is deemed suspicious (i.e. its magic number is not recognized). This dramatically reduces heap contention and I/O in a multi-threaded context.

---

## 2026-03-24 - Multithreaded file scanning (producer-consumer thread pool)

### Goal
Parallelise file scanning to exploit multi-core CPUs, reducing elapsed time on large directory trees.

### Architecture

Producer-consumer pattern with a bounded ring-buffer work queue:

```
main_scan()
  ├─ Initialise work queue (WORK_QUEUE_CAP=512 items)
  ├─ Launch NUM_THREADS worker threads
  │
  ├─ p_scan_files() [producer, single thread]
  │   └─ Traverses directories; for each regular file → wq_push(path, size)
  │
  ├─ Signal producer_done → workers drain queue and exit
  └─ pthread_join() all workers → make_stats() → close_file()
```

Each worker thread calls `p_scan_file()`, which now consolidates all `g_stats` updates into **one** locked critical section at the `report:` label, and wraps `append_line_to_report()` in a separate `g_report_mutex` lock.

### Files changed

#### `logic/scan_engine.c`
- Added `#include <pthread.h>` and `WorkItem`/`WorkQueue` types.
- Added `g_stats_mutex`, `g_report_mutex` (static, PTHREAD_MUTEX_INITIALIZER), `g_wq`, `g_verbose_flag` globals.
- `main_scan()`: initialises/destroys queue, spawns/joins `NUM_THREADS` workers.
- `p_scan_files()`: removed `g_stats.num_files++`; replaced `p_scan_file()` calls with `wq_push()`.
- `p_scan_file()`: removed all inline `g_stats.xxx++` from the function body; added single locked stats block + locked report write at `report:` label.
- New `wq_push()`: blocks when queue full (`pthread_cond_wait not_full`).
- New `p_worker_thread()`: dequeues and calls `p_scan_file()`; exits when queue empty and `producer_done`.

#### `headers/config.h`
- Added `extern int NUM_THREADS;`.

#### `logic/config_manager.c`
- Added `int NUM_THREADS = 4;` default.
- Parses `NUM_THREADS` from config.ini (minimum 1).

#### `misc/utils.c`
- `make_stats()`: added `NUM_THREADS` line in CONFIG section of stats output.

#### `config.ini`
- Added `NUM_THREADS=4`.

#### `CMakeLists.txt`
- Added `target_link_libraries(xdartscan pthread)`.

---

## 2026-03-24 - Windows (MinGW) build fixes

### Goal
Fix compilation failures on Windows with MinGW introduced by recent changes.

### Changes

#### `main.c`
- **`glob.h` not available in MinGW**: replaced unconditional `#include <glob.h>` with `#ifdef _WIN32 / #include <windows.h> / #else / #include <glob.h> / #endif`.
- **`clean_reports()`**: reimplemented with `#ifdef _WIN32` branch using `FindFirstFileA`/`FindNextFileA`/`FindClose` (Win32 API), and `#else` branch keeping the existing `glob()`/`globfree()` POSIX implementation.

#### `misc/utils.c`
- **Removed unused `#include <dirent.h>`**: the header was included but none of the functions in `utils.c` use `dirent` types or functions. Removing it avoids unnecessary POSIX dependency.

#### `logic/scan_engine.c`
- **Added `_POSIX_C_SOURCE` define**: `ctime_r()` in MinGW's `time.h` is guarded by `#if defined(_POSIX_C_SOURCE)`. Without this define, compiling with `-std=gnu99` causes an implicit declaration warning. Added `#ifndef _POSIX_C_SOURCE / #define _POSIX_C_SOURCE 200112L / #endif` before the includes.

### Root cause
The commit `da8ffac "Add -clean CLI option"` introduced `#include <glob.h>` which is a POSIX header absent from MinGW's headers. This caused a build failure on Windows for all translation units that transitively pulled in affected headers.

---

## 2026-03-21 - Performance optimizations and bug fixes

### Goal
Optimize scanning performance by reducing I/O overhead, memory allocations, and system calls.

### Changes

#### `logic/scan_engine.c`
- **Single-read optimization**: eliminated the double read (magic number + full content). A single `fread` now reads the entire buffer, from which both the first 4 bytes for the magic number and the data for entropy calculation are extracted.
- **Magic number conversion via bit-shift**: replaced `strtoul()` (hex string parsing) with direct arithmetic shifting from the already-read bytes. Avoids string conversion for every file.
- **Stack-allocated path normalization**: `normalize_path()` previously allocated with `malloc` on every recursive call. The path is now normalized into a local stack buffer, eliminating malloc/free in the hot path.
- **Reduced string copies**: removed concatenated `strncpy` + `strcat`, replaced with a single `snprintf` to build the path.
- **`d_type` fast-path**: when available (`_DIRENT_HAVE_D_TYPE`), `dp->d_type` is used to distinguish files from directories without unnecessary `stat()` calls.
- **Efficient `.` and `..` skip**: direct character check instead of `strncmp`.
- **Entropy read cap**: entropy reads are capped at `MAX_FILE_SIZE` bytes, preventing huge files from being loaded entirely into memory.
- **Removed `p_read_magic_number` function**: no longer needed since the magic number is extracted from the already-read buffer.
- **`sprintf` to `snprintf`**: for safety against buffer overflows in report line formatting.

#### `logic/config_manager.c`
- **`qsort` instead of selection sort**: signature sorting goes from O(n^2) to O(n log n).
- **`strcmp` instead of `strncmp`**: for config parameters where max length is not needed.

#### `logic/report_manager.c`
- **64KB I/O buffer**: added `setvbuf(*fp, NULL, _IOFBF, 64 * 1024)` on report files to reduce the number of write syscalls.
- **TSV header as constant string**: replaced dynamic `sprintf` construction with a `static const` string.
- **Null parameter checks**: added early validation in `create_report_file`.

#### `misc/utils.c`
- **`malloc` instead of `calloc`** in `read_file_content`: the buffer is immediately overwritten by `fread`, so zero-initialization with `calloc` was wasted work.
- **`format_size`: `units` array made `static`**: avoids re-initialization on every call.
- **Optimized `strnstr`**: added early-return if the needle is empty or longer than the haystack.
- **Input validation**: added null checks in `format_size`, `read_file_content`, `itoa`.

#### `logic/random_test.c`
- Extended documentation for the `calc_rand_idx` function.

#### `.gitignore`
- Added `.iac-data` and `.DS_Store`.

### Bug fix
- **`ctime()` buffer overwrite**: `ctime()` returns a pointer to a shared static buffer. Three consecutive calls for mtime/ctime/atime were overwriting the same buffer, resulting in identical timestamps. Fixed by using `ctime_r()` with separate buffers for each timestamp.