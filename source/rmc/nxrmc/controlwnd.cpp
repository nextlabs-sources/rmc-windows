
#include "stdafx.h"
#include <Windows.h>
#include <assert.h>

#pragma warning(push)
#pragma warning(disable: 4091)
#include <mshtml.h>
#include <mshtmdid.h>
#include <ExDispID.h>
#include <Urlmon.h>

#include <Wininet.h>

//#include <boost/algorithm/string.hpp>
#include "weblogondlg.hpp"

#include "controlwnd.hpp"
#include "guiddef.h"
#pragma warning(pop)


CLogonControl::CLogonControl(HWND hWnd, LPCWSTR pSucceedURL)
	: _cRef(1),
	_hWnd(hWnd),
	_dwWBCookie(0),
	_pIOleIPObject(NULL),
	_pIOleIPActiveObject(NULL),
    _pIOleObject(NULL),
    _pWebBrowserOC(NULL),
    _pSucceedURL(pSucceedURL)
{
	_bQuitFlag = FALSE;
}

CLogonControl::~CLogonControl()
{
    Cleanup();
}

///////////////////////////////////////////////////////////////////////////
//
// IUnknown Methods
//
STDMETHODIMP CLogonControl::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown)) {
        *ppvObject = this;
    }
    else if (IsEqualIID(riid, IID_IOleWindow)) {
        *ppvObject = static_cast<IOleWindow*>(this);
    }
    else if (IsEqualIID(riid, IID_IOleClientSite)) {
        *ppvObject = static_cast<IOleClientSite*>(this);
    }
    else if (IsEqualIID(riid, IID_IOleInPlaceSite)) {
        *ppvObject = static_cast<IOleInPlaceSite*>(this);
    }
    else if (IsEqualIID(riid, IID_IDocHostUIHandler)) {
        *ppvObject = static_cast<IDocHostUIHandler*>(this);
    }
    else if (IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, DIID_DWebBrowserEvents2)) {
        *ppvObject = static_cast<IDispatch*>(this);
    }

    if (*ppvObject) {
        static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CLogonControl::AddRef()
{
// #ifdef _DEBUG
//     wchar_t str[256];
//     wsprintf(str, L"ADD controlWnd: %d\r\n", _cRef);
//     TRACE(str);
// #endif
    _cRef++;
    return _cRef;
}

STDMETHODIMP_(ULONG) CLogonControl::Release()
{
    ULONG uCount = 0;

    if (_cRef) {
        _cRef--;
    }

    uCount = _cRef;

    if (!uCount) {
        delete this;
    }
// #ifdef _DEBUG
// 	wchar_t str[256];
// 	wsprintf(str, L"RELEASE controlWnd: %d\r\n", _cRef);
// 	TRACE(str);
// #endif
    return uCount;
}

//STDMETHODIMP ChangeMonitor::QueryInterface(REFIID riid, void** ppvObject)
//	{
//		*ppvObject = NULL;
//
//		if (IsEqualIID(riid, IID_IUnknown)) {
//			*ppvObject = this;
//		}
//		else if (IsEqualIID(riid, IID_IHTMLChangeSink)) {
//			*ppvObject = static_cast<IHTMLChangeSink*>(this);
//		}
//
//		if (*ppvObject) {
//			static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
//			return S_OK;
//		}
//		return E_NOINTERFACE;
//	}
//
//STDMETHODIMP_(ULONG) ChangeMonitor::AddRef()
//	{
//		_cRef++;
//		return _cRef;
//	};
//
//STDMETHODIMP_(DWORD) ChangeMonitor::Release()
//{
//	ULONG uCount = 0;
//
//	if (_cRef) {
//		_cRef--;
//	}
//
//	uCount = _cRef;
//
//	if (!uCount) {
//		delete this;
//	}
//	return uCount;
//}
//
//HRESULT ChangeMonitor::Notify()
//{
//	BYTE szText[10000];
//		long lRecLen;
//		changeLog->GetNextChange(szText, 10000, &lRecLen);
//		//IHTMLElement* pDoc = NULL;
//		//markup->GetMasterElement(&pDoc);
//		//if (pDoc)
//		//{
//		//	BSTR inerHTML;
//		//	pDoc->get_innerHTML(&inerHTML);
//		//	BSTR outerHTML;
//		//	pDoc->get_outerHTML(&outerHTML);
//		//	BSTR inerText;
//		//	pDoc->get_innerText(&inerText);
//		//	BSTR outerText;
//		//	pDoc->get_innerText(&outerText);
//		//}
//        IDispatch *pDoc;
//        auto hr = _pWebBrowser->get_Document(&pDoc);
//        if (SUCCEEDED(hr))
//        {
//            IHTMLDocument2* pHTMLDoc2 = NULL;
//            IUnknown* unk;
//            IMarkupContainer2* markup;
//            IMarkupServices* services;
//
//
//            hr = pDoc->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc2);
//
//            IHTMLElement* pBody = NULL;
//            BSTR inerHTML;
//            pHTMLDoc2->get_body(&pBody);
//            pBody->get_innerHTML(&inerHTML);
//            TRACE(L"\n\ninerHTML*********************\n");
//            TRACE(inerHTML);
//            BSTR outerHTML;
//            TRACE(L"\n\nouterHTML*********************\n");
//            pBody->get_outerHTML(&outerHTML);
//            TRACE(outerHTML);
//            BSTR inerText;
//            TRACE(L"\n\ninerText*********************\n");
//            pBody->get_innerText(&inerText);
//            TRACE(inerText);
//            BSTR outerText;
//            TRACE(L"\n\nouterText*********************\n");
//            pBody->get_innerText(&outerText);
//            TRACE(outerText);
//        }
//
//    return 0;
//}
//
//void CLogonControl::registerMonitor()
//{
//	IDispatch *pDoc;
//   auto hr = _pWebBrowserOC->get_Document(&pDoc);
//   if (SUCCEEDED(hr))
//   {
//	   IHTMLDocument2* pHTMLDoc2 = NULL;
//	   IUnknown* unk;
//	   IMarkupContainer2* markup;
//	   IMarkupServices* services;
//
//
//	   hr = pDoc->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc2);
//	   auto ret1 = pDoc->QueryInterface(IID_IUnknown, (void**)&unk);
//	   //hr  = pDoc->QueryInterface(IID_IMarkupServices, (void **)&services); //4 
//	   auto ret2 = unk->QueryInterface(IID_IMarkupContainer2, (void**)&markup);
//
//	  
//	   changeSink = (IHTMLChangeSink *)_pChangeSink;
//	   changeLog = NULL;
//
//	   markup->CreateChangeLog(changeSink, &changeLog, 1, 1);
//	   _pChangeSink->SetPointer(changeLog, markup);
//   }
//}

//HRESULT Web2FromSite(IUnknown *punkSite, IWebBrowser2 **pWeb2) {
//	IServiceProvider* psp;
//	HRESULT hr = punkSite->QueryInterface(IID_IServiceProvider, (void **)&psp);
//	if (SUCCEEDED(hr))
//	{
//		hr = psp->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void **)pWeb2);
//		psp->Release();
//	}
//	return hr;
//}
//HRESULT Doc2FromWeb2(IWebBrowser2 *pWeb2, IHTMLDocument2 **ppDoc2) {
//	CComPtr<IDispatch> spDisp;
//	HRESULT hr = pWeb2->get_Document(&spDisp);
//	if (SUCCEEDED(hr) && spDisp)
//	{
//		hr = spDisp->QueryInterface(IID_IHTMLDocument2, (void**)ppDoc2);
//	}
//	else {
//		hr = E_FAIL;
//	}
//	return hr;
//}
//HRESULT Doc2FromSite(IUnknown *punkSite, IHTMLDocument2 **ppDoc2) {
//	CComPtr<IWebBrowser2> spWeb2;
//	HRESULT hr = Web2FromSite(punkSite, &spWeb2);
//	if (SUCCEEDED(hr)) {
//		hr = Doc2FromWeb2(spWeb2, ppDoc2);
//	}
//	return hr;
//}
//
//HRESULT ExecScriptOnDocComplete(
//	CComPtr<IWebBrowser2> & spIWebBrowser2, BSTR bstrScriptToExec) {
//	// get the IHTMLDocument2
//	CComPtr<IDispatch> spIDispatchDocument;
//	HRESULT hr = spIWebBrowser2->get_Document(&spIDispatchDocument);
//	if (FAILED(hr)) return hr;
//	CComPtr<IHTMLDocument2> spIHTMLDocument2;
//	hr = spIDispatchDocument.QueryInterface<IHTMLDocument2>(&spIHTMLDocument2);
//	if (FAILED(hr)) return hr;
//	// get the IHTMLWindow2
//	CComPtr<IHTMLWindow2> spIHTMLWindow2;
//	hr = spIHTMLDocument2->get_parentWindow(&spIHTMLWindow2);
//	if (FAILED(hr)) return hr;
//	// Execute the script
//	CComVariant ccomvariantRetVal;
//	hr = spIHTMLWindow2->execScript(bstrScriptToExec, CComBSTR(L"JavaScript"), &ccomvariantRetVal);
//	return hr;
//}

//////////////////////////////////////////////////////////////////////////
//
// IOleClientSite Methods
//
STDMETHODIMP CLogonControl::SaveObject()
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, LPMONIKER* ppmk)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetContainer(LPOLECONTAINER* ppContainer)
{
    *ppContainer = NULL;
    return E_NOINTERFACE;
}

