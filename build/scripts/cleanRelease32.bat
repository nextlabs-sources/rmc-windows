@ECHO OFF

SETLOCAL

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

cd ..

devenv.com all.sln /Clean "Release_MT|x86"
IF ERRORLEVEL 1 GOTO END
devenv.com all.sln /Clean "Release|x86"
IF ERRORLEVEL 1 GOTO END

IF EXIST output\x86_release_mt rd /s /q output\x86_release_mt
IF EXIST output\x86_release rd /s /q output\x86_release

cd scripts

:END
ENDLOCAL
