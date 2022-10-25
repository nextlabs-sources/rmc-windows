
// nxrmRMCDlg.h : header file
//

#pragma once


// CnxrmRMCDlg dialog
class CnxrmRMCDlg : public CDialogEx
{
// Construction
public:
	CnxrmRMCDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPLASH };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCancel();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnStnClickedSplash();
};
