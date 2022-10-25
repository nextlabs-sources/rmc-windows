

#ifndef __NXRM_COMMONUI_DLGTEMPLATE_HPP__
#define __NXRM_COMMONUI_DLGTEMPLATE_HPP__

#include <Windows.h>
#include <assert.h>
#include "uiutilities.hpp"


class CDlgTemplate 
{
public:
    CDlgTemplate();
	CDlgTemplate(HINSTANCE hInst);
	CDlgTemplate(_In_ UINT uDlgId);
    virtual ~CDlgTemplate();

    inline BOOL AutoRelease() const throw() {return _AutoRelease;}
    inline HWND GetSafeHwnd() throw() {return m_hWnd;}
	inline void SetDlgID(UINT dlgId) throw() { m_dlgId = dlgId;  }

    int DoModal(_In_opt_ HWND hParent);

    HWND DoModaless(_In_opt_ HWND hParent);

    virtual DLGPROC GetDlgProc() throw() {return CDlgTemplate::DlgProc;}
	virtual BOOL OnInitDialog() { DoDataExchange();  return TRUE; }
    virtual void OnOk() {_nResult=IDOK; _bModaless? ::DestroyWindow(m_hWnd) : ::EndDialog(m_hWnd, IDOK);}
    virtual void OnCancel() {_nResult=IDCANCEL; _bModaless ? ::DestroyWindow(m_hWnd) : ::EndDialog(m_hWnd, IDCANCEL);}
    virtual void OnDestroy() {}
    virtual BOOL OnClose() {return FALSE;}
    virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd) {UNREFERENCED_PARAMETER(notify);UNREFERENCED_PARAMETER(id);UNREFERENCED_PARAMETER(hwnd);return FALSE;}
    virtual BOOL OnSysCommand(WPARAM wParam, LPARAM lParam) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(lParam);return FALSE;}
    virtual BOOL OnTimer(_In_ UINT id, _In_opt_ TIMERPROC proc) {UNREFERENCED_PARAMETER(id);UNREFERENCED_PARAMETER(proc);return FALSE;}
    virtual BOOL OnHelp(_In_ LPHELPINFO info) {UNREFERENCED_PARAMETER(info);return FALSE;}
    virtual BOOL OnNotify(_In_ LPNMHDR lpnmhdr) {UNREFERENCED_PARAMETER(lpnmhdr);return FALSE;}
    
    virtual BOOL OnLButtonDown(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnLButtonUp(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnLButtonDblClk(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnRButtonDown(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnRButtonUp(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnRButtonDblClk(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnMButtonDown(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnMButtonUp(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnMButtonDblClk(WPARAM wParam, int x, int y) {UNREFERENCED_PARAMETER(wParam);UNREFERENCED_PARAMETER(x);UNREFERENCED_PARAMETER(y);return FALSE;}
    virtual BOOL OnVScroll(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return FALSE; }
    virtual BOOL OnMouseWheel(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return FALSE; }
    virtual BOOL OnTimer(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return FALSE; };
	virtual void OnSize(UINT nType, int cx, int cy) { UNREFERENCED_PARAMETER(nType); UNREFERENCED_PARAMETER(cx); UNREFERENCED_PARAMETER(cy); }
    virtual INT_PTR OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        UNREFERENCED_PARAMETER(uMsg); UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return FALSE;
    }
	virtual void OnGetMinMaxInfo(MINMAXINFO* lpMMI) { UNREFERENCED_PARAMETER(lpMMI); }

    virtual void OnPaint();
    virtual BOOL OnEraseBkGnd(HDC hDC) {UNREFERENCED_PARAMETER(hDC);return FALSE;}
    virtual BOOL OnGetText(WCHAR* text, UINT size) {UNREFERENCED_PARAMETER(text);UNREFERENCED_PARAMETER(size);return FALSE;}
    virtual BOOL OnSetText(const WCHAR* text) {UNREFERENCED_PARAMETER(text);return FALSE;}

    virtual void OnDrawItem(int nCtrlID, DRAWITEMSTRUCT* lpDrawItemStruct) {UNREFERENCED_PARAMETER(lpDrawItemStruct);}
    virtual BOOL OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItemStruct) {UNREFERENCED_PARAMETER(lpMeasureItemStruct);return FALSE;}

    virtual BOOL OnNcPaint(HRGN hRgn) {UNREFERENCED_PARAMETER(hRgn);return FALSE;}
    virtual LRESULT OnNcHitTest(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {return DefWindowProcW(hDlg, uMsg, wParam, lParam);}

	inline HICON SetIcon(HICON hIcon, BOOL bBigIcon)
	{
		return (HICON)::SendMessage(m_hWnd, WM_SETICON, bBigIcon, (LPARAM)hIcon);
	}
	inline void GetClientRect(LPRECT lpRect) const
	{
		::GetClientRect(m_hWnd, lpRect);
	}
	inline void GetWindowRect(LPRECT lpRect) const
	{
		::GetWindowRect(m_hWnd, lpRect);
	}
	inline void CheckRadioButton(
		_In_ int nIDFirstButton,
		_In_ int nIDLastButton,
		_In_ int nIDCheckButton)
	{
				::CheckRadioButton(m_hWnd,
				nIDFirstButton,
				nIDLastButton,
				nIDCheckButton);
	}

	inline void ScreenToClient(LPRECT lpRect) const
	{
		ASSERT(::IsWindow(m_hWnd));
		::ScreenToClient(m_hWnd, (LPPOINT)lpRect);
		::ScreenToClient(m_hWnd, ((LPPOINT)lpRect) + 1);
		//if (GetExStyle(m_hWnd) & WS_EX_LAYOUTRTL)
		//	CRect::SwapLeftRight(lpRect);
	}
protected:
    virtual INT_PTR MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    inline void SetHwnd(_In_ HWND hWnd) throw() {m_hWnd = hWnd;}
    static INT_PTR WINAPI DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	inline void DDX_Control(int nIDC, uiUtilities::CWnd& rControl) 
	{ 
		rControl.m_hWnd = GetDlgItem(m_hWnd, nIDC); 
	};
	virtual void DoDataExchange() {};

    
protected:
    HWND m_hWnd;
    UINT m_dlgId;
    BOOL _AutoRelease;
    int  _nResult;
    bool _bModaless;
	HINSTANCE _hInstance;
};

class CPropPageDlgTemplate : public CDlgTemplate
{
public:
    CPropPageDlgTemplate() : CDlgTemplate() {memset(&_psp, 0, sizeof(_psp)); _psp.dwSize=sizeof(PROPSHEETPAGEW);}
    CPropPageDlgTemplate(BOOL AutoRelease) : CDlgTemplate(AutoRelease) {memset(&_psp, 0, sizeof(_psp)); _psp.dwSize=sizeof(PROPSHEETPAGEW);}
    virtual ~CPropPageDlgTemplate() {}
    virtual DLGPROC GetDlgProc() throw() {return CPropPageDlgTemplate::PropPageDlgProc;}

    virtual void OnPsnSetActive(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnKillActive(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnApply(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnReset(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnHelp(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnWizBack(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnWizNext(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnWizFinish(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnQueryCancel(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnGetObject(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnTranslateAccelerator(_In_ LPPSHNOTIFY lppsn){};
    virtual void OnPsnQueryInitialFocus(_In_ LPPSHNOTIFY lppsn){};

    operator PROPSHEETPAGEW& () throw() {return _psp;}
    operator const PROPSHEETPAGEW& () const throw() {return _psp;}
    const PROPSHEETPAGEW& GetPage() const throw() {return _psp;}
    PROPSHEETPAGEW& GetPage() throw() {return _psp;}

protected:
    virtual INT_PTR MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static INT_PTR WINAPI PropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:    
    PROPSHEETPAGEW _psp; 
};


#endif