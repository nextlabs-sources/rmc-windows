#pragma once
#include "afxwin.h"
//#include "nxrmRMC.h"
#include "OwnerDrawStatic.h"
#include "btnclass.h"
#include "NxButton.h"
#include "projdata.h"
#include "resource.h"
#include <queue>
#include <map>
#include "nx\common\time.h"
//#include "ProjectFileList.h"

// CProjSummary dialog

typedef struct
{
    int btnID;
    int descID;
    int sepID;
    int mbID;
} ProjListItem;



typedef struct
{
    wstring fileName;
    wstring desc;
} ProjFile;

typedef struct
{
    bool bProjByMe;     //Project created by me
    bool bPending;      //This is pending member
    bool bOwner;        //This is the owner
    wstring projID;
    wstring memberID;
    wstring memberName;
    wstring invitorName;
    wstring desc;
    wstring emailAddr;
    NX::time::datetime dt;
} ProjMember;


class CProjSummary : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjSummary)
#define MAX_ITEMS   8
    const ProjListItem m_fileIDArr[MAX_ITEMS] =
    {
        { IDC_BTN_FILE_NAME1,IDC_STATIC_FILE_DESC1,IDC_ST_HLINE2,IDC_MB_LABEL1},
        { IDC_BTN_FILE_NAME2,IDC_STATIC_FILE_DESC2,IDC_ST_HLINE3,IDC_MB_LABEL2},
        { IDC_BTN_FILE_NAME3,IDC_STATIC_FILE_DESC3,IDC_ST_HLINE4,IDC_MB_LABEL3},
        { IDC_BTN_FILE_NAME4,IDC_STATIC_FILE_DESC4,IDC_ST_HLINE5,IDC_MB_LABEL4},
        { IDC_BTN_FILE_NAME5,IDC_STATIC_FILE_DESC5,IDC_ST_HLINE6,IDC_MB_LABEL5},
        { IDC_BTN_FILE_NAME6,IDC_STATIC_FILE_DESC6,IDC_ST_HLINE7,IDC_MB_LABEL6},
        { IDC_BTN_FILE_NAME7,IDC_STATIC_FILE_DESC7,IDC_ST_HLINE8,IDC_MB_LABEL7},
        { IDC_BTN_FILE_NAME8,IDC_STATIC_FILE_DESC8,IDC_ST_HLINE9,IDC_MB_LABEL8}
    };

    const ProjListItem m_memberIDArr[MAX_ITEMS] =
    {
        { IDC_BTN_FILE_NAME9 ,IDC_STATIC_FILE_DESC9 ,IDC_ST_HLINE11,0 },
        { IDC_BTN_FILE_NAME10,IDC_STATIC_FILE_DESC10,IDC_ST_HLINE12,0 },
        { IDC_BTN_FILE_NAME11,IDC_STATIC_FILE_DESC11,IDC_ST_HLINE13,0 },
        { IDC_BTN_FILE_NAME12,IDC_STATIC_FILE_DESC12,IDC_ST_HLINE14,0 },
        { IDC_BTN_FILE_NAME13,IDC_STATIC_FILE_DESC13,IDC_ST_HLINE15,0 },
        { IDC_BTN_FILE_NAME14,IDC_STATIC_FILE_DESC14,IDC_ST_HLINE16,0 },
        { IDC_BTN_FILE_NAME15,IDC_STATIC_FILE_DESC15,IDC_ST_HLINE17,0 },
        { IDC_BTN_FILE_NAME16,IDC_STATIC_FILE_DESC16,IDC_ST_HLINE18,0 }
    };
    
    const map<int, int> idToFileIdx =
    {
        {IDC_BTN_FILE_NAME1,0 },
        {IDC_BTN_FILE_NAME2,1 },
        {IDC_BTN_FILE_NAME3,2 },
        {IDC_BTN_FILE_NAME4,3 },
        {IDC_BTN_FILE_NAME5,4 },
        {IDC_BTN_FILE_NAME6,5 },
        {IDC_BTN_FILE_NAME7,6 },
        {IDC_BTN_FILE_NAME8,7 }
    };

    const map<int, int> idToMemberIdx =
    {
        { IDC_BTN_FILE_NAME9 ,0 },
        { IDC_BTN_FILE_NAME10,1 },
        { IDC_BTN_FILE_NAME11,2 },
        { IDC_BTN_FILE_NAME12,3 },
        { IDC_BTN_FILE_NAME13,4 },
        { IDC_BTN_FILE_NAME14,5 },
        { IDC_BTN_FILE_NAME15,6 },
        { IDC_BTN_FILE_NAME16,7 }
    };


    typedef struct
    {
        //CWnd file;
        CWnd date;
        CWnd size;
        CWnd sep;

    } FileCtrl;

    typedef struct
    {
        //CWnd file;
        CWnd date;
        //CWnd size;
        CWnd sep;

    } MemberCtrl;

public:
	CProjSummary(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjSummary();

    void SetProject();
    CProjData m_projData;


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJ_SUMMARY };
#endif
    void SetProject(const CProjData &projData);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    //queue<NX::RmProjectFile> m_fileQueue;
    //queue<NX::RmProjectMember> m_memberQueue;
    vector<ProjFile> m_files;
    vector<ProjMember> m_members;
    vector<NX::RmProjectFile> m_fileData;
    vector<CtrlSizeData> m_ctrlData;
    CPoint m_topPos;
    size_t m_fileItemStart;
    size_t m_memberItemStart;
   

public:
    void SortedFileDescendByTime();
    void SortedMemberDescendByTime();
    void ClearFileList();
    void ShowFileList();
    void ClearMemberList();
    void ShowMemberList();
    void InitializeButtons();
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    CStatic m_desc_val;
    //CStatic m_descGroup;
    afx_msg void OnStnClickedStaticFileDesc8();
    afx_msg void OnStnClickedStaticDescGroup();
    CStatic m_descTitle;
    CNxButton m_addFile;
    CNxButton m_viewAllFiles;
    CNxButton m_invite;
    CNxButton m_viewAllMember;
    afx_msg void OnBnClickedButtonInvitemember();
    afx_msg void OnBnClickedBtnViewAllMember();
    afx_msg void OnBnClickedButtonAddfile();
    void OpenSharingDlg(HWND hWnd);
    afx_msg void OnBnClickedBtnViewAll();
    CNxButton m_fileBtns[MAX_ITEMS];
    CNxButton m_memberBtns[MAX_ITEMS];

    FileCtrl    m_fileCtrls[MAX_ITEMS];
    MemberCtrl m_memberCtrls[MAX_ITEMS];

    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnPaint();
    CStatic m_peopleLabel;
    CStatic m_fileLabel;
    afx_msg void OnStnClickedStHline9();
    //COwnerDrawStatic m_memberPictures;
    
    afx_msg LRESULT OnNxRefreshProjfilelist(WPARAM wParam, LPARAM lParam);

    int ResizeWindow(int cx, int cy);


    void SetAllOrignRects();
    void SetLeftTopPosition(CPoint pt) { m_topPos = pt; };

protected:
public:
    CStatic m_noFileLabel;
    CStatic m_baseSepLine;
    CStatic m_stcVertLine;
    CStatic m_grpBox;
    CRect m_rcBound;
    CStatic m_memberBaseLine;
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    BOOL IsWindowVisible();
};

