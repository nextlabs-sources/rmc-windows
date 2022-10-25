#pragma once

#include "commdlg.h"
#include "shellapi.h"

#define SAVEASCTL_COCREATEINSTANCE_PROC_NAME	"CoCreateInstance"
#define SAVEASCTL_GETSAVEFILENAMEW_PROC_NAME	"GetSaveFileNameW"
#define SAVEASCTL_GETOPENFILENAMEW_PROC_NAME	"GetOpenFileNameW"
#define SAVEASCTL_MOVEFILEEXW_PROC_NAME			"MoveFileExW"
#define SAVEASCTL_COPYFILEEXW_PROC_NAME			"CopyFileExW"
#define SAVEASCTL_COPYFILE2_PROC_NAME			"CopyFile2"
#define SAVEASCTL_SHFILEOPERATIONW_PROC_NAME	"SHFileOperationW"

#define SAVEASCTL_NXL_EXT	L".nxl"

typedef HRESULT (WINAPI *SAVEASCTL_COCREATEINSTANCE)(
	IN REFCLSID		rclsid, 
	IN LPUNKNOWN	pUnkOuter,
	IN DWORD		dwClsContext, 
	IN REFIID		riid, 
	OUT LPVOID FAR* ppv);

HRESULT WINAPI Core_SaveAsCtl_CoCreateInstance(
	IN REFCLSID		rclsid, 
	IN LPUNKNOWN	pUnkOuter,
	IN DWORD		dwClsContext, 
	IN REFIID		riid, 
	OUT LPVOID FAR* ppv);

typedef BOOL (WINAPI *GETSAVEFILENAMEW)(LPOPENFILENAMEW);

typedef BOOL (WINAPI *GETOPENFILENAMEW)(LPOPENFILENAME);

BOOL WINAPI Core_GetSaveFileNameW(LPOPENFILENAMEW);

BOOL WINAPI Core_GetOpenFileNameW(_Inout_  LPOPENFILENAME lpofn);

BOOL InitializeSaveAsCtlHook(void);
void CleanupSaveAsCtlHook(void);

BOOL InitializeLegacySaveAsCtlHook(void);
void CleanupLegacySaveAsCtlHook(void);

typedef BOOL (WINAPI *MOVEFILEEXW)(
	_In_     LPCWSTR lpExistingFileName,
	_In_opt_ LPCWSTR lpNewFileName,
	_In_     DWORD    dwFlags
	);

BOOL WINAPI Core_MoveFileExW(
	_In_     LPCWSTR lpExistingFileName,
	_In_opt_ LPCWSTR lpNewFileName,
	_In_     DWORD    dwFlags
	);

typedef BOOL (WINAPI *COPYFILEEXW)(
	_In_        LPCWSTR lpExistingFileName,
	_In_        LPCWSTR lpNewFileName,
	_In_opt_    LPPROGRESS_ROUTINE lpProgressRoutine,
	_In_opt_    LPVOID lpData,
	_When_(pbCancel != NULL, _Pre_satisfies_(*pbCancel == FALSE))
	_Inout_opt_ LPBOOL pbCancel,
	_In_        DWORD dwCopyFlags
	);

BOOL WINAPI	Core_CopyFileExW(
	_In_        LPCWSTR lpExistingFileName,
	_In_        LPCWSTR lpNewFileName,
	_In_opt_    LPPROGRESS_ROUTINE lpProgressRoutine,
	_In_opt_    LPVOID lpData,
	_When_(pbCancel != NULL, _Pre_satisfies_(*pbCancel == FALSE))
	_Inout_opt_ LPBOOL pbCancel,
	_In_        DWORD dwCopyFlags
	);

typedef HRESULT (WINAPI *COPYFILE2)(
	_In_      PCWSTR                          pwszExistingFileName,
	_In_      PCWSTR                          pwszNewFileName,
	_In_opt_  COPYFILE2_EXTENDED_PARAMETERS   *pExtendedParameters
);

HRESULT WINAPI Core_CopyFile2(
	_In_      PCWSTR                          pwszExistingFileName,
	_In_      PCWSTR                          pwszNewFileName,
	_In_opt_  COPYFILE2_EXTENDED_PARAMETERS   *pExtendedParameters
);

typedef int (WINAPI *SHFILEOPERATIONW)(_Inout_ LPSHFILEOPSTRUCTW lpFileOp);

int WINAPI Core_SHFileOperationW(_Inout_ LPSHFILEOPSTRUCTW lpFileOp);

typedef int (WINAPI *SHFILEOPERATIONWITHADDITIONALFLAGS)(_Inout_ LPSHFILEOPSTRUCTW lpFileOp, _In_ ULONG Flags);

int WINAPI Core_SHFileOperationWithAdditionalFlags(_Inout_ LPSHFILEOPSTRUCTW lpFileOp, _In_ ULONG Flags);

BOOL InitializeExplorerRenameHook(void);
void CleanupExplorerRenameHook(void);

BOOL InitializeCopyFileExWHook(void);
void CleanupCopyFileExWHook(void);

BOOL InitializeExplorerCopyFileHook(void);
void CleanupExplorerCopyFileHook(void);