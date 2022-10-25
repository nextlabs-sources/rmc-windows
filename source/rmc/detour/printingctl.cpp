#include "stdafx.h"
#include "nxrmcoreglobal.h"
#include "nxrmdrv.h"
#include "nxrmcorehlp.h"
#include "detour.h"
#include "printingctl.h"
#include "adobe.h"
#include "nudf\conversion.hpp"
#include "nudf\json.hpp"

#ifdef __cplusplus
extern "C" {
#endif

	extern	CORE_GLOBAL_DATA Global;

	extern	BOOL send_block_notification(const WCHAR *FileName, BLOCK_NOTIFICATION_TYPE Type);

	extern  BOOL update_active_document_from_protected_child_process(void);

	void send_printing_activity_log(const BOOL allow, const WCHAR *filename, const WCHAR *filepath, const LANGID LangId, const BOOL notify);

#ifdef __cplusplus
}
#endif

static STARTDOCW	g_fnorg_StartDocW = NULL;
static STARTDOCW	g_fn_StartDocW_trampoline = NULL;

static BOOL allow_process_to_print(void);

BOOL InitializePrintingCtlHook(void)
{
	BOOL bRet = TRUE;

	do 
	{
		if (!Global.gdi32Handle)
		{
			Global.gdi32Handle = GetModuleHandleW(GDI32_MODULE_NAME);
		}

		if (!Global.gdi32Handle)
		{
			bRet = FALSE;
			break;
		}

		if (Global.PrintingCtlHooksInitialized)
		{
			break;
		}

		g_fnorg_StartDocW = (STARTDOCW)GetProcAddress(Global.gdi32Handle, PRINTINGCTL_STARTDOCW_PROC_NAME);

		if (g_fnorg_StartDocW)
		{
			if (!install_hook(g_fnorg_StartDocW, (PVOID*)&g_fn_StartDocW_trampoline, (Global.IsMsOffice || Global.IsAdobeReader) ? Core_StartDocW : Core_StartDocW2))
			{
				bRet = FALSE;
				break;
			}
		}

		Global.PrintingCtlHooksInitialized = TRUE;

	} while (FALSE);

	return bRet;
}

void CleanupPrintingCtlHook(void)
{
	if (g_fn_StartDocW_trampoline)
	{
		remove_hook(g_fn_StartDocW_trampoline);
		g_fn_StartDocW_trampoline = NULL;
	}

	Global.PrintingCtlHooksInitialized = FALSE;
}

int WINAPI Core_StartDocW(_In_ HDC hdc, _In_ CONST DOCINFOW *lpdi)
{
	int nRet = 0;

	BOOL SkipOrignal = FALSE;

	WCHAR ActiveDoc[MAX_PATH] = {0};

	do 
	{
		if (!init_rm_section_safe())
		{
			break;
		}

		if (Global.DisablePrinting)
		{
			nRet = -1;
			SkipOrignal = TRUE;
			SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);

			EnterCriticalSection(&Global.ActiveDocFileNameLock);

			memcpy(ActiveDoc,
				   Global.ActiveDocFileName,
				   min(sizeof(ActiveDoc) - sizeof(WCHAR), sizeof(Global.ActiveDocFileName) - sizeof(WCHAR)));

			LeaveCriticalSection(&Global.ActiveDocFileNameLock);

			send_printing_activity_log(FALSE, ActiveDoc, ActiveDoc, GetUserDefaultLangID(), TRUE);

			break;
		}

		if (!Global.IsAdobeReader)
		{
			HRESULT hr = S_OK;

			ULONGLONG RightsMask = 0;
			ULONGLONG CustomRightsMask = 0;
			ULONGLONG EvaluationId = 0;

			EnterCriticalSection(&Global.ActiveDocFileNameLock);

			memcpy(ActiveDoc,
				   Global.ActiveDocFileName,
				   min(sizeof(ActiveDoc) - sizeof(WCHAR), sizeof(Global.ActiveDocFileName) - sizeof(WCHAR)));

			LeaveCriticalSection(&Global.ActiveDocFileNameLock);

			hr = NX::NXL::RAW::CheckRights(ActiveDoc, &RightsMask, &CustomRightsMask, &EvaluationId);

			if (S_OK == hr)
			{
				//
				// only send activity if it's NXL file
				send_printing_activity_log(TRUE, ActiveDoc, ActiveDoc, GetUserDefaultLangID(), FALSE);
			}

			break;
		}

		if (Global.AdobeMode == ADOBE_MODE_PARENT)
		{
			update_active_document_from_protected_child_process();
		}

		EnterCriticalSection(&Global.ActiveDocFileNameLock);

		memcpy(ActiveDoc,
			   Global.ActiveDocFileName,
			   min(sizeof(ActiveDoc) - sizeof(WCHAR), sizeof(Global.ActiveDocFileName) - sizeof(WCHAR)));

		LeaveCriticalSection(&Global.ActiveDocFileNameLock);

		if (wcslen(ActiveDoc) == 0 && wcsstr(Global.AdobeCommandLine, ADOBE_IEMODE_CMD_LINE) == NULL)
		{
			nRet = -1;
			SkipOrignal = TRUE;
			SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);

			send_printing_activity_log(FALSE, ActiveDoc, ActiveDoc, GetUserDefaultLangID(), TRUE);

			break;
		}

	} while (FALSE);

	if (!SkipOrignal)
	{
		nRet = g_fn_StartDocW_trampoline(hdc, lpdi);
	}

	return nRet;
}

