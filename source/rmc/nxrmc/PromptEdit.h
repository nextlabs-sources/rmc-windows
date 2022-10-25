#pragma once


// CPromptEdit

class CPromptEdit : public CEdit
{
	DECLARE_DYNAMIC(CPromptEdit)

public:
	CPromptEdit();
	virtual ~CPromptEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    bool m_bEmpty;
    afx_msg void OnEnChange();
    afx_msg void OnPaint();
};


