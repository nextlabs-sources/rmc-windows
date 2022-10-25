
// nxmaindlg.h : header file
//

#pragma once

#include <nx\rmc\keym.h>

#include "localviewerdlg.h"
#include "remoteviewerdlg.h"

// CViewMainDlg dialog
class CViewMainDlg : public CDialogEx
{
// Construction
public:
	CViewMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NXRVIEWER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


private:
	CLocalViewerDlg*	m_pLocalViewer;
	CRemoteViewerDlg*	m_pRemoteViewer;
	NX::RmClientId		m_clientId;

	void SaveViewerPosition();
	void LoadViewerPosition(LPRECT rect);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
