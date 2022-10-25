// nxrminstca.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <msi.h>
#include <msiquery.h>
#include <stdio.h>
#include <Winreg.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <Lm.h>
#include <shellapi.h>
#include <SetupAPI.h>
#include "nx\rmc\keym.h"

using namespace NX;

#define COMPANY_NAME L"NextLabs"
#define PRODUCT_NAME L"SkyDRM"
#define FILENAME_REGISTER L"register.xml"
#define FILENAME_AUDIT_LOG L"audit.db"

#define MAX_DRIVERFILES 2
const wchar_t *wstrSourceDriverFiles[MAX_DRIVERFILES] = {L"drv2\\nxrmdrv.sys", L"drv1\\nxrmflt.sys"};
const wchar_t *wstrDistDriverFiles[MAX_DRIVERFILES] = {L"nxrmdrv.sys", L"nxrmflt.sys"};

#define MAX_UNSTOPPABLE_DRIVERS 2
const wchar_t * const wstrUnstoppableDriverSrcDirs[MAX_UNSTOPPABLE_DRIVERS] = {L"drv2", L"drv3"};
const wchar_t * const wstrUnstoppableDriverNames[MAX_UNSTOPPABLE_DRIVERS] = {L"nxrmdrv", L"nxrmvhd"};

#define MAX_STOPPABLE_DRIVERS 1
const wchar_t * const wstrStoppableDriverNames[MAX_STOPPABLE_DRIVERS] = {L"nxrmflt"};



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//Note:  MessageBox can not use in deferred execution since not be able to get UILevel property
UINT _stdcall MessageAndLogging(MSIHANDLE hInstall, BOOL bLogOnly, const WCHAR* wstrMsg )
{
	if(bLogOnly == FALSE && hInstall!= NULL)
	{
		INT nUILevel =0;
		WCHAR wstrTemp[2] = {0};
		DWORD dwBufsize = 0;
		
		dwBufsize = sizeof(dwBufsize)/sizeof(WCHAR);	
		if(ERROR_SUCCESS == MsiGetProperty(hInstall, TEXT("UILevel"), wstrTemp, &dwBufsize))
		{
			nUILevel = _wtoi(wstrTemp);
		}

		if(nUILevel > 2)
		{
			MessageBox(GetForegroundWindow(),(LPCWSTR) wstrMsg, (LPCWSTR)PRODUCT_NAME, MB_OK|MB_ICONWARNING);	
		}
	}

	//add log here
	PMSIHANDLE hRecord = MsiCreateRecord(1);
	if(hRecord !=NULL)
	{
		MsiRecordSetString(hRecord, 0, wstrMsg);
		// send message to running installer
		MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRecord);
		MsiCloseHandle(hRecord);
	}

	
	return ERROR_SUCCESS;
}//return service current status, or return 0 for service not existed

DWORD GetServiceStatus(const wchar_t *wstrServiceName)
{
	SC_HANDLE hSCManager,hService;
	WCHAR wstrTemp[128] = {0};
	DWORD dwErrorCode = 0;

	if ( wstrServiceName==NULL || wstrServiceName[0]==L'\0')	
		return 0;
	
	hSCManager = OpenSCManager(NULL, NULL, GENERIC_READ);
	if (hSCManager==NULL)
	{
		dwErrorCode = GetLastError();
		swprintf_s(wstrTemp,128, L"Open SC Menager Failed. Error Code: %d", dwErrorCode);
		//MessageBox(GetForegroundWindow(), (LPCWSTR)wstrTemp,(LPCWSTR)PRODUCT_NAME , MB_OK|MB_ICONWARNING);
		return 0;
	}

	hService = OpenService(hSCManager, wstrServiceName, GENERIC_READ);	
	if (hService== NULL)
	{		
		CloseServiceHandle(hSCManager);
		return 0;
	}

	SERVICE_STATUS_PROCESS ServiceStatus;
	ZeroMemory(&ServiceStatus, sizeof(ServiceStatus));
	DWORD dwBytesNeeded = 0;

	if (!QueryServiceStatusEx(hService,
							SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ServiceStatus,
							sizeof(ServiceStatus),
							&dwBytesNeeded))
	{
		dwErrorCode = GetLastError();
		swprintf_s(wstrTemp,128,L"SC query Service status failed. Error Code: %d", dwErrorCode);
		//MessageBox(GetForegroundWindow(), (LPCWSTR)wstrTemp, (LPCWSTR)PRODUCT_NAME, MB_OK|MB_ICONWARNING);
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hService);
		return 0;
	}

	DWORD dwStatus = 0;
	dwStatus = ServiceStatus.dwCurrentState;
	
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);

	return dwStatus;
}


BOOL SHCopy(LPCWSTR from, LPCWSTR to, BOOL bDeleteFrom)
{
	SHFILEOPSTRUCT fileOp = {0};
	WCHAR newFrom[MAX_PATH + 1];
	WCHAR newTo[MAX_PATH + 1];

	if(bDeleteFrom)
		fileOp.wFunc = FO_MOVE;
	else
		fileOp.wFunc = FO_COPY;

	fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

	wcscpy_s(newFrom, MAX_PATH, from);
	newFrom[wcslen(from) + 1] = NULL;
	fileOp.pFrom = newFrom;

	wcscpy_s(newTo, MAX_PATH, to);
	newTo[wcslen(to) + 1] = NULL;
	fileOp.pTo = newTo;

	int result = SHFileOperation(&fileOp);

	return result == 0;
}

int SHDelete(LPCWSTR from)
{
	SHFILEOPSTRUCT fileOp = {NULL};
	WCHAR newFrom[MAX_PATH + 1];

	fileOp.wFunc = FO_DELETE;
	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI;

	wcscpy_s(newFrom, MAX_PATH, from);
	newFrom[wcslen(from) + 1] = NULL;
	fileOp.pFrom = newFrom;

	fileOp.pTo = NULL;

	return SHFileOperation(&fileOp);
}

std::wstring squishGUID(const std::wstring& guid) {
	// Convert this:    "{12345678-9ABC-DEFG-HIJK-LMNOPQRSTUVW}"
	// to this:         "87654321CBA9GFEDIHKJMLONQPSRUTWV"
	static const struct {short start, len;} squishDict[] = {
		{ 1, 8}, {10, 4}, {15, 4}, {20, 2}, {22, 2}, {25, 2}, {27, 2}, {29, 2},
		{31, 2}, {33, 2}, {35, 2}
	};

	std::wstring squished;

	for (int i = 0; i < _countof(squishDict); i++) {
		for (int j = squishDict[i].start + squishDict[i].len - 1;
			 j >= squishDict[i].start;
			 j--) {
			squished += guid[j];
		}
	}

	return squished;
}

//*****************************************************************************************************
//				START MSI CUSTOM ACTION FUNCTION HERE
//*****************************************************************************************************

UINT __stdcall MyTest(MSIHANDLE hInstall )
{
	//MessageBox(GetForegroundWindow(),(LPCWSTR) L"Hello world, I am here # 1", (LPCWSTR)PRODUCT_NAME, MB_OK);	
	MessageAndLogging(hInstall, FALSE, L"Hello world, I am here # 1 " );
	
	return ERROR_SUCCESS;
}

//CACleanUp, call in deferred execution in system context
UINT __stdcall UninstallCleanUp(MSIHANDLE hInstall )
{
	HKEY hKey = NULL;
		
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
									TEXT("SYSTEM\\CurrentControlSet\\services\\"),
									0, 
									KEY_ALL_ACCESS, 
									&hKey))
	{			
		SHDeleteKey(hKey,TEXT("nxrmdrv"));	
		SHDeleteKey(hKey,TEXT("nxrmserv"));	
		SHDeleteKey(hKey,TEXT("nxrmflt"));	
		RegCloseKey(hKey);
	}
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Uninstall clean up done."));

   return ERROR_SUCCESS;
}

//CAResetDrvReg, call in deferred execution in system context
UINT __stdcall ResetNxrmdrvRegdata(MSIHANDLE hInstall )
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start update registry for drivers."));
	//remove pending delete flag for nxrmdrv duing major upgrade
	HKEY hKey = NULL;
	WCHAR wstrTemp[MAX_PATH] = {0};
	DWORD dwBufsize = 0;

	
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
									TEXT("SYSTEM\\CurrentControlSet\\services\\nxrmdrv\\"),
									0, 
									KEY_WRITE|KEY_READ, 
									&hKey))

	{	
		 dwBufsize = sizeof(wstrTemp)/sizeof(WCHAR);
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, 			
											TEXT("DeleteFlag"),
											NULL, 
											NULL, 
											(LPBYTE)wstrTemp, 
											& dwBufsize))
		{
			RegDeleteValue(hKey,TEXT("DeleteFlag"));
			MessageAndLogging(hInstall, TRUE, TEXT("Remove delete mode."));
		}

		DWORD dwStart = 1; //reset to 1 will auto start driver after reboot
		if (ERROR_SUCCESS == RegSetValueEx(hKey, 			
											TEXT("Start"),
											NULL, 
											REG_DWORD, 
											(const BYTE*)&dwStart,
											sizeof(dwStart)))
		{
			MessageAndLogging(hInstall, TRUE, TEXT("Reset to auto start."));
		}

		RegCloseKey(hKey);
	}
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG:  Update registry for drivers done."));
   return ERROR_SUCCESS;
}

