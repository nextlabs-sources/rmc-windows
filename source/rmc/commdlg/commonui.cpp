

#include <Windows.h>
//#include <assert.h>

//#include <iostream>
//#include <fstream>
//#include <string.h>

//#include <nudf\conversion.hpp>
//#include <nudf\json.hpp>
//#include <nudf\resutil.hpp>

#include <CommCtrl.h>
#include "dlgshare.hpp"
#include "dlgproject.hpp"
#include "dlgmanageusers.hpp"
#include "gdiplus.h"
#include "commondlg.hpp"

HINSTANCE _hResource = NULL;
HINSTANCE _hInstance = NULL;
DWORD     _dwLangId = LANG_NEUTRAL;                     // Language ID

HMODULE _reLib = NULL;

static HMODULE LoadResourceModule();


BOOL WINAPI DllMain(_In_ HINSTANCE hInstance, _In_ DWORD dwReason, _In_ LPVOID lpReserved)
{
    INITCOMMONCONTROLSEX icex;
    static ULONG_PTR m_gdiplusToken;

    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        _hInstance = hInstance;
        ::DisableThreadLibraryCalls((HMODULE)hInstance);

        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_STANDARD_CLASSES | ICC_LINK_CLASS | ICC_DATE_CLASSES | ICC_WIN95_CLASSES;
        InitCommonControlsEx(&icex);
        _reLib = LoadLibrary(L"riched20.dll");	//load the dll don't forget this
                                                //and don't forget to free it (see wm_destroy)
        //_hResource = LoadResourceModule();


        break;
    case DLL_PROCESS_DETACH:
        //FreeLibrary(_hResource);
        FreeLibrary(_reLib);
        
        //GdiPlusUnload
        //Gdiplus::GdiplusShutdown(m_gdiplusToken);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }

    return TRUE;
}


BOOL WINAPI RmuInitialize()
{
    return TRUE;
}

VOID WINAPI RmuDestroy()
{
    return;
}

static HMODULE LoadResourceModule()
{
    HMODULE hMod = NULL;
    WCHAR   wzPath[MAX_PATH] = { 0 };
    std::wstring wsMod1;
    std::wstring wsMod2;

    //TODO: Load Resource DLL
    WCHAR wzResDll[MAX_PATH + 1] = { 0 };
    std::wstring wsMenuText;

    GetModuleFileNameW(_hInstance, wzResDll, MAX_PATH);

    WCHAR* pos = wcsrchr(wzResDll, L'\\');
    if (pos == NULL) {
        return NULL;
    }

    *(pos + 1) = L'\0';
    wcsncat_s(wzResDll, MAX_PATH, L"nxrmres.dll", _TRUNCATE);

    HMODULE hRes = ::LoadLibraryW(wzResDll);
    return hRes;
}



VOID WINAPI RmuFreeResource(_In_ PVOID mem)
{
    if(NULL != mem) {
        free(mem);
    }
}


//CDlgShare dlg(L"", L"", L"");

LONG WINAPI  RmuShowShareDialog(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ NXSharingData &sharingIn, _Out_ NXSharingData *pSharingOut, _Out_ LPWSTR* ppwzClassifyTags)
{
    CDlgShare dlg(wzFile, sharingIn, pSharingOut);
    int nResult = dlg.DoModal(hParent);
    if (nResult == IDOK)
    {
		const wchar_t *pResultStr;
		int len = dlg.GetResult(&pResultStr);
		if (len > 0)
		{
			*ppwzClassifyTags = (LPWSTR)malloc(len * sizeof(wchar_t));
			memcpy(*ppwzClassifyTags, pResultStr, len * sizeof(wchar_t));
		}

        return 0;
    }
    return -1L;
}



LONG WINAPI  RmuShowFilePropertiesDialog(_In_opt_ HWND hParent, _In_ LPCWSTR wzFile, _In_ NXSharingData &sharingIn, _Out_ NXSharingData &sharingOut)
{
    CDlgShare dlg(wzFile, IDD_DIALOG_FILE_PROPERTIES, sharingIn);
    int nResult = dlg.DoModal(hParent);
    if (nResult == IDOK)
    {
        //int len = dlg.GetResult(&pResultStr);
        //if (len > 0)
        //{
        //    *ppwzClassifyTags = (LPWSTR)malloc(len * sizeof(wchar_t));
        //    memcpy(*ppwzClassifyTags, pResultStr, len * sizeof(wchar_t));
        //}

		//sharingOut = dlg.GetResult();
 
        return 0;
    }
    return -1L;
}


