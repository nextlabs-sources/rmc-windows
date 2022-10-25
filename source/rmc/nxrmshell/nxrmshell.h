#pragma once
//#include <nx\rmc\session.h>
//#include <nx\common\stringex.h>
//#include <nx\common\time.h>


#ifdef NXRMSHELL_EXPORTS
#define NXRMSHELL_API __declspec(dllexport)
#else
#define NXRMSHELL_API __declspec(dllimport)
#endif

#ifndef CACHE_LINE
#define CACHE_LINE	64
#endif

#ifndef CACHE_ALIGN
#define CACHE_ALIGN	__declspec(align(CACHE_LINE))
#endif
//// {B4B506CC-112F-4739-B93B-F4F3869B6E3E}
//static const GUID <<name>> = 
//{ 0xb4b506cc, 0x112f, 0x4739, { 0xb9, 0x3b, 0xf4, 0xf3, 0x86, 0x9b, 0x6e, 0x3e } };

#define NXRMSHELL_NAME								L"SkyDRM"
#define NXRMSHELL_INSTALL_GUID_KEY					L"{B4B506CC-112F-4739-B93B-F4F3869B6E3E}"
#define NXRMSHELL_INSTALL_CLSID_KEY					L"CLSID\\{B4B506CC-112F-4739-B93B-F4F3869B6E3E}"
#define NXRMSHELL_INSTALL_INPROCSERVER32_KEY		L"CLSID\\{B4B506CC-112F-4739-B93B-F4F3869B6E3E}\\InprocServer32"
#define NXRMSHELL_INSTALL_CONTEXTMENUHANDLERS_KEY	L"*\\shellex\\ContextMenuHandlers"

#define NXRMSHELL_INSTALL_CLSID_KEY_CU					L"Software\\Classes\\CLSID\\{B4B506CC-112F-4739-B93B-F4F3869B6E3E}"
#define NXRMSHELL_INSTALL_INPROCSERVER32_KEY_CU			L"Software\\Classes\\CLSID\\{B4B506CC-112F-4739-B93B-F4F3869B6E3E}\\InprocServer32"
#define NXRMSHELL_INSTALL_CLASSES_KEY_CU				L"Software\\Classes"
#define NXRMSHELL_INSTALL_STAR_KEY_CU					L"Software\\Classes\\*"
#define NXRMSHELL_INSTALL_SHELLEX_KEY_CU				L"Software\\Classes\\*\\shellex"
#define NXRMSHELL_INSTALL_CONTEXTMENUHANDLERS_KEY_CU	L"Software\\Classes\\*\\shellex\\ContextMenuHandlers"

class Inxrmshell : public IClassFactory
{
public:
	Inxrmshell();
	~Inxrmshell();

	STDMETHODIMP QueryInterface(REFIID riid, void **ppobj);

	STDMETHODIMP_(ULONG) AddRef();

	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject);

	STDMETHODIMP LockServer(BOOL fLock);

private:
	ULONG				m_uRefCount;
	ULONG				m_uLockCount;
};

//extern std::shared_ptr<NX::RmSession> g_pSession;