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

if not exist "%OUT%" mkdir "%OUT%"

echo.
echo ========================================
echo Building state.lib - %CONFIG%
echo ========================================
echo.

if /I "%CONFIG%"=="Debug" (
    set CFLAGS=/std:c++20 /EHsc /MDd /W4 /Zi
) else (
    set CFLAGS=/std:c++20 /EHsc /MD /W4 /O2
)

cl %CFLAGS% /c /nologo ^
    /I"%ROOT%include" ^
    "%ROOT%src\state.cpp" ^
    "%ROOT%src\state_manager.cpp"

if errorlevel 1 exit /b 1

lib /nologo ^
    /OUT:"%OUT%\state.lib" ^
    state.obj ^
    state_manager.obj 

if errorlevel 1 exit /b 1

del /q state.obj 2>nul
del /q state_manager.obj 2>nul

echo.
echo Built:
echo   %OUT%\state.lib
echo.

endlocal
