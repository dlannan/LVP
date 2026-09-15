@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

set TARGET=video_file.exe
set RUNDIR=..\..\..\..

set CONFIG=%~1
if "%CONFIG%"=="" set CONFIG=Release

set CLEAN=%~2
if "%CLEAN%"=="" set CLEAN=NoClean

if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" (
    echo Usage: build.bat [Debug^|Release]
    exit /b 1
)

set ROOT=%~dp0
set OUT=%ROOT%%RUNDIR%\bin\modules\%CONFIG%
if not exist "%OUT%" mkdir "%OUT%"

if /I "%CLEAN%"=="Run" (
    pushd %ROOT%%RUNDIR%
    .\bin\modules\%CONFIG%\%TARGET% video_file 42010 dummy
    popd
    exit /b 2
)

if not exist "obj" mkdir "obj"

if /I "%CLEAN%"=="Clean" (
rmdir /s /q obj
mkdir obj
)

echo.
echo ========================================
echo Building %TARGET% - %CONFIG%
echo ========================================
echo.

if /I "%CONFIG%"=="Debug" (
    set CFLAGS=/std:c++20 /EHsc /MDd /W4 /Zi /wd4100 /Foobj\
) else (
    set CFLAGS=/std:c++20 /EHsc /MD /W4 /O2 /wd4100 /Foobj\
)

if /I "%CONFIG%"=="Debug" (
    set LINK_DEBUG=/DEBUG
) 

set LINK=%LINK_DEBUG% ^
    "..\..\common\lib\%CONFIG%\state.lib" ^
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
    "Synchronization.lib" ^
    "..\..\..\..\lib\libmpv\libmpv.dll.a"

cl %CFLAGS% /nologo ^
    /I"..\..\common\include" ^
    /I"..\..\..\..\lib\libmpv\include" ^
    /I"%ROOT%src" ^
    "%ROOT%src\mpv\VideoFrame.cpp" ^
    "%ROOT%src\mpv\FrameRingBuffer.cpp" ^
    "%ROOT%src\mpv\MpvVideoSource.cpp" ^
    "%ROOT%src\states\stateRegister.cpp" ^
    "%ROOT%src\states\stateVideoFileStream.cpp" ^
    "%ROOT%src\main.cpp" ^
    /Fe:"%OUT%\%TARGET%" ^
    %LINK%

if errorlevel 1 exit /b 1

echo.
echo Built:
echo   %OUT%\%TARGET%
echo.

endlocal
exit