STDMETHODIMP CLogonControl::ShowObject()
{
    return S_OK;
}

STDMETHODIMP CLogonControl::OnShowWindow(BOOL fShow)
{
    return S_OK;
}

STDMETHODIMP CLogonControl::RequestNewObjectLayout()
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetWindow(HWND *phwnd)
{
    *phwnd = _hWnd;
    return S_OK;
}

STDMETHODIMP CLogonControl::ContextSensitiveHelp(BOOL fEnterMode)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////
//
// IOleInPlaceSite Methods
//

STDMETHODIMP CLogonControl::CanInPlaceActivate(void)
{
    return S_OK;
}

STDMETHODIMP CLogonControl::OnInPlaceActivate(void)
{
    return S_OK;
}

STDMETHODIMP CLogonControl::OnUIActivate(void)
{
    return S_OK;
}

STDMETHODIMP CLogonControl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppIIPUIWin,
    LPRECT lprcPosRect, LPRECT lprcClipRect,
    LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    *ppFrame = NULL;
    *ppIIPUIWin = NULL;

    GetClientRect(_hWnd, lprcPosRect);
    GetClientRect(_hWnd, lprcClipRect);

    return S_OK;
}

STDMETHODIMP CLogonControl::Scroll(SIZE scrollExtent)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::OnUIDeactivate(BOOL fUndoable)
{

    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::OnInPlaceDeactivate(void)
{
    if (_pIOleIPObject) {
        _pIOleIPObject->Release();
        _pIOleIPObject = NULL;
    }

	if (_pIOleIPActiveObject) {
		_pIOleIPActiveObject->Release();
		_pIOleIPActiveObject = NULL;
	}

    return S_OK;
}

STDMETHODIMP CLogonControl::DiscardUndoState(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::DeactivateAndUndo(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::OnPosRectChange(LPCRECT lprcPosRect)
{
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////
//
// Private Methods
//

STDMETHODIMP CLogonControl::GetTypeInfoCount(UINT* pctinfo)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
    LCID lcid, DISPID* rgDispId)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS* pDispParams, VARIANT* pVarResult,
    EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    if (IID_NULL != riid)
        return DISP_E_UNKNOWNINTERFACE;

    if (!pDispParams)
        return DISP_E_PARAMNOTOPTIONAL;

    switch (dispIdMember)
    {
    case DISPID_BEFORENAVIGATE2:
        assert(7 == pDispParams->cArgs);
        assert((VT_BOOL | VT_BYREF) == pDispParams->rgvarg[0].vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[5].vt);
        assert((VT_BSTR) == pDispParams->rgvarg[5].pvarVal->vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[4].vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[3].vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[2].vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[1].vt);
        OnBeforeNavigate2(pDispParams->rgvarg[5].pvarVal->bstrVal,
            pDispParams->rgvarg[4].pvarVal,
            pDispParams->rgvarg[3].pvarVal,
            pDispParams->rgvarg[2].pvarVal,
            (pDispParams->rgvarg[1].pvarVal) ? pDispParams->rgvarg[1].pvarVal->bstrVal : NULL,
            pDispParams->rgvarg[0].pboolVal);
        break;

    case DISPID_NAVIGATEERROR:
        assert(5 == pDispParams->cArgs);
        assert((VT_BOOL | VT_BYREF) == pDispParams->rgvarg[0].vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[1].vt);
        assert((VT_I4) == pDispParams->rgvarg[1].pvarVal->vt);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[3].vt);
        assert((VT_BSTR) == pDispParams->rgvarg[3].pvarVal->vt);
        OnNavigateError(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[1].uintVal, pDispParams->rgvarg[0].pboolVal);
        break;

    case DISPID_NAVIGATECOMPLETE2:
        assert(2 == pDispParams->cArgs);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[0].vt);
        assert((VT_BSTR) == pDispParams->rgvarg[0].pvarVal->vt);
        OnNavigateComplete2(pDispParams->rgvarg[0].pvarVal->bstrVal);
        break;

    case DISPID_DOCUMENTCOMPLETE:
        assert(2 == pDispParams->cArgs);
        assert((VT_VARIANT | VT_BYREF) == pDispParams->rgvarg[0].vt);
        assert((VT_BSTR) == pDispParams->rgvarg[0].pvarVal->vt);
        OnDocumentComplete(pDispParams->rgvarg[0].pvarVal->bstrVal);
        break;
	case  DISPID_DOWNLOADBEGIN:
		TRACE(L"%s",L"DISPID_DOWNLOADBEGIN\n");
		break;
	case  DISPID_DOWNLOADCOMPLETE:
		TRACE(L"%s",L"DISPID_DOWNLOADCOMPLETE\n");
		break;

    default:
        return DISP_E_MEMBERNOTFOUND;
    }

    return S_OK;
}