//CACheckPendingReboot, call in immediate excution
UINT __stdcall CheckRebootCondition(MSIHANDLE hInstall)
{
	DWORD dwRetStatus = 0;
	UINT uiRet =0;

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start check reboot status."));

	dwRetStatus = GetServiceStatus(L"nxrmdrv"); //running
	if(dwRetStatus == SERVICE_RUNNING)
	{
		MessageAndLogging(hInstall, TRUE, TEXT("NXRMLOG: nxrmdrv is running."));
		if( GetServiceStatus(L"nxrmflt") != SERVICE_RUNNING ) //not running
		{
			MessageAndLogging(hInstall, TRUE, TEXT("NXRMLOG: nxrmflt is stopped. Needs to reboot this computer."));
			uiRet =  MsiSetProperty(hInstall, L"REBOOT", L"Force");
			if (uiRet != ERROR_SUCCESS )
			{
				MessageAndLogging(hInstall, FALSE, TEXT("Set Force REBOOT property failed."));
				return ERROR_INSTALL_FAILURE; 
			}
		}
		else
		{
			MessageAndLogging(hInstall, TRUE, TEXT("NXRMLOG: nxrmflt is running. No reboot needed."));
		}
			
	}
	
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Check reboot status done."));

	return ERROR_SUCCESS;
}

//CAFindFile, call in immediate excution
UINT __stdcall FindConfigFile(MSIHANDLE hInstall )
{
	//0. Skip checking for file if we are to use the built-in one
	WCHAR wstrPropVal[1024];
	DWORD dwPropVal = _countof(wstrPropVal);
	UINT uiRet = 0;
	uiRet = MsiGetProperty(hInstall, TEXT("NXRMUSEBUILTINCONFIGFILE"), wstrPropVal, &dwPropVal);
	if (uiRet == ERROR_SUCCESS && _wcsicmp(wstrPropVal, L"Yes") == 0)
	{
		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Skip checking file Register.xml"));
		return ERROR_SUCCESS;
	}

	WCHAR wstrSourceDir[MAX_PATH] = {0};
	WCHAR wstrTemp[MAX_PATH] = {0};
	DWORD dwPathBuffer = 0;
	WCHAR wstrMsg[128] = {0};
	DWORD dwErrorCode = 0;
	BOOL bFindFileError =FALSE;

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start checking file Register.xml"));

	//1. Get source path from MSI
	dwPathBuffer = sizeof(wstrSourceDir)/sizeof(WCHAR);
	uiRet = MsiGetProperty(hInstall, TEXT("SourceDir"), wstrSourceDir, &dwPathBuffer );
	if( uiRet != ERROR_SUCCESS)
	{
		dwErrorCode = GetLastError();
		swprintf_s(wstrMsg, 128, L"Get Souce dirctory from Msi failed. Error Code: %d", dwErrorCode);
		MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg );
	
		return ERROR_INSTALL_FAILURE; 
	}

	//2. Try getting parent dir of OriginalDatabase if SourceDir is not defined.
	if (dwPathBuffer == 0)
	{
		dwPathBuffer = _countof(wstrSourceDir);
		uiRet = MsiGetProperty(hInstall, TEXT("OriginalDatabase"), wstrSourceDir, &dwPathBuffer);
		if(uiRet != ERROR_SUCCESS)
		{
			swprintf_s(wstrMsg, L"Get Original Database from Msi failed. Error Code: %u", uiRet);
			MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg );
			return ERROR_INSTALL_FAILURE;
		}

		//Find parent dir of OriginalDatabase
		WCHAR *p = wcsrchr(wstrSourceDir, L'\\');
		if (p == NULL)
		{
			swprintf_s(wstrMsg, L"Can't find parent directory of OriginaDatabase \"%s\".", wstrSourceDir);
			MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg );
			bFindFileError = TRUE;
		}
		else
		{
			*p = L'\0';
		}
	}

	if (!bFindFileError)
	{
		//Check if file exist
		if(wstrSourceDir[wcslen(wstrSourceDir)-1]!= L'\\')
		{
			wcscat_s(wstrSourceDir, MAX_PATH,  L"\\");
		}
		wcscat_s(wstrSourceDir, MAX_PATH, FILENAME_REGISTER );

		if (GetFileAttributes(wstrSourceDir)==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND)
		{
			MessageAndLogging(hInstall, TRUE, TEXT("File not found, try current directory...."));
			bFindFileError = TRUE;
		}
	}
	
	//3. Try CURRENTDIRECTORY property in reinstall mode
	if(bFindFileError)
	{
		ZeroMemory(wstrSourceDir, sizeof(wstrSourceDir));
		uiRet = 0;
		dwPathBuffer = sizeof(wstrSourceDir)/sizeof(WCHAR);

		uiRet = MsiGetProperty(hInstall, TEXT("CURRENTDIRECTORY"), wstrSourceDir, &dwPathBuffer );
		if( uiRet != ERROR_SUCCESS)
		{
			dwErrorCode = GetLastError();
			swprintf_s(wstrMsg, 128, L"Get Current dirctory from Msi failed. Error Code: %d", dwErrorCode);
			MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg );
	
			return ERROR_INSTALL_FAILURE; 
		}
	
		//Check if file exist
		if(wstrSourceDir[wcslen(wstrSourceDir)-1]!= L'\\')
		{
			wcscat_s(wstrSourceDir, MAX_PATH,  L"\\");
		}
		wcscat_s(wstrSourceDir, MAX_PATH, FILENAME_REGISTER );

		if (GetFileAttributes(wstrSourceDir)==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND)
		{
			// 4. Try installdir in REINSTALL mode
			WCHAR wstrReinstall[MAX_PATH] = {0};
			DWORD dwBuffer = sizeof(wstrReinstall)/sizeof(WCHAR);
			uiRet =0;
			uiRet = MsiGetProperty(hInstall, TEXT("REINSTALL"), wstrReinstall, &dwBuffer );
			
			if( uiRet != ERROR_SUCCESS || wcslen(wstrReinstall) == 0) //not in reinstall
			{
				MessageAndLogging(hInstall, FALSE, TEXT("The required configuration file is not found.  Please run the Client Package Builder first."));
				return ERROR_INSTALL_FAILURE; 
			}
			
			ZeroMemory(wstrSourceDir, sizeof(wstrSourceDir));
			dwPathBuffer = sizeof(wstrSourceDir)/sizeof(WCHAR);
			uiRet =0;
			uiRet = MsiGetProperty(hInstall, TEXT("INSTALLDIR"), wstrSourceDir, &dwPathBuffer );
			if( uiRet != ERROR_SUCCESS)
			{
				dwErrorCode = GetLastError();
				swprintf_s(wstrMsg, 128, L"Get install dirctory from Msi failed. Error Code: %d", dwErrorCode);
				MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg );
				return ERROR_INSTALL_FAILURE; 
			}

			if(wstrSourceDir[wcslen(wstrSourceDir)-1]!= L'\\')
			{
				wcscat_s(wstrSourceDir, MAX_PATH,  L"\\");
			}
			wcscat_s(wstrSourceDir, MAX_PATH, L"conf\\");
			wcscat_s(wstrSourceDir, MAX_PATH, FILENAME_REGISTER );
			

			if (GetFileAttributes(wstrSourceDir)==INVALID_FILE_ATTRIBUTES )
			{		
				MessageAndLogging(hInstall, FALSE, TEXT("The required configuration file is not found.  Please run the Client Package Builder first."));
				return ERROR_INSTALL_FAILURE; 
			}
		}	

	}

	//get temp path
	DWORD dwRetVal = 0;
	dwRetVal = GetTempPath(MAX_PATH, wstrTemp);                                 
    if ((dwRetVal > MAX_PATH) || (dwRetVal == 0))
    {
		MessageAndLogging(hInstall, FALSE, TEXT("Failed to get temp path in this computer."));
        return ERROR_INSTALL_FAILURE;
    }
	
	// verify temp path exists
	HANDLE hTempFile = INVALID_HANDLE_VALUE;
	hTempFile = CreateFile(	wstrTemp,
							GENERIC_READ,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING|CREATE_NEW,
							FILE_FLAG_BACKUP_SEMANTICS,
							NULL);
		
	if ( hTempFile == INVALID_HANDLE_VALUE ) 
	{
		if (!CreateDirectory(wstrTemp, NULL))
		{
			dwErrorCode = GetLastError();
			if ( dwErrorCode != ERROR_ALREADY_EXISTS )
			{
				swprintf_s(wstrMsg, 128, L"Failed to create temp path. Error Code: %d", dwErrorCode);
				MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
				return ERROR_INSTALL_FAILURE;
			}
		}		
	}
	CloseHandle(hTempFile);
	
	//Move file from source to temp
	if(wstrTemp[wcslen(wstrTemp)-1] != L'\\')
	{
		wcscat_s(wstrTemp, MAX_PATH,  L"\\");
	}	
	wcscat_s(wstrTemp, MAX_PATH, FILENAME_REGISTER);

	SetFileAttributes(wstrTemp, FILE_ATTRIBUTE_NORMAL);
	
	if( CopyFile(wstrSourceDir, wstrTemp, FALSE)== FALSE) //Failed
	{
		dwErrorCode = GetLastError();
		swprintf_s(wstrMsg, 128, L"Failed to copy file to temp path. Error Code: %d", dwErrorCode);
		MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
		return ERROR_INSTALL_FAILURE; 
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Checking file Register.xml done.  Status: Good."));

    return ERROR_SUCCESS;
}