int WINAPI Core_StartDocW2(_In_ HDC hdc, _In_ CONST DOCINFOW *lpdi)
{
	int nRet = 0;

	BOOL SkipOrignal = FALSE;

	WCHAR ActiveDoc[MAX_PATH] = { 0 };

	const WCHAR *pName = NULL;

	do
	{
		if (!init_rm_section_safe())
		{
			break;
		}

		if (allow_process_to_print())
		{
			break;
		}

		nRet = -1;
		SkipOrignal = TRUE;
		SetLastError(ERROR_ACCESS_DISABLED_BY_POLICY);

		pName = lpdi->lpszDocName ? lpdi->lpszDocName : L"";

		EnterCriticalSection(&Global.ActiveDocFileNameLock);

		memcpy(ActiveDoc,
			   pName,
			   min(sizeof(ActiveDoc) - sizeof(WCHAR), wcslen(pName) * sizeof(WCHAR)));

		LeaveCriticalSection(&Global.ActiveDocFileNameLock);

		send_printing_activity_log(FALSE, ActiveDoc, NULL, GetUserDefaultLangID(), TRUE);

	} while (FALSE);

	if (!SkipOrignal)
	{
		nRet = g_fn_StartDocW_trampoline(hdc, lpdi);
	}

	return nRet;
}

static BOOL allow_process_to_print(void)
{
	BOOL bRet = TRUE;

	CHECK_PROCESS_RIGHTS_REQUEST req = { 0 };
	CHECK_PROCESS_RIGHTS_RESPONSE resp = { 0 };

	NXCONTEXT Ctx = NULL;

	ULONG bytesret = 0;

	do 
	{
		req.ProcessId = GetCurrentProcessId();
		req.SessionId = Global.SessionId;

		Ctx = submit_request(Global.Section, NXRMDRV_MSG_TYPE_CHECK_PROCESS_RIGHTS, &req, sizeof(req));

		if (!Ctx)
		{
			break;
		}

		if (!wait_for_response(Ctx, Global.Section, (PVOID)&resp, sizeof(resp), &bytesret))
		{
			Ctx = NULL;

			break;
		}

		if (!(resp.Rights & BUILTIN_RIGHT_PRINT))
		{
			bRet = FALSE;
		}

	} while (FALSE);

	return bRet;
}

void send_printing_activity_log(const BOOL allow, const WCHAR *filename, const WCHAR *filepath, const LANGID LangId, const BOOL notify)
{
	const char format[] = {"{ \"code\": %d,\"parameters\": {\"operation\": 4,\"result\": %d,\"fileName\": \"%s\",\"filePath\": \"%s\",\"notifyUser\": true,\"languageId\": %hd}}"};
	
	QUERY_SERVICE_REQUEST req = { 0 };

	NXCONTEXT Ctx = NULL;

	ULONG bytesret = 0;

	do 
	{
		req.ProcessId	= GetCurrentProcessId();
		req.SessionId	= Global.SessionId;
		req.ThreadId	= GetCurrentThreadId();

		NX::json_value log = NX::json_value::create_object(std::vector<std::pair<std::wstring, NX::json_value>>({
					std::pair<std::wstring, NX::json_value>(L"code", NX::json_value((int)NXRMCORE_SERVICE_ID_ACTIVITY_LOG)),
					std::pair<std::wstring, NX::json_value>(L"parameters",
						NX::json_value::create_object(std::vector<std::pair<std::wstring, NX::json_value>>({
							std::pair<std::wstring, NX::json_value>(L"operation", NX::json_value((int)ACTIVITY_OPERATION_ID_PRINT)),
							std::pair<std::wstring, NX::json_value>(L"result", NX::json_value((int)(allow ? 1 : 0))),
							std::pair<std::wstring, NX::json_value>(L"fileName", NX::json_value(filename ? filename : L"")),
							std::pair<std::wstring, NX::json_value>(L"filePath", NX::json_value(filepath ? filepath : L"")),
							std::pair<std::wstring, NX::json_value>(L"notifyUser", NX::json_value((bool)(notify ? true : false))),
							std::pair<std::wstring, NX::json_value>(L"languageId", NX::json_value((int)(LangId))),
				}), false))
		}), false);

		const std::wstring &logstr = log.serialize();

		const std::string &logstr_utf8 = NX::conversion::utf16_to_utf8(logstr);

		memcpy(req.Data,
			   logstr_utf8.c_str(),
			   min(sizeof(req.Data) - sizeof(char), logstr_utf8.length()));

		Ctx = submit_request(Global.Section, NXRMDRV_MSG_TYPE_QUERY_SERVICE, &req, sizeof(req));

		if (!Ctx)
		{
			break;
		}

		if (!wait_for_response(Ctx, Global.Section, NULL, 0, &bytesret))
		{
			Ctx = NULL;
			break;
		}

		Ctx = NULL;

	} while (FALSE);

	return;
}