#pragma once
#include "afxwin.h"
#include "NxButton.h"
#include "projmember.h"

#define NX_MEMBER_WINDOW_SPACE_RATIO       6/5
class CProjMembers : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjMembers)

public:
	CProjMembers(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjMembers();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJ_MEMBERS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    void SearchMembers(const wstring & keyWord);
    CStatic m_titleName;
    CComboBox m_sortList;
    CBtnClass m_inviteBtn;
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    CBtnClass m_searchBtn;
//    CBtnClass m_btnGoBack;
//    CBtnClass m_btnForward;
    CStatic m_projName;
    CBtnClass m_btnRefresh;
    CEdit m_searchEditBox;
    CBtnClass m_resetSearchBtn;
    CStatic m_navBand;
    CBrush m_gray;
    bool m_searchMode;
    CString m_searchString;
    const CProjData *m_pProjData;
    vector<ProjMember> m_members;
    vector<ProjMember> m_pendingMembers;
    vector<CProjMember*> m_memberArr;
    vector<CProjMember*> m_pendingMemberArr;
    wstring m_ownerId;
    int m_nMaxPrjInRow;
    CRect m_rcMember;
    POINT m_topPos;

    CRect m_originRect;
    afx_msg void OnBnClickedButtonGoback2();
    afx_msg void OnBnClickedButtonForward();
    afx_msg void OnBnClickedButtonRefresh();
    afx_msg void OnBnClickedButtonSearch2();
    afx_msg void OnBnClickedButtonInvite();
    void CreateMemberWindows();
    void SetProject(const CProjData * projData);
    void ListMembers();
    CStatic m_pendingLabel;
    vector<NX::RmProjectInvitation> m_invites;
    afx_msg void OnClickedButtonResetSearch();
    void ResetSearch();
    int ResizeWindow(int cx, int cy);
    void SetAllOrignRects();
    BOOL IsWindowVisible();
    CStatic m_vertSep;
    CStatic m_stcHorizLine;
    CRect m_rcBound;
    vector<CtrlSizeData> m_ctrlData;

    void SetLeftTopPosition(CPoint pt) { m_topPos = pt; };

    CStatic m_searchSep;
};
