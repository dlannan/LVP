@echo off
setlocal

set "BUILD=%~1"
if "%BUILD%"=="" set "BUILD=Debug"

pushd "%~dp0video_file"
call "build.bat"  "%BUILD%"
popd

endlocal