//CACopyFile, call in deferred execution in system context
UINT __stdcall CopyConfigFile(MSIHANDLE hInstall ) //run in deferred execution
{
	WCHAR wstrSourceDir[MAX_PATH] = {0};
	WCHAR wstrInstallDir[MAX_PATH] = {0};
	DWORD dwPathBuffer = 0;
	WCHAR wstrMsg[128] = {0};
	DWORD dwErrorCode = 0;

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start copy config file."));
	//get current Installdir from MSI
	dwPathBuffer = sizeof(wstrInstallDir)/sizeof(WCHAR);
	if(ERROR_SUCCESS !=  MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrInstallDir, & dwPathBuffer))
	{
		dwErrorCode = GetLastError();
		swprintf_s(wstrMsg, 128, L"Failed to get install directory from MSI. Error Code: %d", dwErrorCode);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);//log only
	
		return ERROR_INSTALL_FAILURE;
	}


	if(wstrInstallDir[wcslen(wstrInstallDir)-1]!= L'\\')
	{
		wcscat_s(wstrInstallDir, L"\\");
	}	
	wcscat_s(wstrInstallDir, L"conf\\");
	wcscat_s(wstrInstallDir, FILENAME_REGISTER);

	//get file from temp
	DWORD dwRetVal = 0;
	dwRetVal = GetTempPath(MAX_PATH, wstrSourceDir);                                 
    if ((dwRetVal > MAX_PATH) || (dwRetVal == 0))
    {
		MessageAndLogging(hInstall, TRUE, TEXT("Failed to get temp path in this computer."));
        return ERROR_INSTALL_FAILURE;
    }

	if(wstrSourceDir[wcslen(wstrSourceDir)-1]!= L'\\')
	{
		wcscat_s(wstrSourceDir, L"\\");
	}
	wcscat_s(wstrSourceDir, FILENAME_REGISTER);

	//If file does not exist in temp, it must be because we're using a built-in file.  So no need to copy.
	if (GetFileAttributes(wstrSourceDir)==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND)
	{
		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Skip copying file Register.xml"));
		return ERROR_SUCCESS;
	}

	//prevent read only file already existed
	SetFileAttributes(wstrInstallDir, FILE_ATTRIBUTE_NORMAL); 
	
	//Move file from Temp to Install Directory
	if(CopyFile(wstrSourceDir, wstrInstallDir, FALSE)== FALSE)
	{
		dwErrorCode = GetLastError();
		swprintf_s(wstrMsg, 128, L"Copy Register.xml file failed. Error Code: %d", dwErrorCode);

		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
		return ERROR_INSTALL_FAILURE; 
	}

	//Clean up file
	DeleteFile(wstrSourceDir);

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Copy config file success."));

    return ERROR_SUCCESS;

}

//CACopyDrivers, call in deferred execution in system context
UINT __stdcall CopyDriverFiles(MSIHANDLE hInstall ) //run in deferred execution
{	
	WCHAR wstrInstallDir[MAX_PATH] = {0};
	WCHAR wstrInstallDirbin[MAX_PATH] = {0};
	WCHAR wstrWindowsDir[MAX_PATH] ={0};
	WCHAR wstrSystemDirDrivers[MAX_PATH] ={0};
	DWORD dwPathBuffer = 0;
	UINT uiRetCode = 0;

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start copy driver files."));

	//get current Installdir from MSI
	dwPathBuffer = sizeof(wstrInstallDir)/sizeof(WCHAR);
	uiRetCode =  MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrInstallDir, &dwPathBuffer);
	if(ERROR_SUCCESS != uiRetCode)
	{
		WCHAR wstrMsg[128] = {0};		
		swprintf_s(wstrMsg, 128, L"NXRMLOG: Failed to get install directory from MSI. Error Code: %d", uiRetCode);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);//log only	
		return ERROR_INSTALL_FAILURE;
	}
	
	if(wstrInstallDir[wcslen(wstrInstallDir)-1]!= L'\\')
	{
		wcscat_s(wstrInstallDir, _countof(wstrInstallDir),  L"\\");
	}
	wcscpy_s(wstrInstallDirbin, MAX_PATH, wstrInstallDir );	
	wcscat_s(wstrInstallDirbin, _countof(wstrInstallDirbin),  L"bin\\");

	if(!GetSystemWindowsDirectory(wstrWindowsDir, MAX_PATH))
	{
		MessageAndLogging(hInstall, TRUE, TEXT("NXRMLOG: Failed to get windows directory in this computer."));
        return ERROR_INSTALL_FAILURE;
	}

	if(wstrWindowsDir[wcslen(wstrWindowsDir)-1]!= L'\\')	
	{
		wcscat_s(wstrWindowsDir, _countof(wstrWindowsDir),  L"\\");
	}
	wcscpy_s(wstrSystemDirDrivers, MAX_PATH, wstrWindowsDir);
	wcscat_s(wstrSystemDirDrivers, _countof(wstrSystemDirDrivers),  L"System32\\drivers\\");
	

	//start copy *.sys files from install directory to system32\\drivers
	PVOID OldValue = NULL;
	if( Wow64DisableWow64FsRedirection(&OldValue) ) 
	{
		for(int i=0; i<MAX_DRIVERFILES; i++)
		{
			WCHAR wstrFile[MAX_PATH] ={0};
			WCHAR wstrDistFile[MAX_PATH] ={0};
			ZeroMemory(wstrFile, sizeof(wstrFile));
			wcscpy_s(wstrFile, MAX_PATH, wstrInstallDirbin);
			wcscat_s(wstrFile, _countof(wstrFile), wstrSourceDriverFiles[i]);

			ZeroMemory(wstrDistFile, sizeof(wstrDistFile));
			wcscpy_s(wstrDistFile, MAX_PATH, wstrSystemDirDrivers);
			wcscat_s(wstrDistFile, _countof(wstrDistFile), wstrDistDriverFiles[i]);
		
			SetFileAttributes(wstrDistFile, FILE_ATTRIBUTE_NORMAL);

			if(CopyFile(wstrFile, wstrDistFile, FALSE)== FALSE) //Failed
			{
				DWORD lastErr = GetLastError();
				WCHAR wstrMsg[1024] = {0};
				swprintf_s(wstrMsg, 1024, L"NXRMLOG: Copy driver file from %s to %s failed. Error Code: %d", wstrFile, wstrDistFile, lastErr);
				MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
				if(lastErr != ERROR_SHARING_VIOLATION)
				{
					Wow64RevertWow64FsRedirection(OldValue);
					return ERROR_INSTALL_FAILURE;
				}
			}
			else
			{
				WCHAR wstrMsg[1024] = {0};
				swprintf_s(wstrMsg, 1024, L"NXRMLOG: Copy driver file from %s to %s success.", wstrFile , wstrDistFile);
				MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
			}
		}
		Wow64RevertWow64FsRedirection(OldValue) ;
	}

	
	//Get nxrmdrv driverstore path info from register**********************************
	HKEY hKey;
	LONG lResult;

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
									TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DIFx\\Services\\nxrmdrv\\"),
									0, 
									KEY_READ|KEY_WOW64_64KEY, 
									&hKey);
	
	if(ERROR_SUCCESS == lResult )
	{	
		WCHAR wstrTemp[MAX_PATH];
		DWORD dwBufsize = 0;
		dwBufsize = sizeof(wstrTemp)*sizeof(WCHAR);
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, 			
											TEXT("RefCount"),
											NULL, 
											NULL, 
											(LPBYTE)wstrTemp, 
											&dwBufsize))
		{
			WCHAR wstrSys[MAX_PATH] = {0};
			WCHAR wstrInf[MAX_PATH] = {0};
			WCHAR wstrSourceSys[MAX_PATH] = {0};
			WCHAR wstrSourceInf[MAX_PATH] = {0};
			swprintf_s(wstrSys, MAX_PATH, L"%sSystem32\\DRVSTORE\\%s\\nxrmdrv.sys", wstrWindowsDir ,wstrTemp);
			swprintf_s(wstrInf, MAX_PATH, L"%sSystem32\\DRVSTORE\\%s\\nxrmdrv.inf", wstrWindowsDir ,wstrTemp);
			swprintf_s(wstrSourceSys, MAX_PATH, L"%sdrv2\\nxrmdrv.sys", wstrInstallDirbin);
			swprintf_s(wstrSourceInf, MAX_PATH, L"%sdrv2\\nxrmdrv.inf", wstrInstallDirbin);
			
			OldValue = NULL;
			if( Wow64DisableWow64FsRedirection(&OldValue) )
			{
				SetFileAttributes(wstrSys, FILE_ATTRIBUTE_NORMAL);
				if(CopyFile(wstrSourceSys, wstrSys, FALSE)==FALSE) //failed
				{
					WCHAR wstrMsg[1024] = {0};
					swprintf_s(wstrMsg, 1024, L"NXRMLOG: ERROR CODE: %d , copy driver file from %s to %s failed. ", GetLastError(), wstrSourceSys, wstrSys);
					MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);

				}
				SetFileAttributes(wstrInf, FILE_ATTRIBUTE_NORMAL);
				if(CopyFile(wstrSourceInf, wstrInf, FALSE)== FALSE)
				{
					WCHAR wstrMsg[1024] = {0};
					swprintf_s(wstrMsg, 1024, L"NXRMLOG: ERROR CODE: %d , copy driver file from %s to %s failed. ", GetLastError(), wstrSourceInf, wstrInf);
					MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
				}
				Wow64RevertWow64FsRedirection(OldValue) ;
			}
		}
		else
		{
			WCHAR wstrMsg[1024] = {0};
			swprintf_s(wstrMsg, 1024, L"NXRMLOG: query key for nxrmdrv error. ERROR CODE: %d",GetLastError());
			MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
		}
		RegCloseKey(hKey);
	}
	else
	{
		WCHAR wstrMsg[1024] = {0};
		swprintf_s(wstrMsg, 1024, L"NXRMLOG: open key nxrmdrv error. ERROR CODE: %d", lResult);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
	}
		

	//Get nxrmflt driverstore path info from register	
	hKey = NULL;
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
									TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DIFx\\Services\\nxrmflt\\"),
									0, 
									KEY_READ|KEY_WOW64_64KEY, 
									&hKey))

	{	
		WCHAR wstrTemp[MAX_PATH] = {0};
		DWORD dwBufsize = 0;
	
		dwBufsize = sizeof(wstrTemp)/sizeof(WCHAR);
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, 			
											TEXT("RefCount"),
											NULL, 
											NULL, 
											(LPBYTE)wstrTemp, 
											& dwBufsize))
		{
			WCHAR wstrSys[MAX_PATH] = {0};
			WCHAR wstrInf[MAX_PATH] = {0};
			WCHAR wstrSourceSys[MAX_PATH] = {0};
			WCHAR wstrSourceInf[MAX_PATH] = {0};
			swprintf_s(wstrSys, MAX_PATH, L"%sSystem32\\DRVSTORE\\%s\\nxrmflt.sys", wstrWindowsDir ,wstrTemp);
			swprintf_s(wstrInf, MAX_PATH, L"%sSystem32\\DRVSTORE\\%s\\nxrmflt.inf", wstrWindowsDir ,wstrTemp);
			swprintf_s(wstrSourceSys, MAX_PATH, L"%sdrv1\\nxrmflt.sys", wstrInstallDirbin);
			swprintf_s(wstrSourceInf, MAX_PATH, L"%sdrv1\\nxrmflt.inf", wstrInstallDirbin);

			OldValue = NULL;
			if( Wow64DisableWow64FsRedirection(&OldValue) )
			{
				SetFileAttributes(wstrSys, FILE_ATTRIBUTE_NORMAL);
				if(CopyFile(wstrSourceSys, wstrSys, FALSE)==FALSE) //failed
				{
					WCHAR wstrMsg[1024] = {0};
					swprintf_s(wstrMsg, 1024, L"NXRMLOG: ERROR CODE: %d , copy driver file from %s to %s failed. ", GetLastError(), wstrSourceSys, wstrSys);
					MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);

				}
				SetFileAttributes(wstrInf, FILE_ATTRIBUTE_NORMAL);
				if(CopyFile(wstrSourceInf, wstrInf, FALSE)== FALSE)
				{
					WCHAR wstrMsg[1024] = {0};
					swprintf_s(wstrMsg, 1024, L"NXRMLOG: ERROR CODE: %d , copy driver file from %s to %s failed. ", GetLastError(), wstrSourceInf, wstrInf);
					MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
				}
				Wow64RevertWow64FsRedirection(OldValue) ;
			}
		}
		else
		{
			WCHAR wstrMsg[1024] = {0};
			swprintf_s(wstrMsg, 1024, L"NXRMLOG: query key for nxrmflt error. ERROR CODE: %d",GetLastError());
			MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
		}

		RegCloseKey(hKey);
	}
	else
	{
		WCHAR wstrMsg[1024] = {0};
		swprintf_s(wstrMsg, 1024, L"NXRMLOG: open key nxrmflt error. ERROR CODE: %d", lResult);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
	}

	
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Copy driver file done."));
    return ERROR_SUCCESS;

}

