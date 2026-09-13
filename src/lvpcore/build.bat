@echo off
setlocal

set "BUILD=%~1"
if "%BUILD%"=="" set "BUILD=Debug"

pushd "%~dp0common"
call "build.bat"  "%BUILD%"
popd

pushd "%~dp0projects"
call "build.bat"  "%BUILD%"
popd

REM pushd "%~dp0input_handlers"
REM call "build.bat"  "%BUILD%"
REM popd

endlocal
