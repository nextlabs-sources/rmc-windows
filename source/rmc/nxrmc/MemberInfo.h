#pragma once
#include "projsummary.h"
#include "NxButton.h"
#include "afxwin.h"

// CMemberInfo dialog

class CMemberInfo : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CMemberInfo)

public:
	CMemberInfo(const ProjMember &memberData, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMemberInfo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJ_MEMBER_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    ProjMember m_memberData;
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    // Picture control
    CNxButton m_person;
    CStatic m_name;
    CStatic m_email;
    CStatic m_date;
    CNxButton m_btnResend;
    // //Revoke button
    CNxButton m_btnRevoke;
    // remove member button
    CNxButton m_btnRemove;
    CStatic n_invite;
    afx_msg void OnBnClickedButtonRevoke();
    afx_msg void OnBnClickedButtonRemove();
    afx_msg void OnBnClickedButtonResend();
};
