
// nxrmEventViewerDlg.h : header file
//

#pragma once
#include <windows.h>
#include <vector>
#include "..\uiutilities\uiutilities.hpp"
#include "dlgtemplate.hpp"
#include "commondlg.hpp"


// CnxrmEventViewerDlg dialog
using namespace uiUtilities;
using namespace std;



#define WM_NX_ADDEVENTLIST		WM_USER + 100
#define WM_NX_ADDFILTERLIST		WM_USER + 101
#define WM_NX_ADDNEWFILTERS     WM_USER + 102


bool OpenSplashWindow(HINSTANCE hinst, HWND hParentWnd);

void CloseProgressWindow();


class ProgressDlg : public CDlgTemplate
{
// Construction
public:
    ProgressDlg(HINSTANCE hInst, ProgressFuncs &funcs);	// standard constructor
    ~ProgressDlg();	// standard destructor

    static HWND m_staticHwnd;
	bool m_bMarquee;
// Dialog Data
//#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_PROGRESS };
//#endif

	protected:
	virtual void DoDataExchange();	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	NXCancel m_cancelFunc;
	ProgressFuncs &m_funcs;

	// Generated message map functions
	virtual BOOL OnInitialize();
    virtual BOOL OnPaint();
    virtual BOOL OnTimer(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnClosePogress();
	virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd);

	//DECLARE_MESSAGE_MAP()
public:
	virtual void OnDestroy();
	virtual void OnCancel();


};