//CAStopRMService, call in deferred execution in system context
UINT __stdcall StopRMService(MSIHANDLE hInstall) //run in deferred execution
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start stopping RM service."));

	if (GetServiceStatus(L"nxrmserv") != SERVICE_RUNNING)
	{
		MessageAndLogging(hInstall, TRUE, L"NXRLOG: nxrmserv is not running.  No need to stop it.");
		return ERROR_SUCCESS;
	}

	SC_HANDLE hSCManager;
	hSCManager = OpenSCManager(NULL, NULL, 0);
	if (hSCManager == NULL)
	{
		WCHAR wstrMsg[1024];
		swprintf_s(wstrMsg, L"NXRMLOG: OpenSCManager failed. Error Code: %lu", GetLastError());
		MessageAndLogging(hInstall, TRUE, wstrMsg);
		return ERROR_INSTALL_FAILURE;
	}

	SC_HANDLE hService;
	hService = OpenService(hSCManager, L"nxrmserv", SERVICE_STOP);
	if (hService == NULL)
	{
		WCHAR wstrMsg[1024];
		swprintf_s(wstrMsg, L"NXRMLOG: OpenService() failed. Error Code: %lu", GetLastError());
		MessageAndLogging(hInstall, TRUE, wstrMsg);
		CloseServiceHandle(hSCManager);
		return ERROR_INSTALL_FAILURE;
	}

	SERVICE_STATUS status;
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &status))
	{
		WCHAR wstrMsg[1024];
		swprintf_s(wstrMsg, L"NXRMLOG: ControlService() failed. Error Code: %lu", GetLastError());
		MessageAndLogging(hInstall, TRUE, wstrMsg);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return ERROR_INSTALL_FAILURE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	// Check the status until it is stopped or until 30 seconds has passed.
	const int numTries = 30;

	for (int i = 0; i < numTries; i++)
	{
		if (GetServiceStatus(L"nxrmserv") == SERVICE_STOPPED)
		{
			break;
		}
		MessageAndLogging(hInstall, TRUE, L"NXRMLOG: waiting for nxrmserv to finish stopping");
		Sleep(1000);
	}

	// Wait one more second, just in case the service process hasn't exited
	// yet after reporting its status as SERVICE_STOPPED.
	Sleep(1000);

	// Whether the service has stopped or not, we return success in order to
	// let the installer continue.
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Stopping RM service done."));
	return ERROR_SUCCESS;
}

//CAStopStoppableDrivers, call in deferred execution in system context
UINT __stdcall StopStoppableDrivers(MSIHANDLE hInstall) //run in deferred execution
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start stopping stoppable drivers."));

	SC_HANDLE hSCManager;

	hSCManager = OpenSCManager(NULL, NULL, 0);
	if (hSCManager == NULL)
	{
		WCHAR wstrMsg[1024];
		swprintf_s(wstrMsg, L"NXRMLOG: OpenSCManager failed. Error Code: %lu", GetLastError());
		MessageAndLogging(hInstall, TRUE, wstrMsg);
		return ERROR_INSTALL_FAILURE;
	}

	for (int i = 0; i < MAX_STOPPABLE_DRIVERS; i++)
	{
		if (GetServiceStatus(wstrStoppableDriverNames[i]) == SERVICE_RUNNING)
		{
			SC_HANDLE hService;
			hService = OpenService(hSCManager, wstrStoppableDriverNames[i], SERVICE_STOP);
			if (hService == NULL)
			{
				WCHAR wstrMsg[1024];
				swprintf_s(wstrMsg, L"NXRMLOG: OpenService(%s) failed. Error Code: %lu", wstrStoppableDriverNames[i], GetLastError());
				MessageAndLogging(hInstall, TRUE, wstrMsg);
				CloseServiceHandle(hSCManager);
				return ERROR_INSTALL_FAILURE;
			}

			SERVICE_STATUS status;
			if (!ControlService(hService, SERVICE_CONTROL_STOP, &status))
			{
				WCHAR wstrMsg[1024];
				swprintf_s(wstrMsg, L"NXRMLOG: ControlService(%s) failed. Error Code: %lu", wstrStoppableDriverNames[i], GetLastError());
				MessageAndLogging(hInstall, TRUE, wstrMsg);
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return ERROR_INSTALL_FAILURE;
			}

			CloseServiceHandle(hService);
		}
	}

	CloseServiceHandle(hSCManager);

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Stopping stoppable drivers done."));
	return ERROR_SUCCESS;
}

