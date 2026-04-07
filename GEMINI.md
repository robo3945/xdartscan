# XDartScan - Project Context

## Project Overview
XDartScan is a C99-based scanner designed to detect encrypted or suspicious files (e.g., private keys, encrypted volumes, ransomware activity). It achieves this by:
- Analyzing Shannon entropy to identify highly randomized/encrypted data.
- Matching the first 4 bytes of files against a database of known magic numbers (file signatures) using binary search.
- Operating recursively on directories.
- Producing detailed reports in TSV and text formats.

The architecture relies on an optimized, multithreaded (producer-consumer) scanning engine that minimizes I/O overhead by extracting magic numbers and calculating entropy in a single pass.

## Building and Testing
The project uses CMake for build configuration.

**Build (Debug):**
```bash
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug && cmake --build cmake-build-debug
```

**Build (Release):**
```bash
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release && cmake --build cmake-build-release
```

**Run Tests:**
```bash
cd cmake-build-debug && ctest -V
```

**Run Scanner:**
```bash
./cmake-build-debug/xdartscan -i <dir_to_scan> -c config.ini -v -t <threads>
```

## Development Conventions
- **Memory Management:** Prefer stack-allocated buffers for paths (e.g., `MAX_PATH_BUFFER = 2048`) to avoid `malloc`/`free` overhead in recursive loops.
- **I/O Optimization:** Avoid double I/O. Utilize single-read optimization (e.g., one `fread` for both entropy and magic number extraction).
- **Concurrency:** The application uses a multithreaded producer-consumer pattern. Ensure thread safety when updating stats or writing reports by using appropriate mutexes (`g_stats_mutex`, `g_report_mutex`).
- **File System:** Use the `d_type` fast-path for resolving file types (file vs. directory) without `stat()` on supported systems.
- **Documentation:** Before starting modifications, review `WORKLOG.md` for context. After completing modifications, append a new dated entry to `WORKLOG.md` documenting what was changed and why.