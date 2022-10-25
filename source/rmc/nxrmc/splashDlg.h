
// nxrmEventViewerDlg.h : header file
//

#pragma once
#include <windows.h>
#include <vector>
#include "..\..\nxrmc\uiutilities\uiutilities.hpp"
#include "..\..\nxrmc\uiutilities\dlgtemplate.hpp"


// CnxrmEventViewerDlg dialog
using namespace uiUtilities;
using namespace std;



#define WM_NX_ADDEVENTLIST		WM_USER + 100
#define WM_NX_ADDFILTERLIST		WM_USER + 101
#define WM_NX_ADDNEWFILTERS     WM_USER + 102


bool OpenSplashWindow(HINSTANCE hinst, HWND hParentWnd);

void CloseSplashWindow();


class SplashDlg : public CDlgTemplate
{
// Construction
public:
    SplashDlg(HINSTANCE hInst);	// standard constructor
    ~SplashDlg();	// standard destructor

    static HWND m_staticHwnd;
// Dialog Data
//#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SPLASH };
//#endif

	protected:
	virtual void DoDataExchange();	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void OnPaint();
    virtual BOOL OnTimer(WPARAM wParam, LPARAM lParam);

	//DECLARE_MESSAGE_MAP()
public:
	virtual void OnDestroy();
	//virtual void OnCancel();


};
