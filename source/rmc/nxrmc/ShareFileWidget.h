
#pragma once
#include "BtnClass.h"
#include "NxMFCListCtrl.h"
#include "OwnerDrawStatic.h"
#include "RepoPage.h"
#include "afxwin.h"
#include "afxcmn.h"
// CShareFileWidget dialog

class CShareFileWidget : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CShareFileWidget)

public:
	CShareFileWidget(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShareFileWidget();

	BOOL m_bProtectFile;
	std::wstring m_strFullPath;

	int m_repoIndex;
	RepositoryFileInfo m_selRepoFile;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHAREFILEWIDGET_DLG };
#endif
protected:
	std::vector<RepositoryInfo> m_RepositoryArr;
	vector<RepositoryData> m_RepoDataArr;	//This is the current directories for each repo.
	vector <RepositoryFileInfo> m_fileData;

	CListBoxCH m_RepoList;

	std::wstring m_strCurRepoParentPath;

	void UpdateRepoFileList(std::vector<RepositoryFileInfo> filelist);
	void RepoOpenFileOrFolder(RepositoryFileInfo &repoFileInfo, int nIdx);
	void RepoGoParentFolder(void);
	void DropOneFile(CString fullpath);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLocaldrive();
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeListRepo();
	CListCtrl m_fileList;
	afx_msg void OnBnClickedOk();

	CString m_strRepoPath;

	CString m_strDragDrop;
	CStatic m_stcDragDrop;
	afx_msg void OnItemdblclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
//	afx_msg void OnIdsDropbox();
	CNxButton m_btnProceed;
	CNxButton m_btnCancel;
	CBtnClass m_btnLocalFiles;
};
