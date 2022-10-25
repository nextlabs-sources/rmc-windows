#pragma once
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "afxcmn.h"


// CAddRepository dialog

class CAddRepository : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CAddRepository)

public:
	CAddRepository(CStringArray &RepoDisplayNames, CWnd* pParent = NULL );   // standard constructor
	virtual ~CAddRepository();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_REPO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CStringArray m_strDisplayNames;
public:
    virtual BOOL OnInitDialog();
    ::CComboBox m_repoTypeList;
    afx_msg void OnBnClickedButtonAddRepo();
	BOOL CreateIconList(CImageList & imgList);
	CComboBoxEx m_typeList;
	int m_iSelType;
	CImageList m_repoImageList;
	afx_msg void OnCbnSelchangeComboboxexRepotype();
	CString m_strDisplayName;
	CEdit m_edtDisplayName;
	CEdit m_edtSiteURL;
	CStatic m_stcSiteURL;
	CString m_strSiteURL;
	CBtnClass m_btnCancel;
	CBtnClass m_btnConnect;
	afx_msg void OnClickedCancel();
	afx_msg void OnChangeEdtSiteurl();
	afx_msg void OnChangeEditLoginname2();
	afx_msg void OnIdok();
};
