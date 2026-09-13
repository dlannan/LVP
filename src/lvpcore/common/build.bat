@echo off
setlocal

set "BUILD=%~1"
if "%BUILD%"=="" set "BUILD=Debug"

pushd "%~dp0packet_interface"
call "build.bat"  "%BUILD%"
popd

pushd "%~dp0state"
call "build.bat"  "%BUILD%"
popd

endlocal
