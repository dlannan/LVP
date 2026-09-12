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

REM This is kinda bad, but I will add some ways to handle this better. Prob move to luamake
del /q *.obj 2>nul

echo.
echo ========================================
echo Building editor.exe - %CONFIG%
echo ========================================
echo.

if /I "%CONFIG%"=="Debug" (
    set CFLAGS=/std:c++20 /EHsc /MDd /W4 /Zi /D_CONSOLE
) else (
    set CFLAGS=/std:c++20 /EHsc /MD /W4 /O2 /D_CONSOLE
)

if /I "%CONFIG%"=="Debug" (
    set LINK_DEBUG=/DEBUG
) 

set LINK=%LINK_DEBUG% ^
    "..\..\lvpcore\common\lib\%CONFIG%\state.lib" ^
    "..\..\lvpcore\common\lib\%CONFIG%\packet_interface.lib" ^
    "..\..\lvpcore\common\lib\%CONFIG%\libuv.lib" ^
    "%ROOT%..\..\..\external\DXSDK\lib\x64\d3dx11.lib" ^
    "User32.lib" ^
    "Userenv.lib" ^
    "Ole32.lib" ^
    "OneCore.Lib" ^
    "D3D11.lib" ^
    "Advapi32.lib" ^
    "Iphlpapi.lib" ^
    "ws2_32.lib" ^
    "Dbghelp.lib" ^
    "Synchronization.lib"  

cl %CFLAGS% /nologo ^
    /I"%ROOT%..\..\..\external\imgui" ^
    /I"%ROOT%..\..\..\external\stb_image" ^
    /I"%ROOT%..\..\..\external\DXSDK\include" ^
    /I"..\..\lvpcore\common\include" ^
    /I"%ROOT%src/gui/" ^
    /I"%ROOT%src/gui/imgui" ^
    /I"%ROOT%src/gui/imgui/imgui_node_editor" ^
    /I"%ROOT%src" ^
    "%ROOT%..\..\..\external\imgui\imgui.cpp" ^
    "%ROOT%..\..\..\external\imgui\imgui_tables.cpp" ^
    "%ROOT%..\..\..\external\imgui\imgui_widgets.cpp" ^
    "%ROOT%..\..\..\external\imgui\imgui_draw.cpp" ^
    "%ROOT%src\gui\imgui\imgui_impl_dx11.cpp" ^
    "%ROOT%src\gui\imgui\imgui_impl_win32.cpp" ^
    "%ROOT%src\gui\imgui\platform_win32.cpp" ^
    "%ROOT%src\gui\imgui\renderer_dx11.cpp" ^
    "%ROOT%src\gui\imgui\application.cpp" ^
    "%ROOT%src\gui\imgui\imgui_node_editor\crude_json.cpp" ^
    "%ROOT%src\gui\imgui\imgui_node_editor\imgui_canvas.cpp" ^
    "%ROOT%src\gui\imgui\imgui_node_editor\imgui_node_editor.cpp" ^
    "%ROOT%src\gui\imgui\imgui_node_editor\imgui_node_editor_api.cpp" ^
    "%ROOT%src\gui\imgui_editor.cpp" ^
    "%ROOT%src\states\stateRegister.cpp" ^
    "%ROOT%src\states\stateMain.cpp" ^
    "%ROOT%src\main.cpp" ^
    /Fe:"%OUT%\editor.exe" ^
    %LINK%

if errorlevel 1 exit /b 1

del /q *.obj 2>nul

echo.
echo Built:
echo   %OUT%\editor.exe
echo.

endlocal
exit
