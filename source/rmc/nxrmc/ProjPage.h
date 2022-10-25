#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"
//#include "ProjListDlg.h"
#include "NxButton.h"

#define NX_PROJECT_SPACE_DIVISOR    10
#define NX_ONE_PROJECT_SPACE_RATIO  11 / 10

// CProjPage dialog

class CProjPage : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjPage)

public:
	CProjPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
	void RefreshProjectList();
    afx_msg void OnIdok();
    afx_msg void OnDestroy();
    ::CStatic m_pageTitle;
    CStatic m_ProjByMe;
    CStatic m_projByOthers;
	//CProjListDlg  *m_pProjList;
	//CProjListDlg  *pProjOtherList;
protected:
	afx_msg LRESULT OnNxRefreshProjList(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedButtonCreate();
    void RefreshProjList(BOOL bUpdateData=FALSE);
    void AutoWrapProjectList();
    void DisplayProjectList();
    void UpdateProjWnd(CProjData & projData);
    int ResizeWindow(int cx, int cy);
    void SetAllOrignRects();
protected:
    CNxButton m_btnCreate;
    CStatic m_meLabel;
    CStatic m_othersLabel;
    vector <CDialogEx *> m_projArr;
    vector <CDialogEx *> m_projOtherArr;
    int m_nMaxPrjInRow;
    CRect m_rcProj;
    vector<CtrlSizeData> m_ctrlData;
    afx_msg LRESULT OnNxTimedRefreshProjlist(WPARAM wParam, LPARAM lParam);

    vector<CProjData> m_prjDataFromMe;
    vector<CProjData> m_prjDataFromOthers;
    vector<CProjData> m_prjDataInvites;

};