//CAInstallUnstoppableDrivers, call in deferred execution in system context
UINT __stdcall InstallUnstoppableDrivers(MSIHANDLE hInstall ) //run in deferred execution
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start installing unstoppable drivers."));

	WCHAR wstrInstallDir[MAX_PATH];
	WCHAR wstrInstallDirBin[MAX_PATH];
	DWORD dwPathBuffer;
	UINT uiRetCode;

	//get current InstallDir from MSI
	dwPathBuffer = sizeof(wstrInstallDir)/sizeof(WCHAR);
	uiRetCode =  MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrInstallDir, &dwPathBuffer);
	if(ERROR_SUCCESS != uiRetCode)
	{
		WCHAR wstrMsg[128];
		swprintf_s(wstrMsg, L"NXRMLOG: Failed to get install directory from MSI. Error Code: %u", uiRetCode);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);//log only
		return ERROR_INSTALL_FAILURE;
	}

	if(wstrInstallDir[wcslen(wstrInstallDir)-1]!= L'\\')
	{
		wcscat_s(wstrInstallDir, L"\\");
	}
	wcscpy_s(wstrInstallDirBin, wstrInstallDir );
	wcscat_s(wstrInstallDirBin, L"bin");

	PVOID OldValue;
	BOOL disabledFsRedirection = Wow64DisableWow64FsRedirection(&OldValue);
	if (!disabledFsRedirection)
	{
		DWORD lastErr = GetLastError();
		if (lastErr != ERROR_INVALID_FUNCTION)
		{
			WCHAR wstrMsg[1024];
			swprintf_s(wstrMsg, L"NXRMLOG: Disable FS redirection failed, Error Code: %lu", lastErr);
			MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
			return ERROR_INSTALL_FAILURE;
		}
	}

	SC_HANDLE hSCManager;

	hSCManager = OpenSCManager(NULL, NULL, 0);
	if (hSCManager == NULL)
	{
		WCHAR wstrMsg[1024];
		swprintf_s(wstrMsg, L"NXRMLOG: OpenSCManager failed. Error Code: %lu", GetLastError());
		MessageAndLogging(hInstall, TRUE, wstrMsg);

		if (disabledFsRedirection) Wow64RevertWow64FsRedirection(OldValue);
		return ERROR_INSTALL_FAILURE;

	}

	for (int i = 0; i < MAX_UNSTOPPABLE_DRIVERS; i++)
	{
		WCHAR cmdLine[MAX_PATH + 100];
		swprintf_s(cmdLine, L"DefaultInstall 128 %s\\%s\\%s.inf", wstrInstallDirBin, wstrUnstoppableDriverSrcDirs[i], wstrUnstoppableDriverNames[i]);
		InstallHinfSection(NULL, NULL, cmdLine, 0);

		//Usually the driver is already running if we are upgrading from another build to this build.  So don't try to start it again in this case.
		if (GetServiceStatus(wstrUnstoppableDriverNames[i]) != SERVICE_RUNNING)
		{
			SC_HANDLE hService;
			hService = OpenService(hSCManager, wstrUnstoppableDriverNames[i], SERVICE_START);
			if (hService == NULL)
			{
				WCHAR wstrMsg[1024];
				swprintf_s(wstrMsg, L"NXRMLOG: OpenService(%s) failed. Error Code: %lu", wstrUnstoppableDriverNames[i], GetLastError());
				MessageAndLogging(hInstall, TRUE, wstrMsg);

				CloseServiceHandle(hSCManager);
				if (disabledFsRedirection) Wow64RevertWow64FsRedirection(OldValue);
				return ERROR_INSTALL_FAILURE;
			}

			if (!StartService(hService, 0, NULL))
			{
				WCHAR wstrMsg[1024];
				swprintf_s(wstrMsg, L"NXRMLOG: StartService(%s) failed. Error Code: %lu", wstrUnstoppableDriverNames[i], GetLastError());
				MessageAndLogging(hInstall, TRUE, wstrMsg);

				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				if (disabledFsRedirection) Wow64RevertWow64FsRedirection(OldValue);
				return ERROR_INSTALL_FAILURE;
			}

			CloseServiceHandle(hService);
		}
	}

	CloseServiceHandle(hSCManager);
	if (disabledFsRedirection) Wow64RevertWow64FsRedirection(OldValue);

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Installing unstoppable drivers done."));
	return ERROR_SUCCESS;
}

//CAScheduleRebootForUnstoppableDrivers, call in immediate execution
UINT __stdcall ScheduleRebootForUnstoppableDrivers(MSIHANDLE hInstall)
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start Scheduling reboot for unstoppable drivers."));

	for (int i = 0; i < MAX_UNSTOPPABLE_DRIVERS; i++)
	{
		if (GetServiceStatus(wstrUnstoppableDriverNames[i]) == SERVICE_RUNNING)
		{
			// One of the unstoppable drivers is running.  Need to reboot after uninstallation.
			UINT uiRetCode;

			uiRetCode =  MsiSetProperty(hInstall, L"ISSCHEDULEREBOOT", L"1");
			if (uiRetCode != ERROR_SUCCESS)
			{
				WCHAR wstrMsg[1024];
				swprintf_s(wstrMsg, L"Set ISSCHEDULEREBOOT property failed, uiRetCode=%u.", uiRetCode);
				MessageAndLogging(hInstall, FALSE, wstrMsg);
				// Don't return error.  Continue uninstallation anyway.
			}

			break;
		}
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Scheduling reboot for unstoppable drivers done."));
	return ERROR_SUCCESS;
}

