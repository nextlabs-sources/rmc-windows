#include "stdafx.h"
#include "nxrmcoreglobal.h"
#include "nxrmdrv.h"
#include "nxrmcorehlp.h"
#include "detour.h"
#include "saveasctl.h"
#include "filedialog.h"
#include "fileopsink.h"
#include "adobe.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern						CORE_GLOBAL_DATA Global;

	extern						BOOL update_active_document_from_protected_child_process(void);

	void						forecast_saveas_to_engine(const WCHAR *SrcFileName, const WCHAR *SaveAsFileName);

	void						print_guid(const GUID &id);

	BOOL						send_block_notification_ex(const WCHAR *SrcFileName, const WCHAR *DstFileName, BLOCK_NOTIFICATION_TYPE Type);

	extern BOOL					send_block_notification(const WCHAR *FileName, BLOCK_NOTIFICATION_TYPE Type);

	extern BOOL					nxrmcoreLookupEntryPoint(IN PVOID DllBase, IN PCHAR Name, OUT PVOID *EntryPoint);

#ifdef __cplusplus
}
#endif

static SAVEASCTL_COCREATEINSTANCE			g_fnorg_CoCreateInstance = NULL;
static SAVEASCTL_COCREATEINSTANCE			g_fn_CoCreateInstance_trampoline = NULL;

static GETSAVEFILENAMEW		g_fnorg_GetSaveFileNameW = NULL;
static GETSAVEFILENAMEW		g_fn_GetSaveFileNameW_trampoline = NULL;

static GETOPENFILENAMEW		g_fnorg_GetOpenFileNameW = NULL;
static GETOPENFILENAMEW		g_fn_GetOpenFileNameW_trampoline = NULL;

static MOVEFILEEXW			g_fnorg_MoveFileExW = NULL;
static MOVEFILEEXW			g_fn_MoveFileExW_trampoline = NULL;

static COPYFILEEXW			g_fnorg_CopyFileExW = NULL;
static COPYFILEEXW			g_fn_CopyFileExW_trampoline = NULL;

static COPYFILE2			g_fnorg_CopyFile2 = NULL;
static COPYFILE2			g_fn_CopyFile2_trampoline = NULL;

static SHFILEOPERATIONW		g_fnorg_SHFileOperationW = NULL;
static SHFILEOPERATIONW		g_fn_SHFileOperationW_trampoline = NULL;

static SHFILEOPERATIONWITHADDITIONALFLAGS	g_fnorg_SHFileOperationWithAdditionalFlags = NULL;
static SHFILEOPERATIONWITHADDITIONALFLAGS	g_fn_SHFileOperationWithAdditionalFlags_trampoline = NULL;

static BOOL InitializeSaveAsCtlHookPreWin8(void);
static BOOL InitializeSaveAsCtlHookWin8(void);
static BOOL InitializeExplorerCopyFileHookPreWin8(void);
static BOOL InitializeExplorerCopyFileHookWin8(void);

static BOOL Win32Path2NTPath(const WCHAR *src, WCHAR *ntpath, ULONG *ccntpath);

static BOOL is_file_name_end_with_nxl(const WCHAR *FileName);
static BOOL is_path_end_with_slash(const WCHAR *FileName);

BOOL InitializeSaveAsCtlHook(void)
{
	if(Global.IsWin8AndAbove)
	{
		return InitializeSaveAsCtlHookWin8();
	}
	else
	{
		return InitializeSaveAsCtlHookPreWin8();
	}
}

void CleanupSaveAsCtlHook(void)
{
	if(g_fn_CoCreateInstance_trampoline)
	{
		remove_hook(g_fn_CoCreateInstance_trampoline);
		g_fnorg_CoCreateInstance = NULL;
	}

	Global.SaveAsCtlHooksInitialized = FALSE;
}

static BOOL InitializeSaveAsCtlHookPreWin8(void)
{
	BOOL bRet = TRUE;

	do 
	{
		if (Global.Ole32Handle == NULL)
		{
			Global.Ole32Handle = GetModuleHandleW(OLE32_MODULE_NAME);
		}

		if (Global.Ole32Handle == NULL)
		{
			bRet = FALSE;
			break;
		}

		if (Global.SaveAsCtlHooksInitialized)
		{
			break;
		}

		//
		// OFFICE take care of SaveAs by itself
		//
		if (Global.IsMsOffice)
		{
			Global.SaveAsCtlHooksInitialized = TRUE;
			break;
		}

		nxrmcoreLookupEntryPoint(Global.Ole32Handle, SAVEASCTL_COCREATEINSTANCE_PROC_NAME, (PVOID*)&g_fnorg_CoCreateInstance);

		if(g_fnorg_CoCreateInstance)
		{
			if(!install_hook(g_fnorg_CoCreateInstance, (PVOID*)&g_fn_CoCreateInstance_trampoline, Core_SaveAsCtl_CoCreateInstance))
			{
				bRet = FALSE;
				break;
			}
		}

		Global.SaveAsCtlHooksInitialized = TRUE;
		
	} while (FALSE);

	return bRet;
}

static BOOL InitializeSaveAsCtlHookWin8(void)
{
	BOOL bRet = TRUE;

	do 
	{
		if (Global.CombaseHandle == NULL)
		{
			Global.CombaseHandle = GetModuleHandleW(COMBASE_MODULE_NAME);
		}

		if (Global.CombaseHandle == NULL)
		{
			bRet = FALSE;
			break;
		}

		if (Global.SaveAsCtlHooksInitialized)
		{
			break;
		}

		//
		// OFFICE take care of SaveAs by itself
		//
		if (Global.IsMsOffice)
		{
			Global.SaveAsCtlHooksInitialized = TRUE;
			break;
		}

		nxrmcoreLookupEntryPoint(Global.CombaseHandle, SAVEASCTL_COCREATEINSTANCE_PROC_NAME, (PVOID*)&g_fnorg_CoCreateInstance);

		if (g_fnorg_CoCreateInstance)
		{
			if(!install_hook(g_fnorg_CoCreateInstance, (PVOID*)&g_fn_CoCreateInstance_trampoline, Core_SaveAsCtl_CoCreateInstance))
			{
				bRet = FALSE;
				break;
			}
		}

		Global.SaveAsCtlHooksInitialized = TRUE;

	} while(FALSE);

	return bRet;
}

