#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# XDartScan build script — Linux / macOS / Windows (MinGW / MSYS2)
#
# Usage:
#   ./build.sh              # Release build (default)
#   ./build.sh debug        # Debug build
#   ./build.sh release      # Release build (explicit)
#   ./build.sh clean        # Remove all build directories
#   ./build.sh test         # Build + run tests
# ---------------------------------------------------------------------------
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# ── Detect platform ────────────────────────────────────────────────────────
detect_platform() {
    case "$(uname -s)" in
        Linux*)   PLATFORM="linux"   ;;
        Darwin*)  PLATFORM="macos"   ;;
        MINGW*|MSYS*|CYGWIN*) PLATFORM="windows" ;;
        *)        PLATFORM="unknown" ;;
    esac
    echo "Platform detected: $PLATFORM"
}

# ── Build configuration ───────────────────────────────────────────────────
BUILD_TYPE="${1:-release}"

do_build() {
    local cmake_build_type="Release"
    local build_dir="cmake-build-release"

    if [ "$BUILD_TYPE" = "debug" ]; then
        cmake_build_type="Debug"
        build_dir="cmake-build-debug"
    fi

    echo "==> Configuring ($cmake_build_type) ..."

    local cmake_extra_args=()

    case "$PLATFORM" in
        windows)
            # Use MinGW Makefiles on Windows (MSYS2/MinGW environment)
            cmake_extra_args+=(-G "MinGW Makefiles")
            cmake_extra_args+=(-DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static")
            ;;
        linux)
            # Link statically on Linux for portable binaries
            cmake_extra_args+=(-DCMAKE_EXE_LINKER_FLAGS="-static-libgcc")
            ;;
        macos)
            # Default generator (Unix Makefiles / Xcode) — no special flags needed
            ;;
    esac

    cmake -B "$build_dir" \
          -DCMAKE_BUILD_TYPE="$cmake_build_type" \
          "${cmake_extra_args[@]+"${cmake_extra_args[@]}"}"

    echo "==> Building ..."
    cmake --build "$build_dir" --parallel "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"

    echo ""
    echo "==> Build successful: $build_dir/xdartscan"
    echo "    Run with: ./$build_dir/xdartscan -i <dir> -c config.ini -v"
}

do_test() {
    BUILD_TYPE="${2:-release}"
    do_build

    local build_dir="cmake-build-release"
    [ "$BUILD_TYPE" = "debug" ] && build_dir="cmake-build-debug"

    echo ""
    echo "==> Running tests ..."
    cd "$build_dir" && ctest -V
}

do_clean() {
    echo "==> Cleaning build directories ..."
    rm -rf cmake-build-debug cmake-build-release cmake-build-release-static-windows
    echo "==> Clean done."
}

# ── Main ──────────────────────────────────────────────────────────────────
detect_platform

case "$BUILD_TYPE" in
    debug|release)  do_build ;;
    test)           do_test "$@" ;;
    clean)          do_clean ;;
    *)
        echo "Usage: $0 [debug|release|test|clean]"
        exit 1
        ;;
esac
