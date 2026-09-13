@echo off
setlocal

set "BUILD=%~1"
if "%BUILD%"=="" set "BUILD=Debug"

pushd "%~dp0dummymodule"
call "build.bat"  "%BUILD%"
popd

pushd "%~dp0atlas"
call "build.bat"  "%BUILD%"
popd

endlocal
