#pragma once
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "resource.h"
#include "BtnClass.h"
#include "NxButton.h"


// CIntroPage dialog
#define NX_RMC_MAX_INTROIMAGES  4
#define NX_RMC_ANINATION_FRAMES 10

class CIntroPage : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CIntroPage)

public:
	CIntroPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIntroPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INTRO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    int m_nImgIndex;
    int m_nAnimPos;
    bool m_bGoRight;
    const int ImageArray[NX_RMC_MAX_INTROIMAGES] = { IDB_INTRO1,IDB_INTRO2,IDB_INTRO3,IDB_INTRO4};
    const int ProjImageArray[NX_RMC_MAX_INTROIMAGES] = { IDB_PROJ_INTRO1,IDB_PROJ_INTRO2,IDB_PROJ_INTRO3,IDB_PROJ_INTRO4};

    HBITMAP m_imageHandles[NX_RMC_MAX_INTROIMAGES];

    const int stepImageArray[NX_RMC_MAX_INTROIMAGES] = { IDB_STEP1,IDB_STEP2,IDB_STEP3,IDB_STEP4};
    HBITMAP m_stepimageHandles[NX_RMC_MAX_INTROIMAGES];


    CToolTipCtrl m_ToolTip;
    bool m_isProjIntro;
    CBitmap m_bmpDesc;
    vector<CtrlSizeData> m_ctrlData;
    //vector<CRect> m_originRects;
    //vector<CWnd*> m_ctrls;
public:
	void ResetIndex(void) { m_nImgIndex = 0; 	m_introImage.SetBitmap(m_imageHandles[m_nImgIndex]);}
    virtual BOOL OnInitDialog();
    void SetChildRect(CWnd * pWnd);
    void SetAllOrignRects();
    afx_msg void OnBnClickedButtonNext();
    afx_msg void OnBnClickedButtonPrev();
    afx_msg void OnBnClickedButtonSignup();
    afx_msg void OnBnClickedButtonLogin();
    COwnerDrawStatic m_introImage;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedSkipLogin();
    afx_msg void OnPaint();
    CBtnClass m_nextBtn;
    CBtnClass m_prevBtn;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnIdcancel();
    CBtnClass m_signupBtn;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    void StartAnimation(bool bGoRight);
    void ShowImageInPos(int nAnimPos);
    virtual void OnOK();
    void SetProjFlag() { m_isProjIntro = true;  }

	int m_iWindowWidth;
	int m_iWindowHight;
    afx_msg void OnDestroy();
    int ResizeWindow(int cx, int cy);
    COwnerDrawStatic m_stepImage;
    CBtnClass m_btnLogin;
    CBtnClass m_btnLogo;
    CStatic m_mainHelp;
    CStatic m_mainHelp1;
    CStatic m_mainHelp2;
    CStatic m_mainHelp3;
    CStatic m_mainHelp4;
    CStatic m_mainTitle;
    //CStatic m_already;
    CStatic m_sep;

   
    COwnerDrawStatic m_introDesc;
    CBtnClass m_btnAlready;

    CGdiPlusBitmapResource* m_pBitmap;
};
