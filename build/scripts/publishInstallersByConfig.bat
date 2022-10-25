REM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
REM % publishInstallersByConfig.bat
REM %
REM % Publish various installers for a configuration
REM %
REM % Usage: publishInstallersByConfig config
REM % config can be either "Debug" or "Release"
REM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

@ECHO ON

SETLOCAL

set PROD_DIR=SkyDRM

IF "%1"=="" GOTO USAGE

IF "%1"=="Debug" (
  set CONFIG=Debug
  set CFG_FILENAME_SUFFIX=_Debug
  set ZIP_FILENAME_SUFFIX=-debug
) ELSE IF "%1"=="Release" (
  set CONFIG=Release
  set CFG_FILENAME_SUFFIX=
  set ZIP_FILENAME_SUFFIX=
) ELSE (
  GOTO USAGE
)

CALL setVersion.bat

IF "%BUILD_NUMBER%"=="" (
  echo Error: BUILD_NUMBER is not set!
  GOTO :EOF
)

set S_DRIVE=\\nextlabs.com\share\data
set S_DRIVE_POSIX=//nextlabs.com/share/data
set VERSION_DIR=%S_DRIVE%\build\release_candidate\%PROD_DIR%\%VERSION_MAJMIN%
set VERSION_DIR_POSIX=%S_DRIVE_POSIX%/build/release_candidate/%PROD_DIR%/%VERSION_MAJMIN%
set INSTALLER_FILE=%VERSION_DIR_POSIX%/%BUILD_NUMBER%/SkyDRM-%VERSION_MAJMIN%.%BUILD_NUMBER%%ZIP_FILENAME_SUFFIX%.zip



cd ..\..\install



REM
REM Copy installers
REM

md temp_%CONFIG%\%CONFIG%_32bit
IF ERRORLEVEL 1 GOTO :EOF
copy build\output\%CONFIG%_32bit\Media_EXE\DiskImages\DISK1\setup.exe temp_%CONFIG%\%CONFIG%_32bit
IF ERRORLEVEL 1 GOTO :EOF
copy build\output\%CONFIG%_32bit\Media_EXE\DiskImages\DISK1\Setup.ini temp_%CONFIG%\%CONFIG%_32bit
IF ERRORLEVEL 1 GOTO :EOF
copy "build\output\%CONFIG%_32bit\Media_MSI\DiskImages\DISK1\SkyDRM.msi" temp_%CONFIG%\%CONFIG%_32bit
IF ERRORLEVEL 1 GOTO :EOF

md temp_%CONFIG%\%CONFIG%_64bit
IF ERRORLEVEL 1 GOTO :EOF
copy build\output\%CONFIG%_64bit\Media_EXE\DiskImages\DISK1\setup.exe temp_%CONFIG%\%CONFIG%_64bit
IF ERRORLEVEL 1 GOTO :EOF
copy build\output\%CONFIG%_64bit\Media_EXE\DiskImages\DISK1\Setup.ini temp_%CONFIG%\%CONFIG%_64bit
IF ERRORLEVEL 1 GOTO :EOF
copy "build\output\%CONFIG%_64bit\Media_MSI\DiskImages\DISK1\SkyDRM.msi" temp_%CONFIG%\%CONFIG%_64bit
IF ERRORLEVEL 1 GOTO :EOF



REM
REM Publish ZIP file for installers and tools
REM
set RES_PUBLISH_TO_BODA=false
if "%PUBLISH_TO_BODA%" == "Yes" set RES_PUBLISH_TO_BODA=true
if "%PUBLISH_TO_BODA%" == "yes" set RES_PUBLISH_TO_BODA=true
if "%PUBLISH_TO_BODA%" == "YES" set RES_PUBLISH_TO_BODA=true
if "%PUBLISH_TO_BODA%" == "1" set RES_PUBLISH_TO_BODA=true
if "%RES_PUBLISH_TO_BODA%" == "true" (
IF NOT EXIST %VERSION_DIR% md %VERSION_DIR%
IF ERRORLEVEL 1 GOTO :EOF
IF NOT EXIST %VERSION_DIR%\%BUILD_NUMBER% md %VERSION_DIR%\%BUILD_NUMBER%
IF ERRORLEVEL 1 GOTO :EOF
cd temp_%CONFIG%
zip -D -r -9 %INSTALLER_FILE% .
IF ERRORLEVEL 1 GOTO :EOF
echo INFO: Created %INSTALLER_FILE%
cd ..
)
rd /s /q temp_%CONFIG%



