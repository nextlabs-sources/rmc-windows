
// nxrmEventViewerDlg.cpp : implementation file
//
#include <Unknwn.h>  
#include <windows.h>
#include "resource.h"
#include "progressDlg.h"
#include "..\uiutilities\uiutilities.hpp"
#include "dlgtemplate.hpp"
#include "gdiplus.h"
#include <string>
//#include <iostream>
//#include <fstream>
#include <Shlwapi.h>
#include "commonui.hpp"
#include "mfcutil2.h"
#include "commondlg.hpp"

//#include <thread>

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

using namespace uiUtilities;

// CAboutDlg dialog used for App About
using namespace std;

HWND ProgressDlg::m_staticHwnd;

static CProgressCtrl protectProgress;
HWND g_parentWnd;

//ProgressDlg *evtdlg = NULL;



void RmuProgressStep(int percent)
{
	protectProgress.SetStep(percent);
}


void CloseProgressWindow()
{
	if (ProgressDlg::m_staticHwnd)
	{
		PostMessage(ProgressDlg::m_staticHwnd, WM_COMMAND, IDCANCEL, 0l);
		EnableWindow(g_parentWnd, TRUE);

	}
}


bool WINAPI RmuOpenProgressWindow(HWND hParentWnd, ProgressFuncs &funcs, bool bMarquee)
{

	auto evtdlg = new ProgressDlg(_hInstance, funcs);
	if (evtdlg)
	{
		evtdlg->m_bMarquee = bMarquee;
		ProgressDlg::m_staticHwnd = evtdlg->DoModaless(hParentWnd);
        ShowWindow(ProgressDlg::m_staticHwnd, SW_SHOW);
        CenterWindow(hParentWnd, ProgressDlg::m_staticHwnd);
		funcs.pStepProgress = RmuProgressStep;
		funcs.closeProgress = CloseProgressWindow;
		if (funcs.bDisableParent)
		{
			EnableWindow(hParentWnd, FALSE);
		}
		g_parentWnd = hParentWnd;
		
    }
    return true;
}





ProgressDlg::ProgressDlg(HINSTANCE hInst, ProgressFuncs &funcs)
    : CDlgTemplate(IDD), m_funcs(funcs)

{
    //SetDlgID((UINT)IDD);funcs
}

ProgressDlg::~ProgressDlg()
{
}

void ProgressDlg::DoDataExchange()
{
}


BOOL ProgressDlg::OnInitialize()
{
    CDlgTemplate::OnInitialize();
    HWND hDlg = GetHwnd();
	SetWindowText(hDlg, m_funcs.titleText.c_str());
    protectProgress.m_hWnd = GetDlgItem(hDlg, IDC_PROGRESS);
    ShowWindow(protectProgress.m_hWnd, SW_SHOW);
	if (!m_bMarquee)
	{
		protectProgress.SetRange(0, 100);
		protectProgress.SetStep(1);

//		SetTimer(hDlg, 1, 100, NULL);
	}
	else
	{
		//protectProgress.SetState(PBST_NORMAL);
		protectProgress.SetMarquee(true, 10);
	}
    return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

BOOL ProgressDlg::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hDlg = GetHwnd();
    RECT rect;

    hdc = BeginPaint(hDlg, &ps);
    GetClientRect(hDlg, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    EndPaint(hDlg, &ps);
    return TRUE;
}

BOOL ProgressDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{
    //DestroyWindow(GetHwnd());
    protectProgress.StepIt();
    if (protectProgress.GetPos() == 100)
    {
        DestroyWindow(GetHwnd());
    }
    return FALSE;
}

BOOL ProgressDlg::OnClosePogress()
{
	CDlgTemplate::OnCancel();
	return 0;
}



void ProgressDlg::OnDestroy()
{
    m_staticHwnd = NULL;
	//m_funcs.cancelCallback();
	//delete this;
}

void ProgressDlg::OnCancel()
{
	CDlgTemplate::OnCancel();
	//MessageBox(m_staticHwnd, L"Close", L"", MB_OK);
}

BOOL ProgressDlg::OnCommand(WORD notify, WORD id, HWND hwnd)
{
	HWND hDlg = GetHwnd();

	switch (id)
	{
	case IDC_CANCEL:
		m_funcs.cancelCallback();
		break;
	}
	return CDlgTemplate::OnCommand(notify, id, hwnd);
}