BOOL InitializeExplorerCopyFileHook(void)
{
	if (Global.IsWin8AndAbove)
	{
		return InitializeExplorerCopyFileHookWin8();
	}
	else
	{
		return InitializeExplorerCopyFileHookPreWin8();
	}
}

void CleanupExplorerCopyFileHook(void)
{
	if (g_fn_CopyFile2_trampoline)
	{
		remove_hook(g_fn_CopyFile2_trampoline);
		g_fn_CopyFile2_trampoline = NULL;
	}

	if (g_fn_SHFileOperationW_trampoline)
	{
		remove_hook(g_fn_SHFileOperationW_trampoline);
		g_fn_SHFileOperationW_trampoline = NULL;
	}

	if (g_fn_SHFileOperationWithAdditionalFlags_trampoline)
	{
		remove_hook(g_fn_SHFileOperationWithAdditionalFlags_trampoline);
		g_fn_SHFileOperationWithAdditionalFlags_trampoline = NULL;
	}
}

static BOOL InitializeExplorerCopyFileHookPreWin8(void)
{
	BOOL bRet = TRUE;

	HMODULE hShell32 = NULL;

	do 
	{
		hShell32 = GetModuleHandleW(SHELL32_MODULE_NAME);

		if (!hShell32)
		{
			bRet = FALSE;
			break;
		}

		g_fnorg_SHFileOperationW = (SHFILEOPERATIONW)GetProcAddress(hShell32, SAVEASCTL_SHFILEOPERATIONW_PROC_NAME);

		if (g_fnorg_SHFileOperationW)
		{
#ifndef _AMD64_

			if (!install_hook(g_fnorg_SHFileOperationW, (PVOID*)&g_fn_SHFileOperationW_trampoline, Core_SHFileOperationW))
			{
				bRet = FALSE;
				break;
			}

#else
			BYTE *p = (BYTE*)g_fnorg_SHFileOperationW;
			LONG off_set = 0;

			if (*p != 0x33 ||
				*(p + 1) != 0xd2 ||
				*(p + 2) != 0xe9)
			{
				bRet = FALSE;
				break;
			}

			//
			// skip the xor edx, edx instruction
			//
			p += 2;

			off_set = *(LONG*)(p + 1);

			g_fnorg_SHFileOperationWithAdditionalFlags = (SHFILEOPERATIONWITHADDITIONALFLAGS)((p + 5) + off_set);

			if (!install_hook(g_fnorg_SHFileOperationWithAdditionalFlags, (PVOID*)&g_fn_SHFileOperationWithAdditionalFlags_trampoline, Core_SHFileOperationWithAdditionalFlags))
			{
				bRet = FALSE;
				break;
			}
#endif
		}
		
	} while (FALSE);

	return TRUE;
}

static BOOL InitializeExplorerCopyFileHookWin8(void)
{
	BOOL bRet = TRUE;

	HMODULE hkernelBase = NULL;

	do
	{
		hkernelBase = GetModuleHandleW(KERNELBASE_MODULE_NAME);

		if (!hkernelBase)
		{
			bRet = FALSE;
			break;
		}

		g_fnorg_CopyFile2 = (COPYFILE2)GetProcAddress(hkernelBase, SAVEASCTL_COPYFILE2_PROC_NAME);

		if (g_fnorg_CopyFile2)
		{
			if (!install_hook(g_fnorg_CopyFile2, (PVOID*)&g_fn_CopyFile2_trampoline, Core_CopyFile2))
			{
				bRet = FALSE;
				break;
			}
		}

	} while (FALSE);

	return bRet;
}

HRESULT WINAPI Core_SaveAsCtl_CoCreateInstance(
	IN REFCLSID		rclsid, 
	IN LPUNKNOWN	pUnkOuter,
	IN DWORD		dwClsContext, 
	IN REFIID		riid, 
	OUT LPVOID FAR* ppv)
{
	HRESULT hr = S_OK;

	hr = g_fn_CoCreateInstance_trampoline(rclsid,pUnkOuter,dwClsContext,riid,ppv);

	if (riid == IID_IFileDialog && hr == S_OK)
	{
		CoreIFileDialog *pCoreIFileDialog = NULL;
		IFileDialog *pFileDialog = NULL;

		do 
		{
			pFileDialog = *(IFileDialog **)ppv;

			pCoreIFileDialog = new CoreIFileDialog(pFileDialog);

			*ppv = (LPVOID)pCoreIFileDialog;

		} while (FALSE);
	}
	else if (riid == IID_IFileSaveDialog && hr == S_OK)
	{
		CoreIFileSaveDialog *pCoreIFileSaveDialog = NULL;
		IFileSaveDialog *pFileSaveDialog = NULL;

		do 
		{
			pFileSaveDialog = *(IFileSaveDialog **)ppv;

			pCoreIFileSaveDialog = new CoreIFileSaveDialog(pFileSaveDialog);

			*ppv = (LPVOID)pCoreIFileSaveDialog;

		} while (FALSE);

	}
	else if (riid == IID_IFileOperation && hr == S_OK && Global.IsExplorer)
	{
		CoreIFileOperationProgressSink *pCoreFileOpSink = NULL;
		IFileOperation	*pFileOp = NULL;

		DWORD dwCookie = 0;

		do 
		{
			pFileOp = *(IFileOperation**)ppv;

			pCoreFileOpSink = new CoreIFileOperationProgressSink;

			pFileOp->Advise((IFileOperationProgressSink*)pCoreFileOpSink, &dwCookie);

		} while (FALSE);
	}

	return hr;
}

