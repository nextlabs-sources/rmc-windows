// nxrmctxmenu.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "nxrmshell.h"
#include "ContextMenu.h"
#include "nxrmshellglobal.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	SHELL_GLOBAL_DATA Global;

	BOOL init_rm_section_safe(void);

#ifdef __cplusplus
}
#endif

//std::shared_ptr<NX::RmSession> g_pSession = nullptr;

extern "C" const GUID CLSID_IRmCtxMenu;


static HRESULT create_key_with_default_value(
	const HKEY	root,
	const WCHAR *parent, 
	const WCHAR *key, 
	const WCHAR *default_value);

static HRESULT set_value_content(
	const HKEY	root,
	const WCHAR *key, 
	const WCHAR *valuename, 
	const WCHAR *content);

static HRESULT delete_key(
	const HKEY	root,
	const WCHAR *parent,
	const WCHAR *key);

#define NXL_FILETYPE_KEY        NXL_FILE_EXTENTIONW
#define NXL_FILETYPE_APPKEY     L"NextLabs.Handler.1"



BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			Global.hModule = hModule;

			InitializeCriticalSection(&Global.SectionLock);

			DisableThreadLibraryCalls(hModule);

			break;
		}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			DeleteCriticalSection(&Global.SectionLock);
		}

		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

Inxrmshell::Inxrmshell()
{
	m_uRefCount		= 1;
	m_uLockCount	= 0;
}

Inxrmshell::~Inxrmshell()
{
	//g_pSession.reset();
}

STDMETHODIMP Inxrmshell::QueryInterface(REFIID riid, void **ppobj)
{
	HRESULT hRet = S_OK;

	void *punk = NULL;

	*ppobj = NULL;

	do 
	{
		if (IID_IClassFactory == riid)
		{
			punk = (ICallFactory *)this;
		}
		else if (IID_IUnknown == riid)
		{
			punk = (IUnknown *)this;
		}
		else
		{
			hRet = E_NOINTERFACE;
			break;
		}

		AddRef();

		*ppobj = punk;

	} while (FALSE);

	return hRet;
}

STDMETHODIMP Inxrmshell::CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject)
{
	HRESULT hr = S_OK;

	IRmCtxMenu *p = NULL;

	do 
	{
		if(pUnkOuter)
		{
			*ppvObject = NULL;
			hr = CLASS_E_NOAGGREGATION;
			break;
		}

		p = new IRmCtxMenu;

		if(!p)
		{
			*ppvObject = NULL;
			hr = E_OUTOFMEMORY;
			break;
		}

		InterlockedIncrement(&Global.ContextMenuInstanceCount);

		hr = p->QueryInterface(riid,ppvObject);

		p->Release();

	} while (FALSE);

	return hr;
}

STDMETHODIMP Inxrmshell::LockServer(BOOL fLock)
{
	if(fLock)
	{
		m_uLockCount++;
	}
	else
	{
		if(m_uLockCount > 0)
			m_uLockCount--;
	}

	return m_uLockCount;	
}

STDMETHODIMP_(ULONG) Inxrmshell::AddRef()
{
	m_uRefCount++;

	return m_uRefCount;
}

STDMETHODIMP_(ULONG) Inxrmshell::Release()
{
	ULONG uCount = 0;

	if(m_uRefCount)
		m_uRefCount--;

	uCount = m_uRefCount;

	if(!uCount && (m_uLockCount == 0))
	{
		delete this;
		InterlockedDecrement(&Global.nxrmshellInstanceCount);
	}

	return uCount;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT  hr = E_OUTOFMEMORY;

	Inxrmshell *Inxrmshellnstance = NULL;

	if(IsEqualCLSID(rclsid, CLSID_IRmCtxMenu))
	{
		Inxrmshellnstance = new Inxrmshell;

		if(Inxrmshellnstance)
		{
			InterlockedIncrement(&Global.nxrmshellInstanceCount);

			hr = Inxrmshellnstance->QueryInterface(riid,ppv);

			Inxrmshellnstance->Release();

		}
	}
	else
	{
		hr = CLASS_E_CLASSNOTAVAILABLE;
	}

	return(hr);
}