//CAUninstallUnstoppableDrivers, call in deferred execution in system context
UINT __stdcall UninstallUnstoppableDrivers(MSIHANDLE hInstall ) //run in deferred execution
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start uninstalling unstoppable drivers."));

	WCHAR wstrInstallDir[MAX_PATH];
	WCHAR wstrInstallDirBin[MAX_PATH];
	WCHAR wstrWindowsDir[MAX_PATH];
	WCHAR wstrSystemDirDrivers[MAX_PATH];
	DWORD dwPathBuffer;
	UINT uiRetCode;

	//get current InstallDir from MSI
	dwPathBuffer = sizeof(wstrInstallDir)/sizeof(WCHAR);
	uiRetCode =  MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrInstallDir, &dwPathBuffer);
	if(ERROR_SUCCESS != uiRetCode)
	{
		WCHAR wstrMsg[128];
		swprintf_s(wstrMsg, L"NXRMLOG: Failed to get install directory from MSI. Error Code: %u", uiRetCode);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);//log only
		return ERROR_INSTALL_FAILURE;
	}

	if(wstrInstallDir[wcslen(wstrInstallDir)-1]!= L'\\')
	{
		wcscat_s(wstrInstallDir, L"\\");
	}
	wcscpy_s(wstrInstallDirBin, wstrInstallDir );
	wcscat_s(wstrInstallDirBin, L"bin");

	if(!GetSystemWindowsDirectory(wstrWindowsDir, MAX_PATH))
	{
		MessageAndLogging(hInstall, TRUE, TEXT("NXRMLOG: Failed to get windows directory in this computer."));
        return ERROR_INSTALL_FAILURE;
	}
	if(wstrWindowsDir[wcslen(wstrWindowsDir)-1]!= L'\\')	
	{
		wcscat_s(wstrWindowsDir,  L"\\");
	}
	wcscpy_s(wstrSystemDirDrivers, wstrWindowsDir);
	wcscat_s(wstrSystemDirDrivers, L"System32\\drivers\\");


	PVOID OldValue;
	BOOL disabledFsRedirection = Wow64DisableWow64FsRedirection(&OldValue);
	if (!disabledFsRedirection)
	{
		DWORD lastErr = GetLastError();
		if (lastErr != ERROR_INVALID_FUNCTION)
		{
			WCHAR wstrMsg[1024];
			swprintf_s(wstrMsg, L"NXRMLOG: Disable FS redirection failed, Error Code: %lu", lastErr);
			MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
			return ERROR_INSTALL_FAILURE;
		}
	}

	for (int i = 0; i < MAX_UNSTOPPABLE_DRIVERS; i++)
	{
		WCHAR cmdLine[MAX_PATH + 100];
		swprintf_s(cmdLine, L"DefaultUninstall 128 %s\\%s\\%s.inf", wstrInstallDirBin, wstrUnstoppableDriverSrcDirs[i], wstrUnstoppableDriverNames[i]);
		InstallHinfSection(NULL, NULL, cmdLine, 0);

		WCHAR wstrDrvFile[MAX_PATH];
		swprintf_s(wstrDrvFile, L"%s%s.sys", wstrSystemDirDrivers, wstrUnstoppableDriverNames[i]);

		if (!DeleteFile(wstrDrvFile))
		{
			WCHAR wstrMsg[1024];
			swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteFile(\"%s\") failed. ERROR CODE: %lu.  Delaying delete until reboot.", wstrDrvFile, GetLastError());
			MessageAndLogging(hInstall, TRUE, wstrMsg);

			if (!MoveFileEx(wstrDrvFile, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
			{
				swprintf_s(wstrMsg, L"******** NXRMLOG: MoveFileEx(\"%s\") failed. ERROR CODE: %lu.", wstrDrvFile, GetLastError());
				MessageAndLogging(hInstall, TRUE, wstrMsg);
			}
		}
	}

	if (disabledFsRedirection) Wow64RevertWow64FsRedirection(OldValue);

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Uninstalling unstoppable drivers done."));
	return ERROR_SUCCESS;
}



//CABackupLogs
UINT __stdcall BackupLogFiles(MSIHANDLE hInstall )
{
	BOOL bBackupConfFiles = TRUE;

	//Get ProductCode of previous product.
	UINT uiRet;
	WCHAR wstrPropVal[1024];
	DWORD dwPropVal;
	dwPropVal = _countof(wstrPropVal);
	uiRet = MsiGetProperty(hInstall, TEXT("OLDPRODUCTS"), wstrPropVal, &dwPropVal);
	if (uiRet != ERROR_SUCCESS)
	{
		WCHAR wstrMsg[128];
		swprintf_s(wstrMsg, L"NXRMLOG: Failed to get product code of previous product from MSI. Error Code: %d", uiRet);
		MessageAndLogging(hInstall, TRUE, wstrMsg);//log only
		return ERROR_INSTALL_FAILURE;
	}

	//Get version number of previous product from Registry.
	HKEY hKey = NULL;
	WCHAR regKey[128] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
	LONG lRet;

	wcscat_s(regKey, wstrPropVal);
	lRet =  RegOpenKeyEx(HKEY_LOCAL_MACHINE, regKey, 0,
                         KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		WCHAR wstrTemp[128];
		DWORD dwBufsize = sizeof(wstrTemp);

		if (ERROR_SUCCESS == RegQueryValueEx(hKey, L"DisplayVersion", NULL,
											 NULL, (LPBYTE) wstrTemp,
											 &dwBufsize))
		{
			//If the previous product is 8.x.x or 9.0.x, don't back up config
			//files
			if (wcsncmp(wstrTemp, L"8.", 2) == 0 ||
				wcsncmp(wstrTemp, L"9.0.", 4) == 0)
			{
				bBackupConfFiles = FALSE;
			}
		}

		RegCloseKey(hKey);
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start back up log files. "));

	WCHAR wstrInstallPath[MAX_PATH] = {0};
	WCHAR wstrInstallLogDir[MAX_PATH] = {0};
	WCHAR wstrInstallConfDir[MAX_PATH] = {0};
	WCHAR wstrInstallDebugDumpFile[MAX_PATH] = {0};
	WCHAR wstrTempDir[MAX_PATH] = {0};
	WCHAR wstrTempLogDir[MAX_PATH] = {0};
	WCHAR wstrTempConfDir[MAX_PATH] = {0};
	WCHAR wstrTempConfRegisterFile[MAX_PATH] = {0};
	WCHAR wstrTempDebugDumpFile[MAX_PATH] = {0};
	DWORD dwBufsize = 0;
	BOOL bFoundInstDir =FALSE;

	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
									TEXT("SYSTEM\\CurrentControlSet\\services\\nxrmserv\\"),
									0, 
									KEY_READ, 
									&hKey))

	{	
		WCHAR wstrTemp[MAX_PATH] = {0};
		DWORD dwBufsize = sizeof(wstrTemp);

		if (ERROR_SUCCESS == RegQueryValueEx(hKey, 			
											TEXT("ImagePath"),
											NULL, 
											NULL, 
											(LPBYTE)wstrTemp, 
											& dwBufsize))
		{
			WCHAR* pStr = NULL;
			WCHAR* pStr1 = NULL;
			WCHAR* pStrNext = NULL;

			pStr1 = wcstok_s(wstrTemp, TEXT("\""), &pStrNext);
			if(pStr1!= NULL)
				pStr = wcsstr(pStr1, TEXT("bin\\"));

			if( pStr != NULL)
			{	
				wcsncpy_s(wstrInstallPath, MAX_PATH, pStr1, pStr-pStr1);

				wcscpy_s(wstrInstallLogDir, MAX_PATH, wstrInstallPath);	
				wcscat_s(wstrInstallLogDir, _countof(wstrInstallLogDir),  L"log\\*");

				wcscpy_s(wstrInstallConfDir, MAX_PATH, wstrInstallPath);					
				wcscat_s(wstrInstallConfDir, _countof(wstrInstallConfDir),  L"conf\\*");

				wcscpy_s(wstrInstallDebugDumpFile, wstrInstallPath);
				wcscat_s(wstrInstallDebugDumpFile, L"DebugDump.txt");

				bFoundInstDir = TRUE;
			}
		}

		RegCloseKey(hKey);
	}
	
	if(!bFoundInstDir)
	{
		MessageAndLogging(hInstall, TRUE, TEXT("The previous install path does not found. "));
		return ERROR_SUCCESS;
	}

	//get file from temp
	DWORD dwRetVal = 0;
	dwRetVal = GetTempPath(MAX_PATH,  wstrTempDir);                                 
    if ((dwRetVal > MAX_PATH) || (dwRetVal == 0))
    {
		MessageAndLogging(hInstall, TRUE, TEXT("Failed to get temp path in this computer."));
		return ERROR_SUCCESS;
    }
	
	if(wstrTempDir[wcslen(wstrTempDir)-1]!= L'\\')
	{
		wcscat_s(wstrTempDir, _countof(wstrTempDir),  L"\\");
	}

	wcscpy_s(wstrTempLogDir, MAX_PATH, wstrTempDir);	
	wcscat_s(wstrTempLogDir, _countof(wstrTempLogDir),  L"NxrmLog\\");

	BOOL result1 = SHCopy(wstrInstallLogDir, wstrTempLogDir, FALSE);
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Back up log file done."));


	if (bBackupConfFiles)
	{
		wcscpy_s(wstrTempConfDir, MAX_PATH, wstrTempDir);
		wcscat_s(wstrTempConfDir, _countof(wstrTempConfDir),  L"NxrmConf\\");

		BOOL result2 = SHCopy(wstrInstallConfDir, wstrTempConfDir, FALSE);	

		// Don't back-up (hence restore) register.xml, because we want to use the
		// new register.xml in this build instead of keeping the register.xml from
		// the previous build.
		wcscpy_s(wstrTempConfRegisterFile, wstrTempConfDir);
		wcscat_s(wstrTempConfRegisterFile, FILENAME_REGISTER);
		DeleteFile(wstrTempConfRegisterFile);

		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Back up Conf files done."));
	}
	else
	{
		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Skip Backing up Conf files."));
	}

	wcscpy_s(wstrTempDebugDumpFile, wstrTempDir);
	wcscat_s(wstrTempDebugDumpFile, L"NxrmDebugDump.txt");

	BOOL result3 = SHCopy(wstrInstallDebugDumpFile, wstrTempDebugDumpFile, FALSE);	
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Back up debug dump file done."));

    return ERROR_SUCCESS;
}

//CARestoreLogs, call in deferred execution in system context
UINT __stdcall RestoreLogFiles(MSIHANDLE hInstall )
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start Restore log and conf files. "));

	HKEY hKey = NULL;
	WCHAR wstrInstallDir[MAX_PATH] = {0};	
	WCHAR wstrInstallLogDir[MAX_PATH] = {0};
	WCHAR wstrInstallConfDir[MAX_PATH] = {0};
	WCHAR wstrInstallDebugDumpFile[MAX_PATH] = {0};
	WCHAR wstrTempDir[MAX_PATH] = {0};
	WCHAR wstrTempLogDir[MAX_PATH] = {0};
	WCHAR wstrTempConfDir[MAX_PATH] = {0};
	WCHAR wstrTempDebugDumpFile[MAX_PATH] = {0};

	DWORD dwBufsize = 0;
	BOOL bFoundLogDir = FALSE;
	UINT uiRetCode = 0;

	
	//get current Installdir from MSI
	dwBufsize = sizeof(wstrInstallDir)/sizeof(WCHAR);
	uiRetCode =  MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrInstallDir, &dwBufsize);
	if(ERROR_SUCCESS != uiRetCode)
	{
		WCHAR wstrMsg[128] = {0};		
		swprintf_s(wstrMsg, 128, L"NXRMLOG: Failed to get install directory from MSI. Error Code: %d", uiRetCode);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);//log only	
		return ERROR_SUCCESS;
	}
	
	if(wstrInstallDir[wcslen(wstrInstallDir)-1]!= L'\\')
	{
		wcscat_s(wstrInstallDir, _countof(wstrInstallDir),  L"\\");
	}

	wcscpy_s(wstrInstallLogDir, MAX_PATH, wstrInstallDir);	
	wcscat_s(wstrInstallLogDir, _countof(wstrInstallLogDir),  L"log\\");

	wcscpy_s(wstrInstallConfDir, MAX_PATH, wstrInstallDir);					
	wcscat_s(wstrInstallConfDir, _countof(wstrInstallConfDir),  L"conf\\");

	wcscpy_s(wstrInstallDebugDumpFile, wstrInstallDir);
	wcscat_s(wstrInstallDebugDumpFile, L"DebugDump.txt");

	
	//get file from temp
	DWORD dwRetVal = 0;
	dwRetVal = GetTempPath(MAX_PATH,  wstrTempDir);                                 
    if ((dwRetVal > MAX_PATH) || (dwRetVal == 0))
    {
		MessageAndLogging(hInstall, TRUE, TEXT("Failed to get temp path in this computer."));
		return ERROR_SUCCESS;
    }
	
	if(wstrTempDir[wcslen(wstrTempDir)-1]!= L'\\')
	{
		wcscat_s(wstrTempDir, _countof(wstrTempDir),  L"\\");
	}

	//Restore log	
	//for move files
	wcscpy_s(wstrTempLogDir, MAX_PATH, wstrTempDir);
	wcscat_s(wstrTempLogDir, _countof(wstrTempLogDir),  L"NxrmLog\\*"); 
	BOOL result1 = SHCopy(wstrTempLogDir, wstrInstallLogDir, TRUE);
	// for clean up 
	WCHAR wstrRemoveTempLogDir[MAX_PATH]= {0};
	wcscpy_s(wstrRemoveTempLogDir, MAX_PATH, wstrTempDir);
	wcscat_s(wstrRemoveTempLogDir, _countof(wstrRemoveTempLogDir),  L"NxrmLog"); 
	RemoveDirectory(wstrRemoveTempLogDir);
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Restore log files done."));

	//Restore conf	
	//for move files
	wcscpy_s(wstrTempConfDir, MAX_PATH, wstrTempDir);
	wcscat_s(wstrTempConfDir, _countof(wstrTempConfDir),  L"NxrmConf\\*"); 
	BOOL result2 = SHCopy(wstrTempConfDir, wstrInstallConfDir, TRUE);
	// for clean up later
	WCHAR wstrRemoveTempConfDir[MAX_PATH]= {0};
	wcscpy_s(wstrRemoveTempConfDir, MAX_PATH, wstrTempDir);
	wcscat_s(wstrRemoveTempConfDir, _countof(wstrRemoveTempConfDir),  L"NxrmConf"); 
	RemoveDirectory(wstrRemoveTempConfDir);
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Restore Conf files done."));

	//Restore debug dump
	//for move file
	wcscpy_s(wstrTempDebugDumpFile, wstrTempDir);
	wcscat_s(wstrTempDebugDumpFile, L"NxrmDebugDump.txt");
	BOOL result3 = SHCopy(wstrTempDebugDumpFile, wstrInstallDebugDumpFile, TRUE);
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Restore debug dump file done."));

    return ERROR_SUCCESS;

}

