@ECHO OFF

SETLOCAL

CALL setVersion.bat

CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"

cd ..

devenv.com all.sln /Build "Debug_MT|x64"
IF ERRORLEVEL 1 GOTO END
devenv.com all.sln /Build "Debug|x64"
IF ERRORLEVEL 1 GOTO END

cd output\x64_debug_mt
IF "%USE_EXTERNAL_SIGNING%" == "Yes" (
    CALL ..\..\scripts\signModulesByServer.bat
) ELSE IF "%USE_INTERNAL_SIGNING%" == "Yes" (
    CALL ..\..\scripts\signModules.bat
)
IF ERRORLEVEL 1 GOTO END
cd ..\..

cd output\x64_debug
IF "%USE_EXTERNAL_SIGNING%" == "Yes" (
    CALL ..\..\scripts\signModulesByServer.bat
) ELSE IF "%USE_INTERNAL_SIGNING%" == "Yes" (
    CALL ..\..\scripts\signModules.bat
)
IF ERRORLEVEL 1 GOTO END
cd ..\..

cd scripts

:END
ENDLOCAL
