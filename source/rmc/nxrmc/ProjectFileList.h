#pragma once
#include "afxwin.h"
#include "nxrmRMC.h"
#include "afxcmn.h"
#include "FlatDlgBase.h"
#include "OwnerDrawStatic.h"
#include "BtnClass.h"

// CProjectFileList dialog
typedef struct {
    NxListType itemType;
    CString fileName;
    NX::time::datetime dt;
    long long  fileSize;
    CString ownerEmail; 
    CString fileType;
    wstring filePath;
    wstring duid;
    int imageID;
    NX::RmProjectFile repoFile;
} ProjFileData;



class CProjectFileList : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjectFileList)

public:
	CProjectFileList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectFileList();
    void SetProject(const CProjData &projData);

    void RefreshPage();

    void ConvertDataToFileData();


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJ_FILE_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    ::SORTORDER m_sortedType;

    vector<ProjFileData> m_fileData;

	DECLARE_MESSAGE_MAP()
    CString m_currDir;
    CString m_currDispDir;
    bool m_searchMode;
    CString m_searchString;

    int m_iCurSelectedListColumn;
    BOOL m_bColumnAscend;
    POINT m_topPos;


    vector<CtrlSizeData> m_ctrlData;


public:
    virtual BOOL OnInitDialog();
    void DisplayAllFiles();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    void SortColumn(int columnindex, BOOL bRefresh = FALSE);
    virtual void OnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
    COwnerDrawStatic m_searchGroup;
    CBtnClass m_resetSearchBtn;
    CEdit m_searchEditBox;
    CBtnClass m_searchBtn;
    CNxMFCListCtrl m_fileList;
    CBtnClass m_uploadBtn;
    CBtnClass m_refreshBtn;
    CStatic m_titleName;
    CStatic m_filePath;
    CBtnClass m_btnAddDir;
    CProjData m_projData;
    CBrush m_gray;
//    afx_msg void OnRdblclkListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnRclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
//    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    afx_msg void OnDblclkListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
    void OpenFileOrFolder(int idx);
    afx_msg void OnBnClickedButtonBack();
    afx_msg void OnBnClickedButtonResetSearch();
    afx_msg void OnBnClickedButtonSearch();
    void SearchFilesUsingSearchText();
    void ShowSortedFileList();
    virtual void OnOK();
    void ResetSearch();
    void SearchProjectFiles(const wstring currentPath, CProjData & projData, const wstring searchString);
    virtual void OnCancel();
    afx_msg void OnStnClickedStaticSearchgroup();
    afx_msg void OnPaint();
//    CBtnClass m_btnGoback;
//    CBtnClass m_btnForward;
    CBtnClass m_projName;
    CBtnClass m_btnRefresh;
    afx_msg void OnBnClickedButtonGoback2();
    afx_msg void OnBnClickedButtonForward();
    afx_msg void OnBnClickedButtonUpload();
    virtual void DropOneFile(CString folderToSave, CString fileToSave);
    afx_msg void OnBnClickedButtonAddDir();
    afx_msg void OnBnClickedButtonRefresh();
    afx_msg void OnFilecontextmenuFileproperties();
    afx_msg void OnFilecontextmenuDelete();
    afx_msg void OnFilecontextmenuDownload();
    afx_msg void OnFilecontextmenuView();
    BOOL IsFileInfoAvailable();
    BOOL IsFileDeleteable();
    afx_msg void OnProjectcontextmenuViewactivity();
protected:
public:
    afx_msg void OnLvnItemchangedListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
    int ResizeWindow(int cx, int cy);
    int GetFileListRepoDataIndex(int nsel = -1);
    void SetAllOrignRects();
    void ResizeRepoFileListControl();
    afx_msg LRESULT OnNxRefreshProjfilelist(WPARAM wParam, LPARAM lParam);
    CStatic m_stcHorizLine;
    CStatic m_stcVertLine;
    CRect m_rcBound;


    void SetLeftTopPosition(CPoint pt) { m_topPos = pt; };

    CStatic m_searchSep;
    CStatic m_navBand;
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    BOOL IsWindowVisible();
};