// If skipAuditLog is TRUE:
// - Return TRUE if:
//   - Directory and all of its content are deleted.  (*auditLogFound will be
//     set to FALSE.)
//   - Directory is not deleted because audit log files are present, but all
//     non-audit-log files and empty sub-directories are deleted.
//     (*auditLogFound will be set to TRUE.)
// - Return FALSE if:
//   - Directory is not deleted because some non-audit-log files or empty
//     sub-directories cannot be deleted.  (*auditLogFound will be undefined.)
//
// If skipAuditLog is FALSE:
// - Return TRUE if :
//   - Directory and all of its content are deleted.
// - Return FALSE if:
//   - Directory is not deleted because some files or empty sub-directories
//     cannot be deleted.
// (*auditLogFound will be undefined.)
BOOL DeleteDirMaybeSkipAuditLogs(MSIHANDLE hInstall, const wchar_t *dir,
								 BOOL tryAfterReboot, BOOL skipAuditLog,
								 BOOL *auditLogFound)
{
	BOOL ret = TRUE;
	HANDLE h;
	WIN32_FIND_DATA data;

	*auditLogFound = FALSE;
	wchar_t wildcardPathBuf[MAX_PATH];
	wcscpy_s(wildcardPathBuf, dir);
	wcscat_s(wildcardPathBuf, L"\\*");

	h = FindFirstFile(wildcardPathBuf, &data);
	if (h == INVALID_HANDLE_VALUE)
	{
		DWORD lastErr = GetLastError();
		// Even if directory is empty, FindFirstFile() should still have found
		// "."  or ".." (except for empty root directories, in which case we
		// can't delete anyway).  Hence this error is a real error.
		WCHAR wstrMsg[1024];
		swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteDirMaybeSkipAuditLogs: FindFirstFile(\"%s\") failed. ERROR CODE: %lu", wildcardPathBuf, lastErr);
		MessageAndLogging(hInstall, TRUE, wstrMsg);

		// If the error is access-denied, try to delete the directory itself
		// anyway.  If deletion is successful, we'll know that the directory
		// is empty.
		//
		// This handles some Content.IE5 directories where we get
		// ERROR_ACCESS_DENIED when trying to traverse it but we can
		// successfully delete it.
		if (lastErr != ERROR_ACCESS_DENIED)
		{
			return FALSE;
		}
	}
	else
	{
		do
		{
			if (wcscmp(data.cFileName, L".") == 0 ||
				wcscmp(data.cFileName, L"..") == 0)
			{
				continue;
			}

			wchar_t subPathBuf[MAX_PATH];
			wcscpy_s(subPathBuf, dir);
			wcscat_s(subPathBuf, L"\\");
			wcscat_s(subPathBuf, data.cFileName);

			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				// Recurse.
				BOOL auditLogFoundInSubDir;

				if (!DeleteDirMaybeSkipAuditLogs(hInstall, subPathBuf,
												 tryAfterReboot, skipAuditLog,
												 &auditLogFoundInSubDir))
				{
					ret = FALSE;
				}

				if (auditLogFoundInSubDir)
				{
					*auditLogFound = TRUE;
				}
			}
			else
			{
				if (skipAuditLog && wcscmp(data.cFileName, FILENAME_AUDIT_LOG) ==0)
				{
					*auditLogFound = TRUE;
					continue;
				}

				if (!DeleteFile(subPathBuf))
				{
					WCHAR wstrMsg[1024];
					swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteDirMaybeSkipAuditLogs: DeleteFile(\"%s\") failed. ERROR CODE: %lu", subPathBuf, GetLastError());
					MessageAndLogging(hInstall, TRUE, wstrMsg);

					if (tryAfterReboot)
					{
						if (!MoveFileEx(subPathBuf, NULL,
										MOVEFILE_DELAY_UNTIL_REBOOT))
						{
							swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteDirMaybeSkipAuditLogs: MoveFileEx(\"%s\") failed. ERROR CODE: %lu", subPathBuf, GetLastError());
							MessageAndLogging(hInstall, TRUE, wstrMsg);
							ret = FALSE;
						}
					} else {
						ret = FALSE;
					}
				}
			}
		} while (FindNextFile(h, &data));

		FindClose(h);
	}

	// Remove this directory if 1) no error occurred in this directory or any
	// sub-directories, and 2) no audit log files were found in this directory
	// or any sub-directories.
	if (ret && !*auditLogFound)
	{
		if (!RemoveDirectory(dir))
		{
			WCHAR wstrMsg[1024];
			swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteDirMaybeSkipAuditLogs: RemoveDirectory(\"%s\") failed. ERROR CODE: %lu", dir, GetLastError());
			MessageAndLogging(hInstall, TRUE, wstrMsg);

			if (tryAfterReboot)
			{
				if (!MoveFileEx(dir, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
				{
					swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteDirMaybeSkipAuditLogs: MoveFileEx(\"%s\") failed. ERROR CODE: %lu", dir, GetLastError());
					MessageAndLogging(hInstall, TRUE, wstrMsg);
					ret = FALSE;
				}
			} else {
				ret = FALSE;
			}
		}
	}

	return ret;
}

BOOL DeleteProfilesCommon(MSIHANDLE hInstall, BOOL tryAfterReboot,
						  BOOL skipAuditLog)
{
	WCHAR wstrSourceDir[MAX_PATH];
	DWORD dwPathBuffer;
	UINT uiRet;

	dwPathBuffer = _countof(wstrSourceDir);
	uiRet = MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrSourceDir, &dwPathBuffer );

	if(wstrSourceDir[wcslen(wstrSourceDir)-1]!= L'\\')
	{
		wcscat_s(wstrSourceDir, _countof(wstrSourceDir),  L"\\");
	}

	wcscat_s(wstrSourceDir, L"profiles");

	// It's okay if profiles directory is not found.
	if (GetFileAttributes(wstrSourceDir) == INVALID_FILE_ATTRIBUTES &&
		GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: profiles directory not found."));
		return TRUE;
	}

	BOOL auditLogFound;
	if (!DeleteDirMaybeSkipAuditLogs(hInstall, wstrSourceDir, tryAfterReboot,
									 skipAuditLog, &auditLogFound))
	{
		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Delete profiles error."));
		return FALSE;
	}

	return TRUE;
}

//CADeleteProfiles, call in deferred execution in system context
UINT __stdcall DeleteProfiles(MSIHANDLE hInstall)
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start deleting profiles."));

	if (!DeleteProfilesCommon(hInstall, TRUE, FALSE))
	{
		return ERROR_INSTALL_FAILURE;
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Deleting profiles done."));
	return ERROR_SUCCESS;
}

//CADeleteProfilesExceptAuditLogs, call in deferred execution in system context
UINT __stdcall DeleteProfilesExceptAuditLogs(MSIHANDLE hInstall)
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start deleting profiles except audit logs."));

	if (!DeleteProfilesCommon(hInstall, TRUE, TRUE))
	{
		return ERROR_INSTALL_FAILURE;
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Deleting profiles except audit logs done."));
	return ERROR_SUCCESS;
}

