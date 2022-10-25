#pragma once
#include "afxwin.h"


// CInvitationDecline dialog

class CInvitationDecline : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CInvitationDecline)

public:
	CInvitationDecline(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInvitationDecline();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJ_INVITE_DECLINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    CEdit m_declineText;
    CBtnClass m_btnDecline;
    CBtnClass m_btnCancel;
    CString m_outText;
};