STDAPI DllCanUnloadNow(void)
{
	if(Global.nxrmshellInstanceCount == 0 && Global.ContextMenuInstanceCount == 0)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

STDAPI DllUnregisterServer(void)
{
	HRESULT h1 = S_OK;
	HRESULT h2 = S_OK;
	HRESULT h3 = S_OK;
	HRESULT h4 = S_OK;
	HRESULT h5 = S_OK;
	HRESULT h6 = S_OK;

	h1 = delete_key(HKEY_CLASSES_ROOT, NXRMSHELL_INSTALL_CLSID_KEY, L"InprocServer32");
	h2 = delete_key(HKEY_CLASSES_ROOT, L"CLSID", NXRMSHELL_INSTALL_GUID_KEY);
	h3 = delete_key(HKEY_CLASSES_ROOT, NXRMSHELL_INSTALL_CONTEXTMENUHANDLERS_KEY, NXRMSHELL_NAME);
	h4 = delete_key(HKEY_CURRENT_USER, NXRMSHELL_INSTALL_CLSID_KEY_CU, L"InprocServer32");
	h5 = delete_key(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID", NXRMSHELL_INSTALL_GUID_KEY);
	h6 = delete_key(HKEY_CURRENT_USER, NXRMSHELL_INSTALL_CONTEXTMENUHANDLERS_KEY_CU, NXRMSHELL_NAME);

    ::RegDeleteKeyW(HKEY_CLASSES_ROOT, NXL_FILETYPE_KEY);
    delete_key(HKEY_CLASSES_ROOT, L"NextLabs.Handler.1\\shell\\open", L"command");
    delete_key(HKEY_CLASSES_ROOT, L"NextLabs.Handler.1\\shell", L"open");
    delete_key(HKEY_CLASSES_ROOT, L"NextLabs.Handler.1", L"shell");
    delete_key(HKEY_CLASSES_ROOT, L"NextLabs.Handler.1", L"DefaultIcon");
    ::RegDeleteKeyW(HKEY_CLASSES_ROOT, NXL_FILETYPE_APPKEY);

	if(h1 == E_UNEXPECTED || h2 == E_UNEXPECTED || h3 == E_UNEXPECTED || h4 == E_UNEXPECTED || h5 == E_UNEXPECTED || h6 == E_UNEXPECTED)
	{
		return E_UNEXPECTED;
	}
	else
	{
		return S_OK;
	}
}

STDAPI DllRegisterServer(void)
{
	HRESULT nRet = S_OK;

	WCHAR module_path[MAX_PATH];

	memset(module_path,0,sizeof(module_path));

	do 
	{
		if(!GetModuleFileNameW(Global.hModule,
							   module_path,
							   sizeof(module_path)/sizeof(WCHAR)))
		{
			nRet = E_UNEXPECTED;
			break;
		}


        // First try to write the Registry keys for all users.

		nRet = create_key_with_default_value(HKEY_CLASSES_ROOT,
											 L"CLSID",
											 NXRMSHELL_INSTALL_GUID_KEY,
											 NXRMSHELL_NAME);

		if(S_OK == nRet)
		{
			nRet = create_key_with_default_value(HKEY_CLASSES_ROOT,
												 NXRMSHELL_INSTALL_CLSID_KEY,
												 L"InprocServer32",
												 module_path);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = set_value_content(HKEY_CLASSES_ROOT,
									 NXRMSHELL_INSTALL_INPROCSERVER32_KEY,
									 L"ThreadingModel",
									 L"Apartment");

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CLASSES_ROOT,
												 NXRMSHELL_INSTALL_CONTEXTMENUHANDLERS_KEY,
												 NXRMSHELL_NAME,
												 NXRMSHELL_INSTALL_GUID_KEY);

			if(S_OK != nRet)
			{
				break;
			}
		}
		else if (E_ACCESSDENIED == nRet)
		{

            // Since we don't have permission to write the Registry keys for
            // all users, try to write them for the current user.

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 L"Software\\Classes",
												 L"CLSID",
												 NULL);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 L"Software\\Classes\\CLSID",
												 NXRMSHELL_INSTALL_GUID_KEY,
												 NXRMSHELL_NAME);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 NXRMSHELL_INSTALL_CLSID_KEY_CU,
												 L"InprocServer32",
												 module_path);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = set_value_content(HKEY_CURRENT_USER,
									 NXRMSHELL_INSTALL_INPROCSERVER32_KEY_CU,
									 L"ThreadingModel",
									 L"Apartment");

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 NXRMSHELL_INSTALL_CLASSES_KEY_CU,
												 L"*",
												 NULL);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 NXRMSHELL_INSTALL_STAR_KEY_CU,
												 L"shellex",
												 NULL);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 NXRMSHELL_INSTALL_SHELLEX_KEY_CU,
												 L"ContextMenuHandlers",
												 NULL);

			if(S_OK != nRet)
			{
				break;
			}

			nRet = create_key_with_default_value(HKEY_CURRENT_USER,
												 NXRMSHELL_INSTALL_CONTEXTMENUHANDLERS_KEY_CU,
												 NXRMSHELL_NAME,
												 NXRMSHELL_INSTALL_GUID_KEY);

			if(S_OK != nRet)
			{
				break;
			}
		}
		else
		{
			break;
		}
	} while (FALSE);

	if(nRet != S_OK)
	{
		DllUnregisterServer();
	}

	return nRet;
}