void CLogonControl::OnNavigateError(BSTR bstrUrl, UINT statusCode, VARIANT_BOOL* bCancel)
{
}

void CLogonControl::OnBeforeNavigate2(BSTR bstrUrl, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, BSTR bstrHeaders, VARIANT_BOOL* bCancel)
{
    //std::wstring wsHeaders;
#ifdef _DEBUG
    std::wstring wsOrigHeaders;
    if (bstrHeaders) {
        wsOrigHeaders = bstrHeaders;
    }
#endif
	if (bstrUrl != NULL) {
		const _bstr_t wrapper(bstrUrl, true);
		std::wstring info((const wchar_t*)wrapper);
		BOOL bexternalurl = FALSE;
		if (wcsstr(info.c_str(), L"//signup.live.com/signup?"))
		{
			bexternalurl = TRUE;
		}
		else if (wcsstr(info.c_str(), L"//www.dropbox.com/forgot?email_from_login=")) {	
			bexternalurl = TRUE;
		}
		else if (wcsstr(info.c_str(), L"//accounts.google.com/signin/usernamerecovery?")) {
			bexternalurl = TRUE;
		}

		if (bexternalurl) {
			*bCancel = VARIANT_TRUE;
			ShellExecuteW(NULL, L"open", info.c_str(), NULL, NULL, SW_SHOWNORMAL);
			PostMessageW(GetParent(_hWnd), WM_CLOSE, 0, (LPARAM)info.c_str());
		}
		
	}

	if (_bQuitFlag){
		*bCancel = VARIANT_TRUE;
	}
}

