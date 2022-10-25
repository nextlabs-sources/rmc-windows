
#pragma once
#ifndef __MAIN_DIALOG_HPP__
#define __MAIN_DIALOG_HPP__

#include <string>

#include "controlwnd.hpp"

enum LogonType
{
    NX_LOGON_SKYDRM,
    NX_LOGON_OAUTH
};


class CLinkButton
{
public:
    CLinkButton();
    virtual ~CLinkButton();

    void Attach(HWND h);
    void Detach();

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnMouseMove();
    void OnMouseLeave();

private:
    HWND _h;
    WNDPROC _prev_wndproc;
    LONG_PTR _prev_userdata;
    bool _is_hand;
};

class CMainDlg
{
public:
    CMainDlg(HINSTANCE hInst, HWND hParent, UINT uDlgId, bool bIsWin10 = false);
    virtual ~CMainDlg();

    static INT_PTR CALLBACK InterDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

    inline HWND GetHwnd() const { return _hWnd; }

	VOID CenterWindow(HWND hWnd);

    INT_PTR LogOn(const std::wstring & authn_url, const std::wstring & succeed_url, const std::wstring &id);

	INT_PTR LogOnToProvider(const std::wstring & authn_url, const std::wstring & succeed_url, const std::wstring &cid, const std::wstring & uid, const std::wstring ticket);

		void DestroyDialog(int closeID);
	static int GetCloseID() { return _closeID; };
    static LogonType _logonType;

protected:
    virtual BOOL OnInitDialog(HWND hDlg);
    virtual void OnDestroy();
    virtual void OnAdjustPageSize(int cx, int cy);
    virtual void OnEraseBkgnd(HDC hDC);
    virtual void OnDrawItem(UINT uCtrlId, DRAWITEMSTRUCT* pDis);
    virtual void OnDrawBackButton(DRAWITEMSTRUCT* pDis);
    virtual void OnClickWebLogon();

    void InitDlgSize();
	void StopBrowsing(void);
    std::wstring get_server_from_url(LPCWSTR wzUrl);

    inline int get_init_dlg_cx() const { return _init_dlg_cx; }
    inline int get_init_dlg_cy() const { return _init_dlg_cy; }
    inline int get_init_browser_cx() const { return _init_browser_cx; }
    inline int get_init_browser_cy() const { return _init_browser_cy; }

private:
    HINSTANCE   _hInst;
    HWND        _hParent;
    HWND        _hWnd;
    UINT        _uDlgId;
    CLogonControl*  _pCtrl;
    std::wstring    _authn_url;
    HBRUSH  _hb_white;
    HPEN    _hp_blue;
    COLORREF _color_blue;
	static int _closeID;
	HHOOK _hHook;
	bool	_bIsWin10;

    std::wstring    _success_url;
	std::wstring	_domain;
	std::wstring	_userid;
	std::wstring	_clientid;
	std::wstring	_ticket;

    const int _init_dlg_cx;
    const int _init_dlg_cy;
    const int _init_browser_cx;
    const int _init_browser_cy;
    const int _init_button_cx;
    const int _init_button_cy;

    CLinkButton _back_button;
};




#endif