BOOL InitializeLegacySaveAsCtlHook(void)
{
	BOOL bRet = TRUE;

	do 
	{
		if (Global.Comdlg32Handle == NULL)
		{
			Global.Comdlg32Handle = GetModuleHandleW(COMDLG32_MODULE_NAME);
		}

		if (Global.Comdlg32Handle == NULL)
		{
			bRet = FALSE;
			break;
		}

		if (Global.LegacySaveAsCtlHooksInitialized)
		{
			break;
		}

		//
		// OFFICE take care of SaveAs by itself
		//
		if (Global.IsMsOffice)
		{
			Global.LegacySaveAsCtlHooksInitialized = TRUE;
			break;
		}

		g_fnorg_GetSaveFileNameW = (GETSAVEFILENAMEW)GetProcAddress(Global.Comdlg32Handle, SAVEASCTL_GETSAVEFILENAMEW_PROC_NAME);

		if(g_fnorg_GetSaveFileNameW)
		{
			if(!install_hook(g_fnorg_GetSaveFileNameW, (PVOID*)&g_fn_GetSaveFileNameW_trampoline, Core_GetSaveFileNameW))
			{
				bRet = FALSE;
				break;
			}
		}

		if (Global.IsVEViewer)
		{
			g_fnorg_GetOpenFileNameW = (GETOPENFILENAMEW)GetProcAddress(Global.Comdlg32Handle, SAVEASCTL_GETOPENFILENAMEW_PROC_NAME);

			if (!install_hook(g_fnorg_GetOpenFileNameW, (PVOID*)&g_fn_GetOpenFileNameW_trampoline, Core_GetOpenFileNameW))
			{
				bRet = FALSE;
				break;
			}
		}

		Global.LegacySaveAsCtlHooksInitialized = TRUE;

	} while (FALSE);

	return bRet;

}

void CleanupLegacySaveAsCtlHook(void)
{
	if (g_fn_GetSaveFileNameW_trampoline)
	{
		remove_hook(g_fn_GetSaveFileNameW_trampoline);
		g_fn_GetSaveFileNameW_trampoline = NULL;
	}

	if (g_fn_GetOpenFileNameW_trampoline)
	{
		remove_hook(g_fn_GetOpenFileNameW_trampoline);
		g_fn_GetOpenFileNameW_trampoline = NULL;
	}
}

BOOL WINAPI Core_GetSaveFileNameW(LPOPENFILENAMEW lpofn)
{
	BOOL bRet = TRUE;

	WCHAR SrcFileName[MAX_PATH] = {0};
	WCHAR DstTmpFileName[MAX_PATH] = {0};

	do 
	{
		bRet = g_fn_GetSaveFileNameW_trampoline(lpofn);

		if (bRet)
		{
			if (!init_rm_section_safe())
			{
				break;
			}

			if (Global.IsAdobeReader && Global.AdobeMode == ADOBE_MODE_PARENT)
			{
				update_active_document_from_protected_child_process();
			}

			EnterCriticalSection(&Global.ActiveDocFileNameLock);

			memcpy(SrcFileName,
				   Global.ActiveDocFileName,
				   min(sizeof(SrcFileName) - sizeof(WCHAR), wcslen(Global.ActiveDocFileName)*sizeof(WCHAR)));

			LeaveCriticalSection(&Global.ActiveDocFileNameLock);

			if (Global.IsAdobeReader && wcslen(SrcFileName) == 0 && wcsstr(Global.AdobeCommandLine, ADOBE_IEMODE_CMD_LINE) == NULL)
			{
				bRet = FALSE;

				send_block_notification(L"", NxrmdrvAdobeHookIsNotReady);
				
				break;
			}

			//
			// take care Adobe Reader X
			// code blow make sure the destination is on local drive

			if (Global.IsAdobeReader)
			{
				WCHAR c = 0;
				WCHAR d = 0;
				WCHAR e = 0;

				WCHAR tmpFileName[4] = { 0 };

				BOOL OnRemoteDrive = FALSE;

				do
				{
					if (wcslen(lpofn->lpstrFile) < sizeof(tmpFileName) / sizeof(WCHAR))
					{
						break;
					}

					c = lpofn->lpstrFile[0];
					d = lpofn->lpstrFile[1];
					e = lpofn->lpstrFile[2];

					if (d != L':' || e != L'\\')
					{
						OnRemoteDrive = TRUE;
						break;
					}

					if ((!(c >= L'A' && c <= L'Z')) &&
						(!(c >= 'a' && c <= 'z')))
					{
						OnRemoteDrive = TRUE;
						break;
					}

					tmpFileName[0] = c;
					tmpFileName[1] = d;
					tmpFileName[2] = e;
					tmpFileName[3] = L'\0';

					if (DRIVE_FIXED != GetDriveTypeW(tmpFileName))
					{
						OnRemoteDrive = TRUE;
						break;
					}

				} while (FALSE);

				if (OnRemoteDrive)
				{
					bRet = FALSE;
					
					send_block_notification(SrcFileName, NxrmdrvSaveAsToUnprotectedVolume);

					break;
				}
			}

			if (Global.IsVEViewer)
			{
				if (0 == wcslen(SrcFileName))
				{
					//
					// double click open
					//

					HMODULE hShell32 = GetModuleHandleW(SHELL32_MODULE_NAME);

					if (hShell32)
					{
						LPWSTR *Argv = NULL;
						int		Argc = 0;

						COMMANDLINETOARGVW fn_CommandLineToArgvW = (COMMANDLINETOARGVW)GetProcAddress(hShell32, NXRMCORE_COMMANDLINETOARGVW_PROC_NAME);

						if (fn_CommandLineToArgvW)
						{
							Argv = fn_CommandLineToArgvW(Global.VEViewerCommandLine, &Argc);

							if (Argv)
							{
								if (Argc >= 2)
								{
									memcpy(SrcFileName,
										   Argv[1],
										   min(sizeof(SrcFileName) - sizeof(WCHAR), wcslen(Argv[1])*sizeof(WCHAR)));
			
								}

								LocalFree(Argv);
								Argv = NULL;
							}
						}
					}
				}

				forecast_saveas_to_engine(SrcFileName, lpofn->lpstrFile);

				swprintf_s(DstTmpFileName, sizeof(DstTmpFileName)/sizeof(WCHAR), L"%s.tmp", lpofn->lpstrFile);

				forecast_saveas_to_engine(SrcFileName, DstTmpFileName);
			}
			else
			{
				forecast_saveas_to_engine(SrcFileName, lpofn->lpstrFile);
			}
		}

	} while (FALSE);

	return bRet;
}

