#if !defined(AFX_LISTBOXCH_H__C572274F_C595_11D6_8D12_004033579553__INCLUDED_)
#define AFX_LISTBOXCH_H__C572274F_C595_11D6_8D12_004033579553__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxCH.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxCH window
#define MAXITEMSTRING	256
#include <afxtempl.h>
class CExtItem
{
public:
	CExtItem(LPCTSTR szItemName, int nIcoNormalID, int nIcoSelID, LPCTSTR emailAddress)
	{
		lstrcpyn(m_szItemName,szItemName, MAXITEMSTRING);
		m_hIcon = AfxGetApp()->LoadIcon(nIcoNormalID);
        m_hSelIcon = AfxGetApp()->LoadIcon(nIcoSelID);
        lstrcpyn(m_emailAddr, emailAddress, MAXITEMSTRING);
    }
    
    CExtItem(LPCTSTR szItemName, int nIcoID, LPCTSTR emailAddress) : m_hSelIcon(0)
    {
        lstrcpyn(m_szItemName, szItemName, MAXITEMSTRING);
        m_hIcon = AfxGetApp()->LoadIcon(nIcoID);
        lstrcpyn(m_emailAddr, emailAddress, MAXITEMSTRING);
    }

    ~CExtItem()
    {
		DestroyIcon(m_hIcon);
		DestroyIcon(m_hSelIcon);
    }

public:
	TCHAR   m_szItemName[MAXITEMSTRING];
	HICON	m_hIcon;
    HICON   m_hSelIcon;
    TCHAR   m_emailAddr[MAXITEMSTRING];
};

enum ListItemDisplayStyle {
	NX_ONE_LINE_ITEM, NX_TWO_LINE_ITEM,
};
class CListBoxCH : public CListBox
{
// Construction
public:
	CListBoxCH();

// Attributes
public:
	void ReleaseItems(void);
// Operations
public:

    void AddItem(LPCTSTR lpszItemName, int nIcoID, LPCTSTR lpszEmailAddr = L"");
    int AddItem(LPCTSTR lpszItemName, int nIcoNormalID, int nIcoSelID, LPCTSTR lpszSecondLine=L"");
    void SetSelColor(COLORREF clr);
	void SetBgColor(COLORREF clr);
	void SetCurSel(int curSel);
	void SetTextColor(COLORREF clr);
	void EnableEdge(BOOL bEnable);
	void SetItemDisplayStyle(ListItemDisplayStyle itemStyle);
	int  GetCurSel();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxCH)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual int VKeyToItem(UINT nKey, UINT nIndex);
	protected:
	virtual void PreSubclassWindow();
    virtual int CompareItem(LPCOMPAREITEMSTRUCT)
    {
        return 0;
    }

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxCH();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxCH)
	afx_msg void OnSelchange();
	afx_msg void OnLButtonUp(UINT nFlags, ::CPoint point);
	//}}AFX_MSG
	CTypedPtrList<CPtrList,CExtItem *> m_pItemList;	// item list
	COLORREF			m_clrSel;	   // select color
	COLORREF			m_clrBg;	   // background color
	COLORREF			m_clrText;	   // text color
	COLORREF			m_clrSecondText;	   // text color
	BOOL				m_bEdge;	   // edge line
	int					m_curSel;	   // current select index base on zero
	ListItemDisplayStyle m_itemStyle;  // one line or 2 line

	
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	UINT ItemFromPoint2(CPoint pt, BOOL & bOutside) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXCH_H__C572274F_C595_11D6_8D12_004033579553__INCLUDED_)
