#pragma once
#include "resource.h"
#include "afxwin.h"
#include "DlgProtectShareFile.h"
//#include "RepoLabel.h"


// CNXHomePage dialog

class CNXHomePage : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CNXHomePage)

public:
	CNXHomePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNXHomePage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOME1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    vector <CDialogEx *> m_myProjArr;
    vector <CDialogEx *> m_otherProjArr;
    vector<CDialogEx *> m_repoLabels;
    int m_repoHeight;
    int m_repoRowCnt;

	void InitialRepoWindows(void);
	void InitialProjWindows(void);
public:
    virtual BOOL OnInitDialog();
    void RefreshPage();
    void UpdateStorageInfo();
    void UpdateUserInfo();
    void UpdateRepositoryList();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    CStatic m_welcome;
    COwnerDrawStatic m_myDriveLabel;
    COwnerDrawStatic m_myVaultLabel;
    CStatic m_stMydriveSize;
    CStatic m_myVaultSize;
    COwnerDrawStatic m_storageChart;
    CStatic m_usedBytes;
    CStatic m_freeBytes;
    // m_btnViewMyDrive
    CStatic m_connectRepoLabel;
    // m_projCreatedByMeLabel
    CStatic m_projCreatedByMeLabel;
    CStatic m_projInvited;
    CBtnClass m_btnProtect;
    CBtnClass m_btnShare;
    CBtnClass m_btnConnectRepo;

    CBtnClass m_viewMyDrive;
    CBtnClass m_viewMyVault;
    CBtnClass m_btnMydrive;
    CBtnClass m_btnMyVault;
    CNxButton m_btnPerson;
    CBtnClass m_gotoMySpace;
    CBtnClass m_gotoArrow;
    CStatic m_storageGroup;
    afx_msg
        void DisplayProjectList();
    void SetScrollbar();
    void MoveOneWindow(CWnd * pWnd, int deltaY);
    void AddWindowHeight(CWnd * pWnd, int deltaY);
    CRect MoveAllControls(int deltaY);
    void OnBnClickedButtonMvViewMyVaultfile();
    afx_msg void OnBnClickedPersonButton();
    void EditUserProfile();
    afx_msg void OnBnClickedButtonProtect();
    afx_msg void OnBnClickedButtonShare();
    afx_msg void OnBnClickedConnectRepository();
    afx_msg void OnBnClickedGotoMyspace();
    afx_msg void OnBnClickedButtonArrowNext();
    afx_msg void OnBnClickedButtonMdViewMyDrivefile();
    CBtnClass m_BtnViewAllProj;
    afx_msg void OnBnClickedButtonViewallproj();
    afx_msg LRESULT OnRefreshProjMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshRepoList(WPARAM wParam, LPARAM lParam);

    CStatic m_headerGroup;
    CNxButton m_btnActivateProj;
    CStatic m_strActivate;
    CStatic m_grpActivate;
    afx_msg void OnBnClickedButtonActivate();
    CStatic m_noRepoLabel;

	void OnManageLocalFile(void);
protected:
    afx_msg LRESULT OnNxRefreshHomePage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
    void UpdateProjWnd(CProjData & projData);
protected:
    afx_msg LRESULT OnNxTimedRefreshProjlist(WPARAM wParam, LPARAM lParam);
};