void CLogonControl::OnNavigateComplete2(BSTR bstrUrl)
{

}

void CLogonControl::OnDocumentComplete(BSTR bstrUrl)
{
	if (bstrUrl != NULL) {
		const _bstr_t wrapper(bstrUrl, true);
		std::wstring info((const wchar_t*)wrapper);
		if (_pSucceedURL && wcsstr(info.c_str(), _pSucceedURL))
		{
			PostMessageW(GetParent(_hWnd), WM_WEBPAGE_URL_SUCCEEDED, 0, (LPARAM)info.c_str());
			return;
		}
	}

    HRESULT hr = S_OK;
    IDispatch *pDoc = NULL;
    hr = _pWebBrowserOC->get_Document(&pDoc);
    if (SUCCEEDED(hr) && NULL != pDoc) {

		//registerMonitor();

        long nWidth = 0, nHeight = 0;
        GetPageSize(pDoc, &nWidth, &nHeight);
        if (0 != nWidth && 0 != nHeight) {
            unsigned long param = nWidth & 0xFFFF;
            param <<= 16;
            param |= (nHeight & 0xFFFF);
            ::PostMessageW(GetParent(_hWnd), WM_WEBPAGE_SET_HEIGHT, (WPARAM)param, 0);
        }
//		pDoc->Release();
    }
}

