#pragma once
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "BtnClass.h"
#include "ListBoxCH.h"
#include <vector>

#include <nx\rmc\session.h>
#include <nx\common\stringex.h>
#include <nx\common\time.h>
#include <RepoPage.h>


// CConnectToRepo dialog
using namespace std;

class CConnectToRepo : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CConnectToRepo)

public:
	CConnectToRepo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectToRepo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONNECT_REPO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	std::vector<RepositoryInfo> m_reposArr;
	vector<int> m_idxArr;
public:
    CListBoxCH m_repoList;
	CStringArray m_DisplayName;
    virtual BOOL OnInitDialog();
	void RefreshRepoList();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnLbnSelchangeListRepo();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    CStatic m_addRepoStatic;
	bool m_needRefresh;
	afx_msg void OnBnClickedAddrepository();
	CBtnClass m_btnAddRepo;
};
