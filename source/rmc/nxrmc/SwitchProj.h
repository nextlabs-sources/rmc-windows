#pragma once
#include "afxwin.h"
#include <vector>
#include "ProjData.h"


// CSwitchProj dialog
using namespace std;

// CProjListDlg dialog

enum ProjListType
{
    NX_HOME_PROJ_LIST, NX_HOME_PROJ_LIST_BY_OTHERS, NX_PROJ_PROJ_LIST, NX_PROJ_PROJ_LIST_BY_OTHERS
};




class CSwitchProj : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CSwitchProj)

public:
	CSwitchProj(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSwitchProj();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECT_SWITCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
    int m_nMaxPrjInRow;
    CRect m_rcProj;
    vector<CtrlSizeData> m_ctrlData;
    POINT m_topPos;

    vector<CProjData> m_prjDataFromMe;
    vector<CProjData> m_prjDataFromOthers;
    vector<CProjData> m_prjDataInvites;

public:
    virtual BOOL OnInitDialog();
    void RefreshProjList(BOOL bUpdateData = FALSE);
    void AutoWrapProjectList();
    //void RefreshProjList();
    void DisplayProjectList();
    vector <CDialogEx *> m_projArr;
    vector <CDialogEx *> m_projOtherArr;
    ProjListType m_prjType;
    CRect m_rcBound;


    

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    CStatic m_switchTitle;
    CStatic m_otherProjLabel;
    CStatic m_projByMeLabel;
	afx_msg void OnDestroy();
    void UpdateProjWnd(CProjData & projData);
    void SetChildRect(CWnd * pWnd);
    void SetAllOrignRects();
    int ResizeWindow(int cx, int cy);
    BOOL IsWindowVisible();
    void SetLeftTopPosition(CPoint pt) { m_topPos = pt; };
protected:
};
