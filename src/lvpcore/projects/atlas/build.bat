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
set OUT=%ROOT%bin\%CONFIG%

if not exist "%OUT%" mkdir "%OUT%"

echo.
echo ========================================
echo Building module.lib - %CONFIG%
echo ========================================
echo.

if /I "%CONFIG%"=="Debug" (
    set CFLAGS=/std:c++20 /EHsc /MDd /W4 /Zi
) else (
    set CFLAGS=/std:c++20 /EHsc /MD /W4 /O2
)

if /I "%CONFIG%"=="Debug" (
    set LINK_DEBUG=/DEBUG
) 

set LINK=%LINK_DEBUG% ^
    "..\..\common\lib\%CONFIG%\module.lib" ^
    "..\..\common\lib\%CONFIG%\packet_interface.lib" ^
    "..\..\common\lib\%CONFIG%\libuv.lib" ^
    "User32.lib" ^
    "Userenv.lib" ^
    "Ole32.lib" ^
    "OneCore.Lib" ^
    "Advapi32.lib" ^
    "Iphlpapi.lib" ^
    "ws2_32.lib" ^
    "Dbghelp.lib" ^
    "Synchronization.lib"    

cl %CFLAGS% /nologo ^
    /I"..\..\common\include" ^
    /I"%ROOT%src" ^
    "%ROOT%src\atlas.cpp" ^
    "%ROOT%src\main.cpp" ^
    /Fe:%OUT%\atlas.exe ^
    %LINK%

if not exist bin\Debug mkdir bin\Debug

if errorlevel 1 exit /b 1

del /q atlas.obj 2>nul
del /q main.obj 2>nul

echo.
echo Built:
echo   %OUT%\atlas.exe
echo.

endlocal
exit
