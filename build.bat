@echo off
REM ---------------------------------------------------------------------------
REM XDartScan build script — Windows (Visual Studio / MSVC)
REM
REM Usage:
REM   build.bat              Release build (default)
REM   build.bat debug        Debug build
REM   build.bat release      Release build (explicit)
REM   build.bat clean        Remove all build directories
REM   build.bat test         Build + run tests
REM
REM Requirements: CMake and a C compiler (MSVC or MinGW) must be in PATH.
REM   For MSVC, run from a "Developer Command Prompt" or "x64 Native Tools".
REM ---------------------------------------------------------------------------

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=release

if "%BUILD_TYPE%"=="clean" goto :clean
if "%BUILD_TYPE%"=="test"  goto :test
if "%BUILD_TYPE%"=="debug" goto :build
if "%BUILD_TYPE%"=="release" goto :build

echo Usage: build.bat [debug^|release^|test^|clean]
exit /b 1

:build
set CMAKE_BUILD_TYPE=Release
set BUILD_DIR=cmake-build-release

if "%BUILD_TYPE%"=="debug" (
    set CMAKE_BUILD_TYPE=Debug
    set BUILD_DIR=cmake-build-debug
)

echo ==^> Configuring (%CMAKE_BUILD_TYPE%) ...
cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    exit /b 1
)

echo ==^> Building ...
cmake --build %BUILD_DIR% --config %CMAKE_BUILD_TYPE%
if errorlevel 1 (
    echo ERROR: Build failed.
    exit /b 1
)

echo.
echo ==^> Build successful: %BUILD_DIR%\xdartscan.exe
echo     Run with: %BUILD_DIR%\xdartscan.exe -i ^<dir^> -c config.ini -v
exit /b 0

:test
set BUILD_TYPE=release
if not "%2"=="" set BUILD_TYPE=%2
call :build
if errorlevel 1 exit /b 1

echo.
echo ==^> Running tests ...
cd %BUILD_DIR% && ctest -V -C %CMAKE_BUILD_TYPE%
exit /b %errorlevel%

:clean
echo ==^> Cleaning build directories ...
if exist cmake-build-debug          rmdir /s /q cmake-build-debug
if exist cmake-build-release        rmdir /s /q cmake-build-release
if exist cmake-build-release-static-windows rmdir /s /q cmake-build-release-static-windows
echo ==^> Clean done.
exit /b 0
