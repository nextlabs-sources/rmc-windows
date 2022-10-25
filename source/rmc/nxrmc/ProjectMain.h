#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"
#include "BtnClass.h"
#include "projectfilelist.h"
#include "ProjSummary.h"
#include "ProjMembers.h"
#include "ProjData.h"
#include "switchproj.h"
#include <queue>

// CProjectMain dialog

enum ProjPage 
{
    NX_PROJ_SUMMARY_PAGE = 0,
    NX_PROJ_FILES_PAGE,
    NX_PROJ_PEOPLE_PAGE,
    NX_PROJ_SWITCH_PAGE
};

class CProjectMain : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjectMain)

public:
    CProjectMain(CWnd* pParent = NULL);   // standard constructor
    void SetProject(const CProjData & projData);
	void SetProject(void);
    virtual ~CProjectMain();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RMCMAIN_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    CProjData m_projData;
    vector<CDialogEx*> m_dlgArray;
    vector<CBtnClass*> m_btnArr;
    ProjPage m_pageIdx;
    CPoint m_ptSubPage;

    int m_initRight;
    int m_initBtnRight;
    int m_initCloseRight;
    vector<CtrlSizeData> m_ctrlData;
    bool m_needUpdateMembers;
    bool m_needUpdateFiles;


   
public:
    CProjMembers m_membersDlg;
    CProjectFileList m_fileDlg;
    CProjSummary m_summaryDlg;
    CSwitchProj m_switchDlg;

    CBrush m_br;
    CStatic m_allProjText;
    CStatic m_projName;
    CBtnClass m_btnShowAllProj;
    CBtnClass m_btnSummary;
    CBtnClass m_btnFiles;
    CBtnClass m_btnPeople;
    CStatic m_projNameLeft;
    bool m_bSwitchpage;
    FromPage m_page;
    void ShowProjPage(ProjPage pageIdx);
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    CStatic m_projCaption;
    afx_msg void OnStnClickedStaticHightlite();
    afx_msg void OnBnClickedButtonAllproject();
    afx_msg void OnBnClickedGotoAllProj();
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnDestroy();
    afx_msg void OnClickedRadioPeople();
    afx_msg void OnClickedRadioFiles();
    afx_msg void OnClickedRadioSummary();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClickedButtonClose();
    void SetReturnPage(FromPage page);
    int ResizeWindow(int cx, int cy);
	void RepositionCaption();
	wstring GetCurrProjID()
	{
		if (m_projData.m_flag == NXRMC_PROJ_NOT_VALID)
			return L"";
		return m_projData.m_nID;
	}
protected:
    afx_msg LRESULT OnNxTimedRefreshOneProj(WPARAM wParam, LPARAM lParam);
    void SetAllOrignRects();
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnStnClickedAllprojText();
protected:
    afx_msg LRESULT OnNxUpdateWindowSize(WPARAM wParam, LPARAM lParam);
};
