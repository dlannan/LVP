@echo off
setlocal

REM ============================================================
REM Build libuv
REM
REM Usage:
REM   build_libuv.bat
REM   build_libuv.bat Release
REM   build_libuv.bat Debug
REM ============================================================

set "CONFIG=%~1"

if "%CONFIG%"=="" set "CONFIG=Release"

if /I not "%CONFIG%"=="Release" if /I not "%CONFIG%"=="Debug" (
    echo ERROR: Configuration must be Debug or Release.
    echo.
    echo Usage:
    echo   build_libuv.bat [Debug^|Release]
    exit /b 1
)

echo ========================================
echo Building libuv - %CONFIG% x64
echo ========================================

call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

if errorlevel 1 (
    echo Failed to initialise Visual Studio environment.
    exit /b 1
)

echo.
echo Cleaning previous build...

REM if exist build (
REM    rmdir /S /Q build
REM )

if exist install (
    rmdir /S /Q install
)

echo.
echo Configuring CMake...

cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DLIBUV_BUILD_SHARED=OFF -DLIBUV_BUILD_TESTS=OFF
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

echo.
echo Building %CONFIG%...

cmake --build build --config %CONFIG%

if errorlevel 1 (
    echo libuv build failed.
    exit /b 1
)

echo.
echo Installing %CONFIG%...

cmake --install build --config %CONFIG% --prefix "%CD%\install"

if errorlevel 1 (
    echo libuv install failed.
    exit /b 1
)

echo.
echo ========================================
echo libuv build complete.
echo Configuration: %CONFIG%
echo Installed to:
echo %CD%\install
echo ========================================

endlocal
pause
