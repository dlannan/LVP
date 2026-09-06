@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

set CONFIG=%~1
if "%CONFIG%"=="" set CONFIG=Release

if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" (
    echo Usage: build.bat [Debug^|Release]
    exit /b 1
)

set ROOT=%~dp0
set OUT=%ROOT%..\lib\%CONFIG%

if "%LIBUV_ROOT%"=="" (
    set LIBUV_ROOT=%ROOT%..\
)

if not exist "%OUT%" mkdir "%OUT%"

echo.
echo ========================================
echo Building packet_interface.lib - %CONFIG%
echo ========================================
echo.
echo libuv:
echo   %LIBUV_ROOT%
echo.

if /I "%CONFIG%"=="Debug" (
    set CFLAGS=/std:c++20 /EHsc /MDd /W4 /Zi
    set UVLIB=%LIBUV_ROOT%\lib\%CONFIG%
) else (
    set CFLAGS=/std:c++20 /EHsc /MD /W4 /O2
    set UVLIB=%LIBUV_ROOT%\lib\%CONFIG%
)

cl %CFLAGS% /c /nologo ^
    /I"%ROOT%include" ^
    /I"%ROOT%..\module\include" ^
    /I"%LIBUV_ROOT%\include" ^
    "%ROOT%src\packet_interface.cpp"

if errorlevel 1 exit /b 1

lib /nologo ^
    /OUT:"%OUT%\packet_interface.lib" ^
    packet_interface.obj

if errorlevel 1 exit /b 1

del /q packet_interface.obj 2>nul

echo.
echo Built:
echo   %OUT%\packet_interface.lib
echo.

endlocal