BOOL WINAPI Core_GetOpenFileNameW(LPOPENFILENAMEW lpofn)
{
	BOOL bRet = TRUE;

	do 
	{
		bRet = g_fn_GetOpenFileNameW_trampoline(lpofn);

		if (bRet)
		{
			EnterCriticalSection(&Global.ActiveDocFileNameLock);

			memset(Global.ActiveDocFileName, 0, sizeof(Global.ActiveDocFileName));

			memcpy(Global.ActiveDocFileName,
				   lpofn->lpstrFile,
				   min(sizeof(Global.ActiveDocFileName) - sizeof(WCHAR), wcslen(lpofn->lpstrFile)*sizeof(WCHAR)));

			LeaveCriticalSection(&Global.ActiveDocFileNameLock);
		}

	} while (FALSE);

	return bRet;
}

void forecast_saveas_to_engine(const WCHAR *SrcFileName, const WCHAR *SaveAsFileName)
{
	SAVEAS_FORECAST_REQUEST req = {0};

	NXCONTEXT Ctx = NULL;

	ULONG bytesret = 0;

	WCHAR NTPath[MAX_PATH] = {0};

	ULONG ccNTPath = 0;

	do 
	{
		req.ProcessId	= GetCurrentProcessId();
		req.ThreadId	= GetCurrentThreadId();

		memset(NTPath, 0, sizeof(NTPath));

		ccNTPath = sizeof(NTPath)/sizeof(WCHAR);

		if (!Win32Path2NTPath(SaveAsFileName, NTPath, &ccNTPath))
		{
			break;
		}

		memcpy(req.SaveAsFileName, 
			   NTPath, 
			   min(sizeof(req.SaveAsFileName) - sizeof(WCHAR), wcslen(NTPath)*sizeof(WCHAR)));

		if (SrcFileName && wcslen(SrcFileName))
		{
			memset(NTPath, 0, sizeof(NTPath));

			ccNTPath = sizeof(NTPath)/sizeof(WCHAR);

			if (!Win32Path2NTPath(SrcFileName, NTPath, &ccNTPath))
			{
				break;
			}

			memcpy(req.SourceFileName,
				   NTPath,
				   min(sizeof(req.SourceFileName) - sizeof(WCHAR), wcslen(NTPath)*sizeof(WCHAR)));

		}
		else
		{
			//
			// Adobe Reader should always knows source file
			//
			if (Global.IsAdobeReader)
			{
				break;
			}

			memset(req.SourceFileName, 0, sizeof(req.SourceFileName));
		}

		Ctx = submit_request(Global.Section, NXRMDRV_MSG_TYPE_SAVEAS_FORECAST, &req, sizeof(req));

		if (!Ctx)
		{
			break;
		}

		if (!wait_for_response(Ctx, Global.Section, NULL, 0, &bytesret))
		{
			Ctx = NULL;

			break;
		}

	} while (FALSE);

	return;
}

static BOOL Win32Path2NTPath(const WCHAR *src, WCHAR *ntpath, ULONG *ccntpath)
{
	BOOL bRet = FALSE;

	LONG dstlength = 0;

	ULONG srclength = 0;
	ULONG CharCount = 0;

	WCHAR DeviceName[MAX_PATH] = { 0 };

	WCHAR DosDrive[16] = { 0 };

	WCHAR *p = (WCHAR*)src;

	do 
	{
		srclength = (ULONG)wcslen(p);

		if (srclength < 4)
		{
			dstlength = 0;
			break;
		}

		if (!wcsstr(src, L"\\"))
		{
			//
			// in the case of there is no "\" in path
			//
			if (*ccntpath > wcslen(src))
			{
				dstlength = (LONG)swprintf_s(ntpath, *ccntpath, L"%s", src);

				if (dstlength != -1)
				{
					bRet = TRUE;
				}
				else
				{
					dstlength = 0;
				}
			}
			else
			{
				dstlength = (ULONG)(wcslen(DeviceName) + wcslen(p));
			}

			break;
		}

		if (src[0] == L'\\' &&
			src[1] == L'\\' &&
			src[2] == L'.' &&
			src[3] == L'\\')
		{
			dstlength = 0;
			break;
		}

		//
		// skip \\?\ prefix
		// 
		if (src[0] == L'\\' &&
			src[1] == L'\\' &&
			src[2] == L'?' &&
			src[3] == L'\\')
		{
			p += 4;
		}

		srclength = (ULONG)wcslen(p);

		if (srclength < 2)
		{
			dstlength = 0;
			break;
		}

		//
		// p point to C:\\path\filename.ext
		//			  ^
		//			  |
		//			  p
		//

		memcpy(DosDrive,
			   p,
			   2 * sizeof(WCHAR));

		p += 2;

		//
		// p point to C:\\path\filename.ext
		//				^
		//				|
		//				p
		//


		CharCount = QueryDosDeviceW(DosDrive, 
								    DeviceName, 
									(sizeof(DeviceName) / sizeof(WCHAR)) - 1);

		if (CharCount == 0)
		{
			//
			// not a DOS path
			//
			dstlength = 0;
		}

		if (*ccntpath > wcslen(DeviceName) + wcslen(p))
		{
			dstlength = (LONG)swprintf_s(ntpath, *ccntpath, L"%s%s", DeviceName, p);

			if (dstlength != -1)
			{
				bRet = TRUE;
			}
			else
			{
				dstlength = 0;
			}
		}
		else
		{
			dstlength = (ULONG)(wcslen(DeviceName) + wcslen(p));
		}

	} while (FALSE);

	*ccntpath = dstlength;

	return bRet;
}