LONG WINAPI  RmuShowProjDialog(_In_opt_ HWND hParent, _Inout_ NXProjData &projIn, _Out_ LPWSTR* ppwzEmailAddrs)
{
    int dlgID = IDD_DIALOG_PROJ_PROPERTIES;
    if (projIn.projFlag == NX_PROJ_INVITE)
    {
        dlgID = IDD_DIALOG_PROJ_INVITES;
    }
    bool bEmptyValid = (dlgID == IDD_DIALOG_PROJ_PROPERTIES);
	CDlgProject dlg(projIn, dlgID, bEmptyValid);
	int nResult = dlg.DoModal(hParent);
	if (nResult == IDOK)
	{
		const wchar_t *pResultStr;
		int len = dlg.GetResult(&pResultStr);
		if (len > 0)
		{
			*ppwzEmailAddrs = (LPWSTR)malloc(len * sizeof(wchar_t));
			memcpy(*ppwzEmailAddrs, pResultStr, len * sizeof(wchar_t));
		}
        else
        {
            *ppwzEmailAddrs = NULL;
        }

		return 0;
	}
	return -1L;
}

LONG WINAPI  RmuShowManageUsersDialog(_In_opt_ HWND hParent, _In_  const LPWSTR pfilename, _In_ const std::vector<std::wstring> &peopleArr, _Out_ LPWSTR* ppwzEmailAddrs)
{
	CDlgManageUsers dlg(pfilename, peopleArr);
	int nResult = dlg.DoModal(hParent);
	if (nResult == IDOK)
	{
		const wchar_t *pResultStr;
		int len = dlg.GetResult(&pResultStr);
		if (len > 0)
		{
			*ppwzEmailAddrs = (LPWSTR)malloc(len * sizeof(wchar_t));
			memcpy(*ppwzEmailAddrs, pResultStr, len * sizeof(wchar_t));
		}
		else {
			*ppwzEmailAddrs = NULL;
		}

		return 0;
	}
	return -1L;
}


//void get_logon_user(std::wstring& user_name, std::wstring& user_id)
//{
//    std::wstring servresp;
//
//	try {
//		NX::json_value reqv = NX::json_value::create_object(std::vector<std::pair<std::wstring, NX::json_value>>({
//			std::pair<std::wstring, NX::json_value>(L"code", NX::json_value(9)),
//			std::pair<std::wstring, NX::json_value>(L"parameters", NX::json_null())
//		}), false);
//		servresp = NXSERV::NServRequest(reqv.serialize());
//
//		NX::json_value respv;
//		respv = NX::json_value::parse(servresp);
//		user_name = respv.as_object().at(L"data").as_object().at(L"logonUser").as_object().at(L"name").as_string();
//		user_id = NX::conversion::to_wstring(respv.as_object().at(L"data").as_object().at(L"logonUser").as_object().at(L"id").as_int32());
//	}
//	catch (NX::json_exception& e) {
//		UNREFERENCED_PARAMETER(e);
//		user_name.clear();
//		user_id.clear();
//	}
//}

const int Max_String_Len = 4096;

std::wstring LoadStringResource(int nID, const wchar_t *szDefaultString)
{
    if (!_hResource)
    {
        return szDefaultString;
    }
    std::wstring wsText = szDefaultString;
        //NX::RES::LoadMessage(_hResource, nID, Max_String_Len, _dwLangId, szDefaultString);
    return wsText;
}

std::wstring LoadStringResource(int nID, int resID)
{
    wchar_t szDefaultString[Max_String_Len];
    LoadString(_hInstance, resID, szDefaultString, Max_String_Len);

    //if (!_hResource)
    //{
        return szDefaultString;
    //}

    //std::wstring wsText = NX::RES::LoadMessage(_hResource, nID, Max_String_Len, _dwLangId, szDefaultString);
    //return wsText;
}

std::wstring LoadLocalStringResource(int resID)
{
    wchar_t szDefaultString[Max_String_Len];
    LoadString(_hInstance, resID, szDefaultString, Max_String_Len);

    return szDefaultString;
}