REM
REM Publish ZIP files CAB files
REM
CALL :PUBLISH_CAB_FILES
IF ERRORLEVEL 1 GOTO :EOF



cd ..\build\scripts



echo ********** Finished publishing %CONFIG% installers **********
GOTO :EOF



:USAGE
echo Usage: %0 config
echo config can be either "Debug" or "Release"
GOTO :EOF



REM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
REM % PUBLISH_CAB_FILES
REM %
REM % Publish CAB file packages
REM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

:PUBLISH_CAB_FILES

CALL :PUBLISH_CAB_FILE_BY_PLAT 32
IF ERRORLEVEL 1 GOTO :EOF
CALL :PUBLISH_CAB_FILE_BY_PLAT 64
IF ERRORLEVEL 1 GOTO :EOF

GOTO :EOF



REM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
REM % PUBLISH_CAB_FILE_BY_PLAT
REM %
REM % Publish CAB file package for one platform
REM %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

:PUBLISH_CAB_FILE_BY_PLAT

SETLOCAL

SET PLAT=%1

md temp_%CONFIG%_%PLAT%bit
IF ERRORLEVEL 1 GOTO :EOF
copy "build\output\%CONFIG%_%PLAT%bit\Media_MSI\DiskImages\Disk1\SkyDRM.msi" temp_%CONFIG%_%PLAT%bit
IF ERRORLEVEL 1 GOTO :EOF
copy ..\build\scripts\rmc.ddf temp_%CONFIG%_%PLAT%bit
IF ERRORLEVEL 1 GOTO :EOF
cd temp_%CONFIG%_%PLAT%bit
makecab /F rmc.ddf
IF ERRORLEVEL 1 GOTO :EOF
del "SkyDRM.msi" rmc.ddf setup.inf setup.rpt
ren RMC.CAB SkyDRM-AutoUpdate%PLAT%-%VERSION_MAJMIN%.%BUILD_NUMBER%%ZIP_FILENAME_SUFFIX%.CAB

CALL ..\..\build\scripts\signModulesByServer.bat
IF ERRORLEVEL 1 GOTO :EOF

..\..\source\tools\nxrmchecksumgen\src\Win32\Release\nxrmchecksumgen.exe SkyDRM-AutoUpdate%PLAT%-%VERSION_MAJMIN%.%BUILD_NUMBER%%ZIP_FILENAME_SUFFIX%.CAB > SkyDRM-AutoUpdate%PLAT%-checksum-%VERSION_MAJMIN%.%BUILD_NUMBER%%ZIP_FILENAME_SUFFIX%.txt
IF ERRORLEVEL 1 GOTO :EOF

IF "%RES_PUBLISH_TO_BODA%" == "true" (
  copy * %S_DRIVE%\build\release_candidate\%PROD_DIR%\%VERSION_MAJMIN%\%BUILD_NUMBER%
  IF ERRORLEVEL 1 GOTO :EOF
  echo INFO: Created %S_DRIVE%\build\release_candidate\%PROD_DIR%\%VERSION_MAJMIN%\%BUILD_NUMBER%\SkyDRM-AutoUpdate%PLAT%-%VERSION_MAJMIN%.%BUILD_NUMBER%%ZIP_FILENAME_SUFFIX%.CAB
)

cd ..
rd /s /q temp_%CONFIG%_%PLAT%bit

ENDLOCAL

GOTO :EOF