static HRESULT create_key_with_default_value(
	const HKEY	root,
	const WCHAR *parent, 
	const WCHAR *key, 
	const WCHAR *default_value)
{
	HRESULT nRet = S_OK;

	HKEY hParent = NULL;
	HKEY hKey = NULL;

	do 
	{
		LONG lRet;

		if(ERROR_SUCCESS != (lRet = RegOpenKeyExW(root,
												  parent,
												  0,
												  KEY_WRITE,
												  &hParent)))
		{
			nRet = (lRet == ERROR_ACCESS_DENIED ? E_ACCESSDENIED : E_UNEXPECTED);
			break;
		}

		if(ERROR_SUCCESS != (lRet = RegCreateKey(hParent,
												 key,
												 &hKey)))
		{
			nRet = (lRet == ERROR_ACCESS_DENIED ? E_ACCESSDENIED : E_UNEXPECTED);
			break;
		}

		if(!default_value)
		{
			break;
		}

		if(ERROR_SUCCESS != (lRet = RegSetValueExW(hKey,
												   NULL,
												   0,
												   REG_SZ,
												   (const BYTE*)default_value,
												   (DWORD)(wcslen(default_value) + 1)*sizeof(WCHAR))))
		{
			nRet = (lRet == ERROR_ACCESS_DENIED ? E_ACCESSDENIED : E_UNEXPECTED);
			break;
		}

	} while (FALSE);

	if(hKey)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	if(hParent)
	{
		RegCloseKey(hParent);
		hParent = NULL;
	}

	return nRet;
}

static HRESULT set_value_content(
	const HKEY  root,
	const WCHAR *key, 
	const WCHAR *valuename, 
	const WCHAR *content)
{
	HRESULT nRet = S_OK;

	HKEY hKey = NULL;

	do 
	{
		if(ERROR_SUCCESS != RegOpenKeyExW(root,
										  key,
										  0,
										  KEY_SET_VALUE,
										  &hKey))
		{
			nRet = E_UNEXPECTED;
			break;
		}

		if(ERROR_SUCCESS != RegSetValueExW(hKey,
										   valuename,
										   0,
										   REG_SZ,
										   (const BYTE*)content,
										   (DWORD)(wcslen(content)+1)*sizeof(WCHAR)))
		{
			nRet = E_UNEXPECTED;
			break;
		}

	} while (FALSE);

	if(hKey)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return nRet;
}

static HRESULT delete_key(const HKEY root, const WCHAR *parent, const WCHAR *key)
{
	HRESULT nRet = S_OK;

	HKEY hKey = NULL;

	do 
	{
		LONG lRet;

		if(ERROR_SUCCESS != (lRet = RegOpenKeyExW(root,
												  parent,
												  0,
												  DELETE,
												  &hKey)))
		{
			nRet = (lRet == ERROR_FILE_NOT_FOUND || lRet == ERROR_ACCESS_DENIED ? HRESULT_FROM_WIN32(lRet) : E_UNEXPECTED);
			break;
		}

		if(ERROR_SUCCESS != (lRet = RegDeleteKeyW(hKey,key)))
		{
			nRet = (lRet == ERROR_FILE_NOT_FOUND || lRet == ERROR_ACCESS_DENIED ? HRESULT_FROM_WIN32(lRet) : E_UNEXPECTED);
			break;
		}

	} while (FALSE);

	if(hKey)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return nRet;
}

BOOL init_rm_section_safe(void)
{
	BOOL bRet = FALSE;

	do 
	{
		if(!Global.Section)
		{
			EnterCriticalSection(&Global.SectionLock);

			//TODO initalize to connect with UI
			Global.Section = NULL;// init_transporter_client(); 

			LeaveCriticalSection(&Global.SectionLock);
		}

		if(!Global.Section)
		{
			break;
		}
		//TODO
		bRet = FALSE;//is_transporter_enabled(Global.Section);

	} while (FALSE);

	return bRet;
}