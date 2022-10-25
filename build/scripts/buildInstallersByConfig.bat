@ECHO ON

SETLOCAL

IF "%1"=="" GOTO USAGE

IF "%1"=="Debug" (
  set CONFIG=Debug
  set CONFIG_LOWERCASE=debug
  set ISM_FILENAME_SUFFIX=_Debug
) ELSE IF "%1"=="Release" (
  set CONFIG=Release
  set CONFIG_LOWERCASE=release
  set ISM_FILENAME_SUFFIX=
) ELSE (
  GOTO USAGE
)

CALL setVersion.bat

IF "%BUILD_NUMBER%"=="" (
  set buildNumber=0
) ELSE (
  set buildNumber=%BUILD_NUMBER%
)

set version="%VERSION_MAJMIN%.%buildNumber%"

CALL c:\windows\syswow64\cscript ISAutoGUIDVersion.js ..\..\install\RightsManagementClient_x86%ISM_FILENAME_SUFFIX%.ism %version%
CALL c:\windows\syswow64\cscript ISAutoGUIDVersion.js ..\..\install\RightsManagementClient_x64%ISM_FILENAME_SUFFIX%.ism %version%

xcopy /s /k /i /y ..\output\x86_%CONFIG_LOWERCASE%_mt\*.dll ..\..\install\build\data\x86_%CONFIG_LOWERCASE%
IF ERRORLEVEL 1 GOTO :EOF
xcopy /s /k /i /y ..\output\x86_%CONFIG_LOWERCASE%_mt\*.exe ..\..\install\build\data\x86_%CONFIG_LOWERCASE%
IF ERRORLEVEL 1 GOTO :EOF
xcopy /s /k /i /y ..\output\x64_%CONFIG_LOWERCASE%_mt\*.dll ..\..\install\build\data\x64_%CONFIG_LOWERCASE%
IF ERRORLEVEL 1 GOTO :EOF
xcopy /s /k /i /y ..\output\x64_%CONFIG_LOWERCASE%_mt\*.exe ..\..\install\build\data\x64_%CONFIG_LOWERCASE%
IF ERRORLEVEL 1 GOTO :EOF

xcopy /s /y ..\..\install\themes "C:\Program Files (x86)\InstallShield\2014 SAB\Support\Themes"
IF ERRORLEVEL 1 GOTO :EOF

CALL build%CONFIG%32Installer.bat
IF ERRORLEVEL 1 GOTO :EOF

CALL build%CONFIG%64Installer.bat
IF ERRORLEVEL 1 GOTO :EOF

GOTO :EOF



:USAGE
echo Usage: %0 config
echo config can be either "Debug" or "Release"
