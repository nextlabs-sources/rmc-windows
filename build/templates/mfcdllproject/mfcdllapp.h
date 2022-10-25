// mfcdllproject.h : main header file for the mfcdllproject DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMfcDllApp
// See dllmain.cpp for the implementation of this class
//

class CMfcDllApp : public CWinApp
{
public:
    CMfcDllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