//CACheckProductInOtherInstallationContext, call in immediate execution
UINT __stdcall CheckProductInOtherInstallationContext(MSIHANDLE hInstall)
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start checking product in other installation context."));

	WCHAR wstrMsg[1024];

	WCHAR wstrUpgradeCode[1024];
	DWORD dwUpgradeCode;
	UINT uiRet;

	dwUpgradeCode = _countof(wstrUpgradeCode);
	uiRet = MsiGetProperty(hInstall, TEXT("UpgradeCode"), wstrUpgradeCode,
		&dwUpgradeCode);
	if (uiRet != ERROR_SUCCESS) {
		swprintf_s(wstrMsg, L"******** NXRMLOG: CheckProductInOtherInstallationContext: Failed to get UpgradeCode.	ERROR CODE: %u", uiRet);
		MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
		return ERROR_INSTALL_FAILURE;
	}

	const std::wstring upgradeCodeSquished = squishGUID(wstrUpgradeCode);

	HKEY hKey;
	LONG lResult;
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer\\UpgradeCodes\\" + upgradeCodeSquished).c_str(),
		0,
		KEY_READ | KEY_WOW64_64KEY,
		&hKey);
	if (lResult == ERROR_SUCCESS) {
		RegCloseKey(hKey);
		MessageAndLogging(hInstall, FALSE, PRODUCT_NAME TEXT(" is already installed by another user on this computer.  It cannot be installed on the same computer again."));
		return ERROR_INSTALL_PREREQUISITE_FAILED;
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Checking product in other installation context done."));
	return ERROR_SUCCESS;
}

//CAClientReg, call in deferred execution in system context
UINT __stdcall ClientReg(MSIHANDLE hInstall)
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start registering client ID."));

	WCHAR wstrMsg[1024];

	WCHAR wstrAllUsers[256];
	DWORD dwAllUsers;
	UINT uiRet;

	dwAllUsers = _countof(wstrAllUsers);
	uiRet = MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrAllUsers, &dwAllUsers);
	if(ERROR_SUCCESS != uiRet)
	{
		swprintf_s(wstrMsg, L"******** NXRMLOG: ClientReg: Failed to get ALLUSERS from MSI.  ERROR CODE: %u", uiRet);
		MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
		return ERROR_INSTALL_FAILURE;
	}

	NX::RmClientId clientId;
	Result res;

	// First, try to load client key from local machine's store.
	res = clientId.Load(true);
	if (!res) {
		if (res.GetCode() != ERROR_NOT_FOUND) {
			swprintf_s(wstrMsg, L"******** NXRMLOG: ClientReg: Failed to load registered client ID from local machine's store.  ERROR CODE: %d, msg: %hs",
				res.GetCode(), res.GetMsg().c_str());
			MessageAndLogging(hInstall, FALSE, wstrMsg);
			return ERROR_INSTALL_FAILURE;
		}

		// There is no client key in local machine's store.

		if (wcscmp(wstrAllUsers, L"1") == 0) {
			// We are installing for local machine.

			// Create client key in local machine's store.
			res = clientId.Create(true);
			if (!res) {
				swprintf_s(wstrMsg, L"******** NXRMLOG: ClientReg: Failed to register client ID in local machine's store.  ERROR CODE: %d, msg: %hs",
					res.GetCode(), res.GetMsg().c_str());
				MessageAndLogging(hInstall, FALSE, wstrMsg);
				return ERROR_INSTALL_FAILURE;
			}

			MessageAndLogging(hInstall, TRUE, L"******** NXRMLOG: ClientReg: Registered Client ID in local machine's store.");
		} else {
			// We are installing for current user.

			// Try to load client key from current user's store.
			res = clientId.Load(false);
			if (!res) {
				if (res.GetCode() != ERROR_NOT_FOUND) {
					swprintf_s(wstrMsg, L"******** NXRMLOG: ClientReg: Failed to load registered client ID from current user's store.  ERROR CODE: %d, msg: %hs",
						res.GetCode(), res.GetMsg().c_str());
					MessageAndLogging(hInstall, FALSE, wstrMsg);
					return ERROR_INSTALL_FAILURE;
				}

				// There is no client key in current user's store.

				// Create client key in current user's store.
				res = clientId.Create(false);
				if (!res) {
					swprintf_s(wstrMsg, L"******** NXRMLOG: ClientReg: Failed to register client ID in current user's store.  ERROR CODE: %d, msg: %hs",
						res.GetCode(), res.GetMsg().c_str());
					MessageAndLogging(hInstall, FALSE, wstrMsg);
					return ERROR_INSTALL_FAILURE;
				}

				MessageAndLogging(hInstall, TRUE, L"******** NXRMLOG: ClientReg: Registered Client ID in current user's store.");
			} else {
				// There is existing cliekt key in current user's store.
				MessageAndLogging(hInstall, TRUE, L"******** NXRMLOG: ClientReg: Found existing Client ID in current user's store.");
			}
		}
	} else {
		// There is existing cliekt key in local machine's store.
		MessageAndLogging(hInstall, TRUE, L"******** NXRMLOG: ClientReg: Found existing Client ID in local machine's store.");
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Registering client ID done."));
	return ERROR_SUCCESS;
}

//CADeleteLocalAppData, call in deferred execution in system context
UINT __stdcall DeleteLocalAppData(MSIHANDLE hInstall)
{
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Start deleting local app data."));

	WCHAR wstrMsg[1024];

	WCHAR wstrAllUsers[256];
	DWORD dwAllUsers;
	UINT uiRet;

	dwAllUsers = _countof(wstrAllUsers);
	uiRet = MsiGetProperty(hInstall, TEXT("CustomActionData"), wstrAllUsers,
		&dwAllUsers);
	if(ERROR_SUCCESS != uiRet)
	{
		swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteLocalAppData: Failed to get ALLUSERS from MSI.  ERROR CODE: %u", uiRet);
		MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
		return ERROR_INSTALL_FAILURE;
	}

	PWSTR pszAppLocalPath;
	HRESULT hResult;

	// Get local app data path for current user.
	hResult = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT,
		NULL, &pszAppLocalPath);
	if (FAILED(hResult)) {
		swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteLocalAppData: Failed to get local app data path for current user.  ERROR CODE: 0x%08lX", hResult);
		MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
		return ERROR_INSTALL_FAILURE;
	}

	// Delete the product directory under the current user's local app
	// directory.
	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: DeleteLocalAppData: Deleting local app data for current user."));

	const std::wstring companyDirCurUser = std::wstring(pszAppLocalPath) +
		L"\\" + COMPANY_NAME;
	const std::wstring prodDirCurUser = companyDirCurUser + L"\\" +
		PRODUCT_NAME;
	int iRet = SHDelete(prodDirCurUser.c_str());
	if (iRet != 0) {
		swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteLocalAppData: Failed to delete %s.  ERROR CODE: 0x%08lX", prodDirCurUser.c_str(), iRet);
		MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
		// Don't return error.  Continue uninstallation anyway.
	}

	// Delete the company directory if it is empty.
	RemoveDirectory(companyDirCurUser.c_str());

	CoTaskMemFree(pszAppLocalPath);

	if (wcscmp(wstrAllUsers, L"1") == 0) {
		// We are uninstalling for local machine.

		// Get local app data path for Default User.
		hResult = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT,
			(HANDLE) -1, &pszAppLocalPath);
		if (FAILED(hResult)) {
			swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteLocalAppData: Failed to get local app data path for Default User.  ERROR CODE: 0x%08lX", hResult);
			MessageAndLogging(hInstall, FALSE, (LPCWSTR)wstrMsg);
			return ERROR_INSTALL_FAILURE;
		}

		// Delete the product directory under other users' local app
		// directories.
		MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: DeleteLocalAppData: Deleting local app data for other users."));

		const std::wstring defaultUserDirStr(L"\\Default\\");
		const std::wstring appLocalPathStr(pszAppLocalPath);
		size_t pos = appLocalPathStr.find(defaultUserDirStr);
		std::wstring appLocalPathPrefix = appLocalPathStr.substr(0, pos);
		std::wstring appLocalPathSuffix = appLocalPathStr.substr(pos +
			defaultUserDirStr.length());
		CoTaskMemFree(pszAppLocalPath);

		HANDLE h;
		WIN32_FIND_DATA data;
		h = FindFirstFile((appLocalPathPrefix + L"\\*").c_str(), &data);
		if (h == INVALID_HANDLE_VALUE) {
			wprintf(L"******** NXRMLOG: DeleteLocalAppData: Failed to enumerate local app directories.  ERROR CODE: %lu", GetLastError());
			MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
			// Don't return error.  Continue uninstallation anyway.
		} else {
			do
			{
				if (wcscmp(data.cFileName, L".") == 0 ||
					wcscmp(data.cFileName, L"..") == 0) {
					continue;
				}

				if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
					const std::wstring companyDir = appLocalPathPrefix +
						L"\\" + data.cFileName + L"\\" + appLocalPathSuffix +
						L"\\" + COMPANY_NAME;
					const std::wstring prodDir = companyDir + L"\\" +
						PRODUCT_NAME;

					if (prodDir != prodDirCurUser) {
						int iRet = SHDelete(prodDir.c_str());
						if (iRet != 0) {
							swprintf_s(wstrMsg, L"******** NXRMLOG: DeleteLocalAppData: Failed to delete %s.  ERROR CODE: 0x%08lX", prodDir.c_str(), iRet);
							MessageAndLogging(hInstall, TRUE, (LPCWSTR)wstrMsg);
							// Don't return error.  Continue uninstallation
							// anyway.
						}

						// Delete the company directory if it is empty.
						RemoveDirectory(companyDir.c_str());
					}
				}
			} while (FindNextFile(h, &data));

			FindClose(h);
		}
	}

	MessageAndLogging(hInstall, TRUE, TEXT("******** NXRMLOG: Deleting local app data done."));
	return ERROR_SUCCESS;
}