void CLogonControl::GetPageSize(IDispatch* pDoc, long* pWidth, long* pHeight)
{
    HRESULT hr = S_OK;
    IHTMLDocument2* pHTMLDoc2 = NULL;

    hr = pDoc->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc2);
    if (SUCCEEDED(hr) && NULL != pHTMLDoc2) {
        IHTMLElement*   pBody = NULL;
        hr = pHTMLDoc2->get_body(&pBody);
        if (SUCCEEDED(hr) && NULL != pBody) {

            IHTMLElement2*   pBody2 = NULL;
            hr = pBody->QueryInterface(IID_IHTMLElement2, (void**)&pBody2);
            if (SUCCEEDED(hr) && NULL != pBody2) {
                pBody2->get_scrollWidth(pWidth);
                pBody2->get_scrollHeight(pHeight);
                pBody2->Release();
                pBody2 = NULL;
            }
            pBody->Release();
            pBody = NULL;
        }

        pHTMLDoc2->Release();
        pHTMLDoc2 = NULL;
    }
}

STDMETHODIMP CLogonControl::ShowContextMenu(_In_ DWORD dwID, _In_  POINT *ppt, _In_  IUnknown *pcmdtReserved, _In_  IDispatch *pdispReserved)
{
    return S_OK;
}

STDMETHODIMP CLogonControl::GetHostInfo(_Inout_  DOCHOSTUIINFO *pInfo)
{
    pInfo->cbSize = sizeof(DOCHOSTUIINFO);
    pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_DISABLE_HELP_MENU;
    // pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DISABLE_HELP_MENU;
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
    pInfo->pchHostCss = NULL;
    pInfo->pchHostNS = NULL;
    return S_OK;

    //return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::ShowUI(_In_  DWORD dwID, _In_  IOleInPlaceActiveObject *pActiveObject, _In_  IOleCommandTarget *pCommandTarget, _In_  IOleInPlaceFrame *pFrame, _In_  IOleInPlaceUIWindow *pDoc)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::HideUI(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::UpdateUI(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::EnableModeless(BOOL fEnable)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::OnDocWindowActivate(BOOL fActivate)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::OnFrameWindowActivate(BOOL fActivate)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::ResizeBorder(_In_  LPCRECT prcBorder, _In_  IOleInPlaceUIWindow *pUIWindow, _In_  BOOL fRameWindow)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetOptionKeyPath(_Out_  LPOLESTR *pchKey, DWORD dw)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetDropTarget(_In_  IDropTarget *pDropTarget, _Outptr_  IDropTarget **ppDropTarget)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::GetExternal(_Outptr_result_maybenull_  IDispatch **ppDispatch)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::TranslateUrl(DWORD dwTranslate, _In_  LPWSTR pchURLIn, _Outptr_  LPWSTR *ppchURLOut)
{
    return E_NOTIMPL;
}

STDMETHODIMP CLogonControl::FilterDataObject(_In_  IDataObject *pDO, _Outptr_result_maybenull_  IDataObject **ppDORet)
{
    return E_NOTIMPL;
}


///////////////////////////////////////////////////////////////////////////
//
// CLogOnUI::AdviseWBEventSink()
//
///////////////////////////////////////////////////////////////////////////

void CLogonControl::AdviseWBEventSink(void)
{
    assert(_pWebBrowserOC);

    if (_pWebBrowserOC)
    {
        LPCONNECTIONPOINT pCP = NULL;

        // Sink WebBrowser Events
        if (SUCCEEDED(GetConnectionPoint(_pWebBrowserOC, DIID_DWebBrowserEvents2, &pCP)))
        {
            pCP->Advise(static_cast<IDispatch*>(this), &_dwWBCookie);
            pCP->Release();
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//
// CLogOnUI::UnadviseWBEventSink()
//
///////////////////////////////////////////////////////////////////////////

void CLogonControl::UnadviseWBEventSink(void)
{
    assert(_pWebBrowserOC);

    if (_pWebBrowserOC)
    {
        LPCONNECTIONPOINT pCP = NULL;

        // Unadvise the WebBrowser Event Sink 
        if (_dwWBCookie && SUCCEEDED(GetConnectionPoint(_pWebBrowserOC, DIID_DWebBrowserEvents2, &pCP))) {
            pCP->Unadvise(_dwWBCookie);
            pCP->Release();
            --_dwWBCookie;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//
// CLogOnUI::GetConnectionPoint()
//
///////////////////////////////////////////////////////////////////////////

HRESULT CLogonControl::GetConnectionPoint(LPUNKNOWN pUnk, REFIID riid, LPCONNECTIONPOINT* ppCP)
{
    assert(_pWebBrowserOC);

    HRESULT hr = E_FAIL;
    IConnectionPointContainer* pCPC;

    if (_pWebBrowserOC) {
        hr = pUnk->QueryInterface(IID_IConnectionPointContainer, (LPVOID*)&pCPC);
        if (FAILED(hr)) {
            return hr;
        }
        hr = pCPC->FindConnectionPoint(riid, ppCP);
        pCPC->Release();
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////
//
// CLogOnUI::Cleanup()
//
// Description: This method releases all interfaces we are holding onto.
//              This has to be done here because Internet Explorer is not
//              releasing all of our interfaces.  Therefore, our ref count
//              never reaches 0 and we never delete ourself.
//
///////////////////////////////////////////////////////////////////////////

void CLogonControl::Cleanup(void)
{
    if (_pIOleIPObject) {
        _pIOleIPObject->Release();
        _pIOleIPObject = NULL;
    }

	if (_pIOleIPActiveObject) {
		_pIOleIPActiveObject->Release();
		_pIOleIPActiveObject = NULL;
	}

    if (_pWebBrowserOC) {
		_pWebBrowserOC->Stop();
        UnadviseWBEventSink();
        _pWebBrowserOC->Quit();
        _pWebBrowserOC->Release();
        _pWebBrowserOC = NULL;
    }

    if (_pIOleObject) {
        _pIOleObject->Release();
        _pIOleObject = NULL;
    }

    InternetSetOptionW(NULL, INTERNET_OPTION_END_BROWSER_SESSION, NULL, 0);
}

void CLogonControl::SetURL(const WCHAR* wzURL, const WCHAR *cid, const WCHAR * uid, const WCHAR * ticket)
{
    HRESULT hr = S_OK;

    do {

        Cleanup();

       const DWORD dwBehavior = 3 /*INTERNET_SUPPRESS_COOKIE_PERSIST*/;
       InternetSetOptionW(NULL, INTERNET_OPTION_SUPPRESS_BEHAVIOR, (LPVOID)&dwBehavior, sizeof(dwBehavior));
		if (NULL != cid && NULL!= uid && NULL != ticket) {
			WCHAR szDecodedUrl[INTERNET_MAX_URL_LENGTH];
			DWORD dwout = INTERNET_MAX_URL_LENGTH;
			if (S_OK == CoInternetParseUrl(wzURL, PARSE_DOMAIN, 0, szDecodedUrl, INTERNET_MAX_URL_LENGTH, &dwout, 0))
			{
				BOOL bret;
				std::wstring strurl = L"https://";
				strurl += szDecodedUrl;
				bret = InternetSetCookie(strurl.c_str(), L"userId", uid);
				bret = InternetSetCookie(strurl.c_str(), L"ticket", ticket);
				std::wstring strpid = L"0";
				bret = InternetSetCookie(strurl.c_str(), L"clientId", cid);
				bret = InternetSetCookie(strurl.c_str(), L"platformId", strpid.c_str());
			}
		}
		else if (NULL != cid) {
			WCHAR szDecodedUrl[INTERNET_MAX_URL_LENGTH];
			DWORD dwout = INTERNET_MAX_URL_LENGTH;
			if (S_OK == CoInternetParseUrl(wzURL, PARSE_DOMAIN, 0, szDecodedUrl, INTERNET_MAX_URL_LENGTH, &dwout, 0))
			{
				BOOL bret;
				std::wstring strurl = L"https://";
				strurl += szDecodedUrl;
				std::wstring strpid = L"0";
				bret = InternetSetCookie(strurl.c_str(), L"clientId", cid);
				bret = InternetSetCookie(strurl.c_str(), L"platformId", strpid.c_str());
			}
		}


        hr = CoCreateInstance(CLSID_WebBrowser,
            NULL,
            CLSCTX_INPROC,
            IID_IOleObject,
            (LPVOID*)&_pIOleObject);
        if (hr != S_OK)
            break;

        if (_pIOleObject->SetClientSite(this) != S_OK) {
            hr = E_FAIL;
            break;
        }

        // Get the rectangle of the client area
        RECT rcClient = { 0, 0, 0, 0 };
        GetClientRect(_hWnd, &rcClient);

        MSG msg;

        hr = _pIOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, &msg, this, 0, _hWnd, &rcClient);
        hr = _pIOleObject->QueryInterface(IID_IWebBrowser2, (LPVOID*)&_pWebBrowserOC);
        if (FAILED(hr)) {
            break;
        }

        if (_pWebBrowserOC) {
            // Disable menu bar & status bar
            VARIANT_BOOL varShow = VARIANT_FALSE;
            _pWebBrowserOC->put_MenuBar(VARIANT_FALSE);
            _pWebBrowserOC->put_AddressBar(VARIANT_FALSE);
            _pWebBrowserOC->put_StatusBar(VARIANT_FALSE);
            _pWebBrowserOC->put_ToolBar(FALSE);
            AdviseWBEventSink();
        }

        //
        // QI for the in-place object to set the size.
        //
        if (_pIOleIPObject) {
            _pIOleIPObject->Release();
            _pIOleIPObject = NULL;
        }

        hr = _pWebBrowserOC->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&_pIOleIPObject);
        if (FAILED(hr)) {
            break;
        }

        hr = _pIOleIPObject->SetObjectRects(&rcClient, &rcClient);

        // Navigate to the sample search page
        VARIANT vtHeaders;
        VARIANT vtEmpty;
        BSTR bstrURL = NULL;
        ::VariantInit(&vtEmpty);
        bstrURL = ::SysAllocString(wzURL);
        if (NULL == bstrURL) {
            hr = E_OUTOFMEMORY;
            break;
        }
        
        vtHeaders = vtEmpty;
        hr = _pWebBrowserOC->Navigate(bstrURL, &vtEmpty, &vtEmpty, &vtEmpty, &vtHeaders);
        ::SysFreeString(bstrURL);
        bstrURL = NULL;

		LPDISPATCH pDisp = NULL;

		hr = _pWebBrowserOC->get_Application(&pDisp);

		if (SUCCEEDED(hr)) {
			hr = pDisp->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID*)&_pIOleIPActiveObject);
			pDisp->Release();
		}
		
		//_pChangeSink = new ChangeMonitor(_pWebBrowserOC);

    } while (FALSE);

    if (hr != S_OK) {
        Cleanup();
    }

    return;
}

void CLogonControl::Goto(const WCHAR* URL)
{
    //const std::wstring user_agent_header(L"User-agent: Mozilla/5.0 (iPhone; CPU iPhone OS 9_3 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13E188a Safari/601.1\r\n");

    HRESULT hr = S_OK;
    VARIANT vtEmpty;
    VARIANT vtHeaders;
    BSTR bstrURL = NULL;

    if (NULL == URL) {
        return;
    }
    if (L'\0' == URL[0]) {
        return;
    }

    ::VariantInit(&vtEmpty);
    std::wstring header = L"Forwarded: userId=68;ticket=667B472906B29D08B346F89D49B621BE;platformId=0";
    COleVariant objHeader(header.c_str());
    if (CMainDlg::_logonType != NX_LOGON_SKYDRM)
    {
        vtHeaders = objHeader.Detach();
    }
    else
    {
        vtHeaders = vtEmpty;
    }
    bstrURL = ::SysAllocString(URL);
    if (NULL == bstrURL) {
        hr = E_OUTOFMEMORY;
        return;
    }

    hr = _pWebBrowserOC->Navigate(bstrURL, &vtEmpty, &vtEmpty, &vtEmpty, &vtHeaders);
    ::SysFreeString(bstrURL);
    bstrURL = NULL;
}

LRESULT CLogonControl::OnSize(void)
{
    HRESULT hr = E_FAIL;

    if (_pIOleIPObject) {
        RECT rcClient;
        GetClientRect(_hWnd, &rcClient);
        hr = _pIOleIPObject->SetObjectRects(&rcClient, &rcClient);
    }

    return hr;
}

HRESULT CLogonControl::DeActivate(void)
{
    HRESULT hr = S_OK;

    if (_pIOleIPObject && _pIOleObject) {
        hr = _pIOleIPObject->UIDeactivate();
        hr = _pIOleIPObject->InPlaceDeactivate();
        hr = _pIOleObject->Close(OLECLOSE_NOSAVE);
    }

    return hr;
}

BOOL CLogonControl::Create(HINSTANCE hInstance, HWND hParent, UINT id, int x, int y, int width, int height)
{
    _hWnd = ::CreateWindowW(L"logonctrl", L"", WS_CHILD | WS_VISIBLE, x, y, width, height, hParent, NULL, hInstance, this);
    return FALSE;
}

LRESULT WINAPI ControlWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        CLogonControl* p = (CLogonControl*)lParam;
        assert(NULL != p);
    }
        break;
    case WM_DESTROY:
        break;
    case WM_SIZE:
        break;
    default:
        lr = ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return lr;
}

HRESULT CLogonControl::TranslateAcceleratorIO(LPMSG lpMsg)
{
	HRESULT hr = S_OK;

	if (_pIOleIPActiveObject && lpMsg)
	{
		hr = _pIOleIPActiveObject->TranslateAccelerator(lpMsg);
	}

	return hr;
}

