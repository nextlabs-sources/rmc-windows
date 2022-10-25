#pragma once


// CNxMFCListCtrl

class CNxMFCListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CNxMFCListCtrl)

public:
	CNxMFCListCtrl();
	virtual ~CNxMFCListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDropFiles(HDROP hDropInfo);
};


