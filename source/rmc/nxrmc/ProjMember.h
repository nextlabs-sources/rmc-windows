#pragma once
#include "afxwin.h"
#include "projsummary.h"
#include "BtnClass.h"



// CProjMember dialog

class CProjMember : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjMember)

public:
    CProjMember(ProjMember & memberData, bool bPending=false, CWnd * pParent=NULL);
    // standard constructor
	virtual ~CProjMember();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEMBER };
#endif

protected:
    ProjMember m_memberData;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CStatic m_joinDate;
    CStatic m_memberName;
    CStatic m_hostLabel;
    bool m_bPending;
    virtual BOOL OnInitDialog();
	void UpdateMember(ProjMember & member);
	void RefreshMember();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    CBtnClass m_btnHost;
    CNxButton m_person;
    afx_msg void OnDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedButtonPerson();
    virtual void OnOK();
    virtual void OnCancel();
};
