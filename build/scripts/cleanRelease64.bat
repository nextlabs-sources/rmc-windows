@ECHO OFF

SETLOCAL

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

cd ..

devenv.com all.sln /Clean "Release_MT|x64"
IF ERRORLEVEL 1 GOTO END
devenv.com all.sln /Clean "Release|x64"
IF ERRORLEVEL 1 GOTO END

IF EXIST output\x64_release_mt rd /s /q output\x64_release_mt
IF EXIST output\x64_release rd /s /q output\x64_release

cd scripts

:END
ENDLOCAL
