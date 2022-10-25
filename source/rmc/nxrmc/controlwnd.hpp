
#pragma once
#ifndef __LOGON_CONTROL_HPP__
#define __LOGON_CONTROL_HPP__

#include <windows.h>
#include <shlobj.h>
#include <mshtml.h>
#include <Mshtmhst.h>

#include <string>


#define WM_WEBPAGE_SET_HEIGHT   (WM_USER + 200)
#define WM_WEBPAGE_URL_SUCCEEDED  (WM_USER + 201)

class ChangeMonitor : IHTMLChangeSink {
public:
	ChangeMonitor(IWebBrowser2 *_pWebBrowserOC) : _cRef(0), _pWebBrowser(_pWebBrowserOC) {};
	virtual HRESULT STDMETHODCALLTYPE Notify();

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);

	STDMETHOD_(DWORD, Release)();
	STDMETHOD_(ULONG, AddRef)();
	void SetPointer(IHTMLChangeLog *pLog, IMarkupContainer2* pMarkup)
	{
		changeLog = pLog;
		markup = pMarkup;
	}
protected:
    IWebBrowser2 *_pWebBrowser;
	IHTMLChangeLog *changeLog;
	IMarkupContainer2* markup;
	LONG _cRef;
};

DEFINE_GUID(CLSID_LogonControl,
    0x864b4d50, 0x3b9a, 0x11d2, 0xb8, 0xdb, 0x0, 0xc0, 0x4f, 0xa3, 0x47, 0x1c);

class CLogonControl : public IOleClientSite,
    public IOleInPlaceSite,
    public IDocHostUIHandler,
    public IDispatch
{
public:
    CLogonControl(HWND hWnd, LPCWSTR pSucceedURL = NULL);
    ~CLogonControl();

public:
    BOOL Create(HINSTANCE hInstance, HWND hParent, UINT id, int x, int y, int width, int height);

    void SetURL(const WCHAR* URL, const WCHAR * cid, const WCHAR * uid = NULL, const WCHAR * ticket = NULL);
    void Goto(const WCHAR* URL);
    LRESULT OnSize(void);
    HRESULT DeActivate(void);
	void SetQuitFlag(void) { _bQuitFlag = TRUE; }
	HRESULT TranslateAcceleratorIO(LPMSG lpMsg);

protected:
    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(DWORD, Release)();

    // IDispatch Methods
    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

    void OnNavigateError(BSTR bstrUrl, UINT statusCode, VARIANT_BOOL* bCancel);
    void OnBeforeNavigate2(BSTR bstrUrl, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, BSTR bstrHeaders, VARIANT_BOOL* bCancel);
    void OnNavigateComplete2(BSTR bstrUrl);
    void OnDocumentComplete(BSTR bstrUrl);
    void GetPageSize(IDispatch* pDoc, long* pWidth, long* pHeight);

    void registerMonitor();

    // IOleClientSite methods 
    STDMETHOD(SaveObject)();
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, LPMONIKER* ppmk);
    STDMETHOD(GetContainer)(LPOLECONTAINER* ppContainer);
    STDMETHOD(ShowObject)();
    STDMETHOD(OnShowWindow)(BOOL fShow);
    STDMETHOD(RequestNewObjectLayout)();

    // IOleWindow methods
    STDMETHOD(GetWindow)(HWND* phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

    // IOleInPlaceSite Methods
    STDMETHOD(CanInPlaceActivate)();
    STDMETHOD(OnInPlaceActivate)();
    STDMETHOD(OnUIActivate)();
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc,
        LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtent);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)();
    STDMETHOD(DiscardUndoState)();
    STDMETHOD(DeactivateAndUndo)();
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

    // IDocHostUIHandler Methods
    STDMETHOD(ShowContextMenu)(_In_ DWORD dwID, _In_  POINT *ppt, _In_  IUnknown *pcmdtReserved, _In_  IDispatch *pdispReserved);
    STDMETHOD(GetHostInfo)(_Inout_  DOCHOSTUIINFO *pInfo);
    STDMETHOD(ShowUI)(_In_  DWORD dwID, _In_  IOleInPlaceActiveObject *pActiveObject, _In_  IOleCommandTarget *pCommandTarget, _In_  IOleInPlaceFrame *pFrame, _In_  IOleInPlaceUIWindow *pDoc);
    STDMETHOD(HideUI)(void);
    STDMETHOD(UpdateUI)(void);
    STDMETHOD(EnableModeless)(BOOL fEnable);
    STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
    STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
    STDMETHOD(ResizeBorder)(_In_  LPCRECT prcBorder, _In_  IOleInPlaceUIWindow *pUIWindow, _In_  BOOL fRameWindow);
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
    STDMETHOD(GetOptionKeyPath)(_Out_  LPOLESTR *pchKey, DWORD dw);
    STDMETHOD(GetDropTarget)(_In_  IDropTarget *pDropTarget, _Outptr_  IDropTarget **ppDropTarget);
    STDMETHOD(GetExternal)(_Outptr_result_maybenull_  IDispatch **ppDispatch);
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, _In_  LPWSTR pchURLIn, _Outptr_  LPWSTR *ppchURLOut);
    STDMETHOD(FilterDataObject)(_In_  IDataObject *pDO, _Outptr_result_maybenull_  IDataObject **ppDORet);

protected:
    LONG _cRef;
	//ChangeMonitor *_pChangeSink;
	//IHTMLChangeSink *changeSink;
	//IHTMLChangeLog *changeLog;

private:
    HWND  _hWnd;
    DWORD _dwWBCookie;

	BOOL	_bQuitFlag;
    // Interface pointers
    IOleInPlaceObject* _pIOleIPObject;
    IOleObject*        _pIOleObject;
    IWebBrowser2*      _pWebBrowserOC;

    std::wstring       _current_url;
    LPCWSTR             _pSucceedURL;

	IOleInPlaceActiveObject* _pIOleIPActiveObject;

private:
    void Cleanup(void);

    // Event Helper Methods
    void AdviseWBEventSink(void);
    void UnadviseWBEventSink(void);
    HRESULT GetConnectionPoint(LPUNKNOWN pUnk, REFIID riid, LPCONNECTIONPOINT* pCP);
};

#endif