BOOL InitializeExplorerRenameHook(void)
{
	BOOL bRet = TRUE;

	HMODULE hkernel32 = NULL;

	do 
	{
		if (!Global.IsWin10)
			hkernel32 = GetModuleHandleW(KERNEL32_MODULE_NAME);
		else
			hkernel32 = GetModuleHandleW(KERNELBASE_MODULE_NAME);

		if (!hkernel32)
		{
			bRet = FALSE;
			break;
		}

		g_fnorg_MoveFileExW = (MOVEFILEEXW)GetProcAddress(hkernel32, SAVEASCTL_MOVEFILEEXW_PROC_NAME);

		if (g_fnorg_MoveFileExW)
		{
			if (!install_hook(g_fnorg_MoveFileExW, (PVOID*)&g_fn_MoveFileExW_trampoline, Core_MoveFileExW))
			{
				bRet = FALSE;
				break;
			}
		}

	} while (FALSE);

	return bRet;
}

void CleanupExplorerRenameHook(void)
{
	if (g_fn_MoveFileExW_trampoline)
	{
		remove_hook(g_fn_MoveFileExW_trampoline);
		g_fn_MoveFileExW_trampoline = NULL;
	}
}

BOOL WINAPI Core_MoveFileExW(
	_In_     LPCWSTR lpExistingFileName,
	_In_opt_ LPCWSTR lpNewFileName,
	_In_     DWORD    dwFlags
	)
{
	BOOL bRet = TRUE;

	ULONG RenameThreadIdOnStack = 0;

	ULONG DstLength = 0;
	ULONG SrcLength = 0;

	WCHAR *p = NULL;
	WCHAR *pSrcDir = NULL;
	WCHAR *pDstDir = NULL;

	WCHAR DstNXLFileName[MAX_PATH] = {0};
	WCHAR SrcNXLFileName[MAX_PATH] = {0};

	ULONGLONG RightsMask = 0;
	ULONGLONG CustomRights = 0;
	ULONGLONG EvaluationId = 0;

	WIN32_FIND_DATA FindFileData = {0};

	HANDLE hFile = INVALID_HANDLE_VALUE;

	do 
	{
		if (lpNewFileName == NULL ||
			lpExistingFileName == NULL)
		{
			break;
		}

		DstLength = (ULONG)wcslen(lpNewFileName);
		SrcLength = (ULONG)wcslen(lpExistingFileName);

		if (DstLength < sizeof(L".nxl") || DstLength > (sizeof(DstNXLFileName)/sizeof(WCHAR) - sizeof(L".nxl")/sizeof(WCHAR)))
		{
			break;
		}

		if (SrcLength < sizeof(L".nxl") || SrcLength > (sizeof(SrcNXLFileName)/sizeof(WCHAR) - sizeof(L".nxl")/sizeof(WCHAR)))
		{
			break;
		}

		pDstDir = (WCHAR*)wcsrchr(lpNewFileName, L'\\');
		pSrcDir = (WCHAR*)wcsrchr(lpExistingFileName, L'\\');

		if (pDstDir == NULL || 
			pSrcDir == NULL)
		{
			break;
		}
		
		memcpy(DstNXLFileName,
			   lpNewFileName,
			   (ULONG)((wcslen(lpNewFileName) - wcslen(pDstDir) + 1)*sizeof(WCHAR)));

		memcpy(SrcNXLFileName,
			   lpExistingFileName,
			   (ULONG)((wcslen(lpExistingFileName) - wcslen(pSrcDir) + 1)*sizeof(WCHAR)));

		if (_wcsicmp(DstNXLFileName, SrcNXLFileName) != 0)
		{
			break;
		}

		p = (WCHAR*)(lpNewFileName + DstLength - sizeof(L".nxl")/sizeof(WCHAR) + 1);

		if (_wcsicmp(p, L".nxl") == 0)
		{
			break;
		}

		p = (WCHAR *)(lpExistingFileName + SrcLength - sizeof(L".nxl")/sizeof(WCHAR) + 1);

		if (_wcsicmp(p, L".nxl") == 0)
		{
			break;
		}

		if (!init_rm_section_safe())
		{
			break;
		}

		if (S_OK != NX::NXL::RAW::CheckRights(lpExistingFileName, &RightsMask, &CustomRights, &EvaluationId))
		{
			break;
		}

		memset(DstNXLFileName, 0, sizeof(DstNXLFileName));
		memset(SrcNXLFileName, 0, sizeof(SrcNXLFileName));

		swprintf_s(DstNXLFileName,
				   sizeof(DstNXLFileName)/sizeof(WCHAR),
				   L"%s%s",
				   lpNewFileName,
				   L".nxl");

		swprintf_s(SrcNXLFileName,
				   sizeof(SrcNXLFileName)/sizeof(WCHAR),
				   L"%s%s",
				   lpExistingFileName,
				   L".nxl");

		bRet = g_fn_MoveFileExW_trampoline(SrcNXLFileName, DstNXLFileName, dwFlags);

		if (bRet)
		{
			hFile = FindFirstFileW(DstNXLFileName, &FindFileData);
		}
		
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
		}

		return bRet;

	} while (FALSE);

	return g_fn_MoveFileExW_trampoline(lpExistingFileName, lpNewFileName, dwFlags);
}

