
// nxrmDocViewer.h : main header file for the nxrmDocViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CnxrmDocViewerApp:
// See nxrmDocViewer.cpp for the implementation of this class
//

class CnxrmDocViewerApp : public CWinApp
{
public:
	CnxrmDocViewerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
protected:
    ULONG_PTR m_gdiplusToken;
};

extern CnxrmDocViewerApp theApp;
