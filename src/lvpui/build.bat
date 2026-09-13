@echo off
setlocal

set "BUILD=%~1"
if "%BUILD%"=="" set "BUILD=Debug"

pushd "%~dp0editor"
call "build.bat"  "%BUILD%"
popd

pushd "%~dp0live"
call "build.bat"  "%BUILD%"
popd

endlocal