BOOL InitializeCopyFileExWHook(void)
{
	BOOL bRet = TRUE;

	HMODULE hkernel32 = NULL;

	do 
	{
		hkernel32 = GetModuleHandleW(KERNELBASE_MODULE_NAME);

		if (!hkernel32)
		{
			bRet = FALSE;
			break;
		}

		if (Global.CopyFileExWHookInitialized)
		{
			break;
		}

		g_fnorg_CopyFileExW = (COPYFILEEXW)GetProcAddress(hkernel32, SAVEASCTL_COPYFILEEXW_PROC_NAME);

		if (g_fnorg_CopyFileExW)
		{
			if (!install_hook(g_fnorg_CopyFileExW, (PVOID*)&g_fn_CopyFileExW_trampoline, Core_CopyFileExW))
			{
				bRet = FALSE;
				break;
			}
		}

		Global.CopyFileExWHookInitialized = TRUE;

	} while (FALSE);

	return bRet;
}

void CleanupCopyFileExWHook(void)
{
	if (g_fn_CopyFileExW_trampoline)
	{
		remove_hook(g_fn_CopyFileExW_trampoline);
		g_fn_CopyFileExW_trampoline = NULL;
	}
}

BOOL WINAPI	Core_CopyFileExW(
	_In_        LPCWSTR lpExistingFileName,
	_In_        LPCWSTR lpNewFileName,
	_In_opt_    LPPROGRESS_ROUTINE lpProgressRoutine,
	_In_opt_    LPVOID lpData,
	_When_(pbCancel != NULL, _Pre_satisfies_(*pbCancel == FALSE))
	_Inout_opt_ LPBOOL pbCancel,
	_In_        DWORD dwCopyFlags
	)
{
	ULONGLONG RightsMask = 0;
	ULONGLONG CustomRights = 0;
	ULONGLONG EvaluationId = 0;

	BOOL bDenyCopy = FALSE;

	do 
	{
		if (!init_rm_section_safe())
		{
			break;
		}

		if (S_OK != NX::NXL::RAW::CheckRights(lpExistingFileName, &RightsMask, &CustomRights, &EvaluationId))
		{
			break;
		}

		if (!(RightsMask & BUILTIN_RIGHT_SAVEAS))
		{
			bDenyCopy = TRUE;
			break;
		}

		forecast_saveas_to_engine(lpExistingFileName, lpNewFileName);

	} while (FALSE);

	if (bDenyCopy)
	{
		send_block_notification_ex(lpExistingFileName, lpNewFileName, NxrmdrvSaveFileBlocked);
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	else
	{
		return g_fn_CopyFileExW_trampoline(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
	}
}

HRESULT WINAPI Core_CopyFile2(
	_In_      PCWSTR                          pwszExistingFileName,
	_In_      PCWSTR                          pwszNewFileName,
	_In_opt_  COPYFILE2_EXTENDED_PARAMETERS   *pExtendedParameters
)
{
	ULONGLONG RightsMask = 0;
	ULONGLONG CustomRights = 0;
	ULONGLONG EvaluationId = 0;

	std::wstring szNewFileNameWithNXLExtension;

	HRESULT hr = S_OK;

	HMODULE hShell32 = NULL;

	SHCHANGENOTIFY	fn_SHChangeNotify = NULL;

	do
	{
		if (!init_rm_section_safe())
		{
			break;
		}

		if (NULL == pwszExistingFileName ||
			NULL == pwszNewFileName)
		{
			break;
		}

		if (S_OK != NX::NXL::RAW::CheckRights(pwszExistingFileName, &RightsMask, &CustomRights, &EvaluationId))
		{
			break;
		}

		if (is_file_name_end_with_nxl(pwszNewFileName))
		{
			break;
		}
		
		szNewFileNameWithNXLExtension = pwszNewFileName;
		szNewFileNameWithNXLExtension += SAVEASCTL_NXL_EXT;

		hr = g_fn_CopyFile2_trampoline(pwszExistingFileName, szNewFileNameWithNXLExtension.c_str(), pExtendedParameters);

		if (S_OK == hr)
		{
			hShell32 = GetModuleHandleW(SHELL32_MODULE_NAME);

			if (hShell32)
			{
				fn_SHChangeNotify = (SHCHANGENOTIFY)GetProcAddress(hShell32, NXRMCORE_SHCHANGENOTIFY_PROC_NAME);
			}
			
			if (fn_SHChangeNotify)
			{
				fn_SHChangeNotify(SHCNE_RENAMEITEM,
								  SHCNF_PATH,
								  pwszNewFileName,
								  szNewFileNameWithNXLExtension.c_str());

				fn_SHChangeNotify(SHCNE_UPDATEITEM,
					              SHCNF_PATH,
								  pwszNewFileName,
								  NULL);
			}
		}

		return hr;

	} while (FALSE);

	return g_fn_CopyFile2_trampoline(pwszExistingFileName, pwszNewFileName, pExtendedParameters);
}

BOOL send_block_notification_ex(const WCHAR *SrcFileName, const WCHAR *DstFileName, BLOCK_NOTIFICATION_TYPE Type)
{
	BOOL bRet = TRUE;

	BLOCK_NOTIFICATION_REQUEST Req = {0};

	NXCONTEXT Ctx = NULL;

	ULONG bytesret = 0;

	do 
	{
		Req.ProcessId	= GetCurrentProcessId();
		Req.ThreadId	= GetCurrentThreadId();
		Req.SessionId	= Global.SessionId;
		Req.LanguageId	= GetSystemDefaultLangID();
		Req.Type		= Type;

		if (SrcFileName)
		{
			memcpy(Req.FileName,
				   SrcFileName,
				   min(sizeof(Req.FileName) - sizeof(WCHAR), wcslen(SrcFileName)*sizeof(WCHAR)));
		}
		else
		{
			memset(Req.FileName, 0, sizeof(Req.FileName));
		}

		if (DstFileName)
		{
			memcpy(Req.FileName2,
				   DstFileName,
				   min(sizeof(Req.FileName2) - sizeof(WCHAR), wcslen(DstFileName)*sizeof(WCHAR)));
		}
		else
		{
			memset(Req.FileName2, 0, sizeof(Req.FileName2));
		}

		Ctx = submit_request(Global.Section, NXRMDRV_MSG_TYPE_BLOCK_NOTIFICATION, &Req, sizeof(Req));

		if (!Ctx)
		{
			break;
		}

		if (!wait_for_response(Ctx, Global.Section, NULL, 0, &bytesret))
		{
			Ctx = NULL;
			bRet = FALSE;
			break;
		}

		Ctx = NULL;

	} while (FALSE);

	return bRet;

}

static BOOL is_file_name_end_with_nxl(const WCHAR *FileName)
{
	BOOL bRet = FALSE;

	ULONG FileNameLength = 0;

	const WCHAR *p = NULL;

	do
	{
		FileNameLength = (ULONG)wcslen(FileName);

		if (FileNameLength < sizeof(SAVEASCTL_NXL_EXT) / sizeof(WCHAR))
		{
			break;
		}

		p = FileName + FileNameLength;

		p -= (sizeof(SAVEASCTL_NXL_EXT) / sizeof(WCHAR) - 1);

		bRet = _wcsicmp(p, SAVEASCTL_NXL_EXT) == 0 ? TRUE : FALSE;

	} while (FALSE);

	return bRet;
}

static BOOL is_path_end_with_slash(const WCHAR *FileName)
{
	BOOL bRet = FALSE;

	ULONG FileNameLength = 0;

	const WCHAR *p = NULL;

	do
	{
		FileNameLength = (ULONG)wcslen(FileName);

		if (FileNameLength < sizeof(L"\\") / sizeof(WCHAR))
		{
			break;
		}

		p = FileName + FileNameLength;

		p -= (sizeof(L"\\") / sizeof(WCHAR) - 1);

		bRet = _wcsicmp(p, L"\\") == 0 ? TRUE : FALSE;

	} while (FALSE);

	return bRet;
}

int WINAPI Core_SHFileOperationW(_Inout_ LPSHFILEOPSTRUCTW lpFileOp)
{
	int nRet = 0;

	ULONGLONG RightsMask = 0;
	ULONGLONG CustomRights = 0;
	ULONGLONG EvaluationId = 0;

	std::wstring szNewFileNameWithNXLExtension;
	std::wstring szNewFileNameWithoutNXLExtension;

	BOOL OnlyOneFileInFrom = FALSE;

	WCHAR *p = NULL;

	DWORD dwToFileAttr = INVALID_FILE_ATTRIBUTES;

	HMODULE hShell32 = NULL;

	SHCHANGENOTIFY	fn_SHChangeNotify = NULL;

	do
	{
		if (!init_rm_section_safe())
		{
			break;
		}

		if (NULL == lpFileOp)
		{
			break;
		}

		if (lpFileOp->pFrom == NULL ||
			lpFileOp->pTo == NULL ||
			lpFileOp->fFlags & FOF_MULTIDESTFILES ||
			lpFileOp->wFunc != FO_COPY)
		{
			break;
		}

		//
		// make sure only one file in pFrom
		//
		p = (WCHAR*)lpFileOp->pFrom;

		while (*p != L'\0')
		{
			p++;
		}
		
		if (*(++p) == L'\0')
		{
			OnlyOneFileInFrom = TRUE;
		}

		if (!OnlyOneFileInFrom)
		{
			break;
		}

		//
		// skip if file name has wild card
		//
		if (wcsstr(lpFileOp->pFrom, L"*.*"))
		{
			break;
		}

		//
		// skip if to is not a directory
		//
		if (dwToFileAttr == GetFileAttributesW(lpFileOp->pTo))
		{
			break;
		}

		if (!(dwToFileAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
			break;
		}

		//
		// make sure the from is a NXL file
		//
		if (S_OK != NX::NXL::RAW::CheckRights(lpFileOp->pFrom, &RightsMask, &CustomRights, &EvaluationId))
		{
			break;
		}

		p--;

		// make p point to the beginning of file name
		while ((ULONG_PTR)p > (ULONG_PTR)(lpFileOp->pFrom))
		{
			if (*p == L'\\')
			{
				p++;
				break;
			}

			--p;
		}

		if (is_file_name_end_with_nxl(lpFileOp->pFrom))
		{
			szNewFileNameWithNXLExtension = lpFileOp->pTo;
			
			if (!is_path_end_with_slash(lpFileOp->pTo))
			{
				szNewFileNameWithNXLExtension += L"\\";
			}
			
			szNewFileNameWithNXLExtension += p;
		}
		else
		{
			szNewFileNameWithNXLExtension = lpFileOp->pTo;

			if (!is_path_end_with_slash(lpFileOp->pTo))
			{
				szNewFileNameWithNXLExtension += L"\\";
			}

			szNewFileNameWithNXLExtension += p;
			szNewFileNameWithNXLExtension += SAVEASCTL_NXL_EXT;
		}

		szNewFileNameWithoutNXLExtension = szNewFileNameWithNXLExtension;
		szNewFileNameWithoutNXLExtension.resize(szNewFileNameWithNXLExtension.size() - (sizeof(SAVEASCTL_NXL_EXT) - sizeof(WCHAR)) / sizeof(WCHAR));

		nRet = g_fn_SHFileOperationW_trampoline(lpFileOp);

		//
		// rename file if success
		//

		if (g_fn_MoveFileExW_trampoline && nRet == 0)
		{
			BOOL bRet = TRUE;

			bRet = g_fn_MoveFileExW_trampoline(szNewFileNameWithoutNXLExtension.c_str(), 
											   szNewFileNameWithNXLExtension.c_str(),
											   MOVEFILE_REPLACE_EXISTING);
			if (bRet)
			{
				hShell32 = GetModuleHandleW(SHELL32_MODULE_NAME);

				if (hShell32)
				{
					fn_SHChangeNotify = (SHCHANGENOTIFY)GetProcAddress(hShell32, NXRMCORE_SHCHANGENOTIFY_PROC_NAME);
				}

				if (fn_SHChangeNotify)
				{
					fn_SHChangeNotify(SHCNE_RENAMEITEM,
									  SHCNF_PATH,
									  szNewFileNameWithoutNXLExtension.c_str(),
									  szNewFileNameWithNXLExtension.c_str());

					fn_SHChangeNotify(SHCNE_UPDATEITEM,
									  SHCNF_PATH,
									  szNewFileNameWithoutNXLExtension.c_str(),
									  NULL);
				}
			}
		}

		return nRet;

	} while (FALSE);

	return g_fn_SHFileOperationW_trampoline(lpFileOp);
}

int WINAPI Core_SHFileOperationWithAdditionalFlags(_Inout_ LPSHFILEOPSTRUCTW lpFileOp, _In_ ULONG Flags)
{
	int nRet = 0;

	ULONGLONG RightsMask = 0;
	ULONGLONG CustomRights = 0;
	ULONGLONG EvaluationId = 0;

	std::wstring szNewFileNameWithNXLExtension;
	std::wstring szNewFileNameWithoutNXLExtension;

	BOOL OnlyOneFileInFrom = FALSE;

	WCHAR *p = NULL;

	DWORD dwToFileAttr = INVALID_FILE_ATTRIBUTES;

	HMODULE hShell32 = NULL;

	SHCHANGENOTIFY	fn_SHChangeNotify = NULL;

	do 
	{
		if (Flags)
		{
			break;
		}

		if (!init_rm_section_safe())
		{
			break;
		}

		if (NULL == lpFileOp)
		{
			break;
		}

		if (lpFileOp->pFrom == NULL ||
			lpFileOp->pTo == NULL ||
			lpFileOp->fFlags & FOF_MULTIDESTFILES ||
			lpFileOp->wFunc != FO_COPY)
		{
			break;
		}

		//
		// make sure only one file in pFrom
		//
		p = (WCHAR*)lpFileOp->pFrom;

		while (*p != L'\0')
		{
			p++;
		}

		if (*(++p) == L'\0')
		{
			OnlyOneFileInFrom = TRUE;
		}

		if (!OnlyOneFileInFrom)
		{
			break;
		}

		//
		// skip if file name has wild card
		//
		if (wcsstr(lpFileOp->pFrom, L"*.*"))
		{
			break;
		}

		//
		// skip if to is not a directory
		//
		if (dwToFileAttr == GetFileAttributesW(lpFileOp->pTo))
		{
			break;
		}

		if (!(dwToFileAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
			break;
		}

		//
		// make sure the from is a NXL file
		//
		if (S_OK != NX::NXL::RAW::CheckRights(lpFileOp->pFrom, &RightsMask, &CustomRights, &EvaluationId))
		{
			break;
		}

		p--;

		// make p point to the beginning of file name
		while ((ULONG_PTR)p > (ULONG_PTR)(lpFileOp->pFrom))
		{
			if (*p == L'\\')
			{
				p++;
				break;
			}

			--p;
		}

		if (is_file_name_end_with_nxl(lpFileOp->pFrom))
		{
			szNewFileNameWithNXLExtension = lpFileOp->pTo;

			if (!is_path_end_with_slash(lpFileOp->pTo))
			{
				szNewFileNameWithNXLExtension += L"\\";
			}

			szNewFileNameWithNXLExtension += p;
		}
		else
		{
			szNewFileNameWithNXLExtension = lpFileOp->pTo;

			if (!is_path_end_with_slash(lpFileOp->pTo))
			{
				szNewFileNameWithNXLExtension += L"\\";
			}

			szNewFileNameWithNXLExtension += p;
			szNewFileNameWithNXLExtension += SAVEASCTL_NXL_EXT;
		}

		szNewFileNameWithoutNXLExtension = szNewFileNameWithNXLExtension;
		szNewFileNameWithoutNXLExtension.resize(szNewFileNameWithNXLExtension.size() - (sizeof(SAVEASCTL_NXL_EXT) - sizeof(WCHAR)) / sizeof(WCHAR));

		nRet = g_fn_SHFileOperationWithAdditionalFlags_trampoline(lpFileOp, Flags);

		//
		// rename file if success
		//

		if (g_fn_MoveFileExW_trampoline && nRet == 0)
		{
			BOOL bRet = TRUE;

			bRet = g_fn_MoveFileExW_trampoline(szNewFileNameWithoutNXLExtension.c_str(),
											   szNewFileNameWithNXLExtension.c_str(),
											   MOVEFILE_REPLACE_EXISTING);
			if (bRet)
			{
				hShell32 = GetModuleHandleW(SHELL32_MODULE_NAME);

				if (hShell32)
				{
					fn_SHChangeNotify = (SHCHANGENOTIFY)GetProcAddress(hShell32, NXRMCORE_SHCHANGENOTIFY_PROC_NAME);
				}

				if (fn_SHChangeNotify)
				{
					fn_SHChangeNotify(SHCNE_RENAMEITEM,
									  SHCNF_PATH,
									  szNewFileNameWithoutNXLExtension.c_str(),
									  szNewFileNameWithNXLExtension.c_str());

					fn_SHChangeNotify(SHCNE_UPDATEITEM,
									  SHCNF_PATH,
									  szNewFileNameWithoutNXLExtension.c_str(),
									  NULL);
				}
			}
		}

		return nRet;

	} while (FALSE);

	return g_fn_SHFileOperationWithAdditionalFlags_trampoline(lpFileOp, Flags);
}
