
// nxrmEventViewerDlg.cpp : implementation file
//
#include "stdafx.h"
#include <Unknwn.h>  
#include <windows.h>
#include "resource.h"
#include "splashDlg.h"
#include "..\..\nxrmc\uiutilities\uiutilities.hpp"
#include "..\..\nxrmc\uiutilities\dlgtemplate.hpp"
#include "gdiplus.h"
#include <string>
//#include <iostream>
//#include <fstream>
#include <Shlwapi.h>
//#include <thread>
#include "nxrmc.h"

extern APPUI g_APPUI; 
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

using namespace uiUtilities;

// CAboutDlg dialog used for App About
using namespace std;

HWND SplashDlg::m_staticHwnd;


bool OpenSplashWindow(HINSTANCE hinst, HWND hParentWnd)
{

	auto evtdlg = new SplashDlg(hinst);
	if (evtdlg)
	{
		SplashDlg::m_staticHwnd = evtdlg->DoModaless(hParentWnd);
        ShowWindow(SplashDlg::m_staticHwnd, SW_SHOW);
        CenterWindow(NULL, SplashDlg::m_staticHwnd);
    }
    //SplashDlg dlg(hinst);
    //dlg.DoModal(NULL);
    return true;
}

void CloseSplashWindow()
{
    if (SplashDlg::m_staticHwnd)
    {
        PostMessage(SplashDlg::m_staticHwnd, WM_COMMAND, IDCANCEL, 0l);
    }
}



SplashDlg::SplashDlg(HINSTANCE hInst)
    : CDlgTemplate(hInst)

{
    SetDlgID((UINT)IDD);
}

SplashDlg::~SplashDlg()
{
}

void SplashDlg::DoDataExchange()
{
}


BOOL SplashDlg::OnInitDialog()
{
    CDlgTemplate::OnInitDialog();
    HWND hDlg = m_hWnd;
    //SetTimer(hDlg, 1, 5000, NULL);
    //uiUtilities::DoEvents(hDlg);
    return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void SplashDlg::OnPaint()
{
    CDlgTemplate::OnPaint();
}

BOOL SplashDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{
    DestroyWindow(m_hWnd);
    return FALSE;
}



void SplashDlg::OnDestroy()
{
    m_staticHwnd = NULL;
    InitInstance(g_APPUI.hInst, SW_SHOW);
}

